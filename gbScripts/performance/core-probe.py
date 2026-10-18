"""Install/remove temporary probe while preserving exact pre-existing source bytes.
Run from repo root with Python 2.7 or 3. Only Vulkan audit builds are supported.
"""
from __future__ import print_function
import os, sys, json, hashlib, shutil
root = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
backup = os.path.join(root, 'gbDemo', 'output', 'island-check', 'core-audit', 'probe-backup')
manifest = os.path.join(backup, 'manifest.json')
def digest(data): return hashlib.sha256(data).hexdigest()
def replace(data, old, new):
    old, new = old.encode('utf8'), new.encode('utf8')
    if data.count(old)!=1: raise RuntimeError('Expected one match: '+repr(old))
    return data.replace(old,new)
files = {
 'ces_systems_feeder.cpp': [
  ('            if (profile && frames >= 60)\n                samples[std::string(typeid(*system).name()) + "/" + phase] += std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count();',
   '            if (profile && frames >= 60) {\n                const double ms=std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count();\n                const std::string name=std::string(typeid(*system).name()) + "/" + phase;\n                samples[name]+=ms;\n                if(gb_probe::enabled() && ms>5.)std::cout<<"PROBE_SYSTEM_SPIKE frame="<<frames<<" name="<<name<<" ms="<<ms<<"\\n";\n            }')],
 'island_demo_scene.cpp': [
  ('[labels,resolution,elapsed=0.f](const gb::ces_entity_shared_ptr&,float dt) mutable {','[labels,resolution,elapsed=0.f,initialized=false](const gb::ces_entity_shared_ptr&,float dt) mutable {\n            if(std::getenv("GB_PROBE_HUD_FREEZE") && initialized)return;'),
  ('            if(elapsed<.25f)return;','            if(elapsed<.25f)return;\n            initialized=true;')],
 'game_conroller_win32.cpp': [('glm::ivec2 resolution = glm::ivec2(1280, 720);','glm::ivec2 resolution = glm::ivec2(1280, 720);\n        if(std::getenv("GB_PROBE_WIDTH") && std::getenv("GB_PROBE_HEIGHT")) {\n            int w=std::atoi(std::getenv("GB_PROBE_WIDTH")),h=std::atoi(std::getenv("GB_PROBE_HEIGHT"));\n            if(w>=128 && w<=4096 && h>=128 && h<=4096) resolution=glm::ivec2(w,h);\n        }\n        std::cout<<"PROBE resolution="<<resolution.x<<"x"<<resolution.y<<std::endl;')],
 'graphics_context_win32.cpp': [
  ('\t\tvkDeviceWaitIdle(vk_device::get_instance()->get_logical_device());','\t\tvkDeviceWaitIdle(vk_device::get_instance()->get_logical_device());\n        gb_probe::destroy();'),
  ('if (strcmp(layer.layerName, "VK_LAYER_KHRONOS_validation") == 0)','if (strcmp(layer.layerName, "VK_LAYER_KHRONOS_validation") == 0 && std::getenv("GB_PROBE_VALIDATION_OFF")==nullptr)'),
  ('VkResult result = vkWaitForFences(logical_device, 1, &in_flight_fence, VK_TRUE, UINT64_MAX);','VkResult result; { gb_probe::scope probe("wait_frame_fence"); result = vkWaitForFences(logical_device, 1, &in_flight_fence, VK_TRUE, UINT64_MAX); }'),
  ('result = vk_swap_chain::get_instance()->acquire_next_image(image_available_semaphore, &current_image_index);','{ gb_probe::scope probe("acquire_image"); result = vk_swap_chain::get_instance()->acquire_next_image(image_available_semaphore, &current_image_index); }'),
  ('result = vkWaitForFences(logical_device, 1, &m_vk_images_in_flight[current_image_index], VK_TRUE, UINT64_MAX);','{ gb_probe::scope probe("wait_image_fence"); result = vkWaitForFences(logical_device, 1, &m_vk_images_in_flight[current_image_index], VK_TRUE, UINT64_MAX); }'),
  ('result = vkQueueSubmit(graphics_queue, 1, &submit_info, in_flight_fence);','{ gb_probe::scope probe("queue_submit"); result = vkQueueSubmit(graphics_queue, 1, &submit_info, in_flight_fence); }'),
  ('result = vk_swap_chain::get_instance()->queue_present(present_queue, current_image_index, render_finished_semaphore);','{ gb_probe::scope probe("queue_present"); result = vk_swap_chain::get_instance()->queue_present(present_queue, current_image_index, render_finished_semaphore); }')],
 'render_pipeline.cpp': [
  ('        render_statistics::begin_frame();','        gb_probe::frame_begin();\n        render_statistics::begin_frame();'),
  ('        m_graphics_context->make_current();','        { gb_probe::scope probe("acquire_fences"); m_graphics_context->make_current(); }'),
  ('        VkResult result = vkBeginCommandBuffer', '        VkResult result = vkBeginCommandBuffer'),
  ('\t\tassert(result == VK_SUCCESS);','\t\tassert(result == VK_SUCCESS);\n        gb_probe::gpu_begin();'),
  ('                technique_it->bind();','                gb_probe::scope probe("ss:"+technique_it->get_name());\n                gb_probe::mark("ss:"+technique_it->get_name());\n                technique_it->bind();'),
  ('                technique_it->unbind();','                technique_it->unbind();\n                gb_probe::mark("end");'),
  ('            m_main_render_technique->bind();','            gb_probe::scope probe("output");\n            gb_probe::mark("output");\n            m_main_render_technique->bind();'),
  ('            m_main_render_technique->unbind();','            m_main_render_technique->unbind();\n            gb_probe::mark("end");'),
  ('\t\tVkResult result = vkEndCommandBuffer','        gb_probe::mark("frame_end");\n\t\tVkResult result = vkEndCommandBuffer'),
  ('            m_graphics_context->draw();','            gb_probe::scope probe("submit_present");\n            m_graphics_context->draw();'),
  ('        render_statistics::end_frame();','        render_statistics::end_frame();\n        gb_probe::frame_end();')],
 'ces_render_system.cpp': [
  ('            technique->bind();','            gb_probe::scope probe("ws:"+technique_name);\n            gb_probe::mark("ws:"+technique_name);\n            technique->bind();'),
  ('            technique->unbind();\n            render_statistics::end_pass();','            technique->unbind();\n            gb_probe::mark("end");\n            render_statistics::end_pass();'),
  ('                technique->unbind();\n                render_statistics::end_pass();','                technique->unbind();\n                gb_probe::mark("end");\n                render_statistics::end_pass();'),
  ('                                draw_3d_entity_func(technique_name,','                                const std::string probe_tag=entity->tag;\n                                const bool probe_terrain=gb_probe::enabled() && technique_name=="ws.base" && (probe_tag.find("island.terrain.lod.main.")==0 || probe_tag.find("0_")==0);\n                                if(probe_terrain)gb_probe::mark("terrain");\n                                draw_3d_entity_func(technique_name,'),
  ('                                                    entity->get_component_raw<ces_shader_uniforms_component>());','                                                    entity->get_component_raw<ces_shader_uniforms_component>());\n                                if(probe_terrain)gb_probe::mark("end");')],
 'shader.cpp': [ ('VkDescriptorSet shader::construct_descriptor_set()\n\t{','VkDescriptorSet shader::construct_descriptor_set()\n\t{\n        gb_probe::scope probe("descriptors");')],
 'vk_utils.cpp': [('void vk_utils::flush_command_buffer(VkCommandBuffer command_buffer, VkQueue queue, bool free)\n\t{','void vk_utils::flush_command_buffer(VkCommandBuffer command_buffer, VkQueue queue, bool free)\n\t{\n        gb_probe::scope probe("upload_flush");')],
 'vk_device.cpp': [('\t\tm_graphics_pipelines.emplace(key, pipeline);','\t\tm_graphics_pipelines.emplace(key, pipeline);\n        if(gb_probe::enabled())std::cout<<"PROBE_PIPELINE created="<<m_graphics_pipelines.size()<<"\\n";')],
}
# Native stress is opt-in and confined to temporary instrumentation. It uses the
# actual engine allocator and draw path, not a reimplementation of those classes.
files['shader.cpp'][0] = (files['shader.cpp'][0][0], files['shader.cpp'][0][1] + '''
        static bool stress_active=false; static unsigned stress_frame=~0u;
        if(gb_probe::enabled() && gb_probe::native_stress() && !stress_active &&
           gb_probe::state().frame>=60 && gb_probe::state().frame<66 && stress_frame!=gb_probe::state().frame) {
            stress_frame=gb_probe::state().frame; stress_active=true;
            for(unsigned i=0;i<1100;++i)construct_descriptor_set();
            stress_active=false;
            std::cout<<"PROBE_NATIVE descriptors=1100 frame="<<stress_frame<<"\\n";
        }''')
for name in ('vbo','ibo'):
    bind = '\t\t\tvkCmdBindVertexBuffers(draw_cmd_buffer, 0, 1, &buffer, offsets);' if name=='vbo' else '\t\t\tvkCmdBindIndexBuffer(draw_cmd_buffer, buffer, 0, VK_INDEX_TYPE_UINT16);'
    rebind='self.bind(attributes);' if name=='vbo' else 'self.bind();'
    stress='''
            static bool native_stress_active=false;
            if(gb_probe::enabled() && gb_probe::native_stress() && !native_stress_active &&
               gb_probe::state().frame>=60 && gb_probe::state().frame<66 && m_mode==gl::constant::dynamic_draw) {
                auto& self=const_cast<CLASS&>(*this); auto original=self.m_dynamic_data;
                auto& frame=self.m_dynamic_frames[current_image_index];
                const unsigned original_slot=frame.used_buffers-1;
                native_stress_active=true;
                DRAW_ORIGINAL
                std::fill(self.m_dynamic_data.begin(),self.m_dynamic_data.end(),0);
                ++self.m_dynamic_version; REBIND
                if(frame.buffers[original_slot]->get_handler()==frame.buffers[frame.used_buffers-1]->get_handler() ||
                   memcmp(frame.buffers[original_slot]->get_mapped_data(),original.data(),original.size())!=0)std::abort();
                self.m_dynamic_data=original; ++self.m_dynamic_version; REBIND
                native_stress_active=false;
                std::cout<<"PROBE_NATIVE CLASS snapshot_slots="<<frame.used_buffers<<" frame="<<gb_probe::state().frame<<"\\n";
            }
'''.replace('CLASS',name).replace('REBIND',rebind)
    draw = 'gb::shader::bind_vulkan_descriptor_set(); vkCmdDrawIndexed(draw_cmd_buffer,m_used_size,1,0,0,0);' if name=='ibo' else ''
    stress=stress.replace('DRAW_ORIGINAL',draw)
    files[name+'.cpp']=[(bind,bind+stress)]
if sys.argv[1]=='install':
    if os.path.exists(manifest): raise RuntimeError('Probe already installed')
    if not os.path.isdir(backup): os.makedirs(backup)
    plans=[]
    for name, substitutions in files.items():
        module='gbDemo' if name in ('game_conroller_win32.cpp','island_demo_scene.cpp') else 'gbCore'
        path=os.path.join(root,module,'sources',name)
        original=open(path,'rb').read(); data=original.replace(b'\r\n',b'\n')
        # Two identical result assertions: target begin block with context instead.
        for old,new in substitutions:
            if old==new: continue
            if name=='graphics_context_win32.cpp' and old=='if (strcmp(layer.layerName, "VK_LAYER_KHRONOS_validation") == 0)' and old.encode('utf8') not in data:
                old='if (enable_validation && strcmp(layer.layerName, "VK_LAYER_KHRONOS_validation") == 0)'
                new='if (enable_validation && strcmp(layer.layerName, "VK_LAYER_KHRONOS_validation") == 0 && std::getenv("GB_PROBE_VALIDATION_OFF")==nullptr)'
            if name=='render_pipeline.cpp' and old=='\t\tassert(result == VK_SUCCESS);':
                old='\t\tassert(result == VK_SUCCESS);\n\n#endif\n\n    }'
                new='\t\tassert(result == VK_SUCCESS);\n        gb_probe::gpu_begin();\n\n#endif\n\n    }'
            data=replace(data,old,new)
        include_name='graphics_context.h' if name=='graphics_context_win32.cpp' else name.replace('.cpp','.h')
        data=replace(data,'#include "'+include_name+'"',
                     '#include "'+include_name+'"\n#include "../../gbScripts/performance/core-probe.h"')
        if name=='ibo.cpp': data=replace(data,'#include "ibo.h"','#include "ibo.h"\n#include "shader.h"')
        plans.append((name,module,path,original,data))
    records=[]
    for name,module,path,original,data in plans:
        open(os.path.join(backup,name),'wb').write(original)
        open(path,'wb').write(data)
        records.append({'name':name,'module':module,'original':digest(original),'instrumented':digest(data)})
    open(manifest,'w').write(json.dumps(records,indent=2))
    print('Installed probe, snapshots:',backup)
elif sys.argv[1]=='remove':
    records=json.load(open(manifest))
    for record in records:
        path=os.path.join(root,record['module'],'sources',record['name'])
        if digest(open(path,'rb').read())!=record['instrumented']: raise RuntimeError('Source changed; refusing overwrite: '+path)
        if digest(open(os.path.join(backup,record['name']),'rb').read())!=record['original']: raise RuntimeError('Snapshot corrupted')
    for record in records: shutil.copyfile(os.path.join(backup,record['name']),os.path.join(root,record['module'],'sources',record['name']))
    if os.path.exists(manifest+'.restored'): os.remove(manifest+'.restored')
    os.rename(manifest,manifest+'.restored')
    print('Restored exact original bytes for all probe targets')
else: raise RuntimeError('Expected install or remove')
