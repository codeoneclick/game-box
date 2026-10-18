#include "render_debug_controller.h"
#include "render_statistics.h"
#include "stylized_post_process.h"
#include "render_pipeline.h"
#include "render_technique_ws.h"
#include "render_technique_ss.h"
#include "render_technique_main.h"
#include "scene_fabricator.h"
#include "configuration_accessor.h"
#include "material_configuration.h"
#include "material.h"
#include "texture.h"
#include <algorithm>
#include <map>
#include <stdexcept>
#include <cstdlib>
#include <iostream>
namespace game { namespace {
std::string ws_name(const std::string& name){
    const auto separator=name.find('_');
    return separator==std::string::npos?name:name.substr(separator+1);
}
int preview_mode(const std::string& id,size_t attachment){
    if(id=="ws.base")return attachment==1?1:attachment==2?2:0;
    if(id.find("ws.sun.")==0)return 4;
    if(id.find("ss.ao")==0)return 4;
    if(id.find("ws.water.")==0||id.find("grass.interaction")!=std::string::npos)return 3;
    if(id=="ws.ui")return 1;
    return 0;
}
}
struct RenderDebugController::implementation {
    std::shared_ptr<RenderDebugState> model=std::make_shared<RenderDebugState>();
    std::shared_ptr<StylizedPostProcessSettings> settings;
    std::shared_ptr<gb::render_pipeline> pipeline;
    std::vector<std::pair<std::string,std::shared_ptr<gb::render_technique_base>>> techniques;
    gb::texture_shared_ptr black,output_texture;
    gb::material_shared_ptr copy_material(const gb::scene_fabricator_shared_ptr& fab,const gb::texture_shared_ptr& source){
        const auto config=std::static_pointer_cast<gb::material_configuration>(fab->get_configuration_accessor()->get_material_configuration("material.render_debug_blit.xml"));
        auto material=gb::material::construct(config);
        gb::material::set_shader(material,config,fab->get_resource_accessor(),true);
        material->set_texture(source,gb::e_shader_sampler_01);
        return material;
    }
    void add(const std::string& id,const std::shared_ptr<gb::render_technique_base>& technique){
        RenderDebugPass pass;pass.id=id;pass.label=id;
        auto attachments=technique->get_color_attachments_texture();
        for(size_t i=0;i<attachments.size();++i){
            if(!attachments[i])continue;
            RenderDebugAttachment attachment;attachment.name="attachment_0"+std::to_string(i+1);
            attachment.texture=attachments[i];attachment.width=attachments[i]->get_width();attachment.height=attachments[i]->get_height();attachment.mode=preview_mode(id,i);
            pass.attachments.push_back(std::move(attachment));
        }
        model->passes.push_back(std::move(pass));techniques.push_back({id,technique});
    }
    implementation(const gb::scene_fabricator_shared_ptr& fab,const std::shared_ptr<gb::render_pipeline>& render,
                   const std::shared_ptr<StylizedPostProcessSettings>& prefs):settings(prefs),pipeline(render){
        if(!fab||!pipeline||!settings)throw std::invalid_argument("Render inspector requires renderer and preferences");
        std::map<std::string,gb::texture_shared_ptr> outputs;
        for(const auto& technique:pipeline->get_ws_techniques()){
            const auto id=ws_name(technique->get_name());outputs[id]=technique->get_color_attachment_texture();
        }
        // Start with the scene G-buffer; auxiliary passes retain actual execution order.
        for(const auto& technique:pipeline->get_ws_techniques())if(ws_name(technique->get_name())=="ws.base")add("ws.base",technique);
        for(const auto& technique:pipeline->get_ws_techniques()){
            const auto id=ws_name(technique->get_name());if(id!="ws.base"&&id!="ws.debug")add(id,technique);
        }
        for(const auto& technique:pipeline->get_ss_techniques()){
            const auto id=technique->get_name();outputs[id]=technique->get_color_attachment_texture();
            add(id,technique);
            const std::map<std::string,std::string> bypass={{"ss.ocean_compose","ws.base"},{"ss.sunshadow","ss.ocean_compose"},{"ss.stylized_final","ss.sunshadow"},{"ss.island_ui","ss.stylized_final"}};
            const auto source=bypass.find(id);
            if(source!=bypass.end()){
                const auto texture=outputs.find(source->second);
                if(texture==outputs.end())throw std::logic_error("Missing render pass bypass input: "+source->second);
                technique->set_bypass_material(copy_material(fab,texture->second));
            }
        }
        auto main=pipeline->get_main_technique();
        if(main){
            add("final",main);
            const auto source=outputs.find("ss.island_ui");
            if(source!=outputs.end())output_texture=source->second;
            // Presentation remains valid while final scene output is disabled.
            // The independent inspector is always composited by the output shader.
            uint8_t pixel[4]={0,0,0,255};black=gb::texture::construct("render.debug.black",1,1,gb::gl::constant::rgba_t,pixel);
            const auto config=std::static_pointer_cast<gb::material_configuration>(fab->get_configuration_accessor()->get_material_configuration("material.ss.output.island_demo.xml"));
            auto fallback=gb::material::construct(config);
            gb::material::set_shader(fallback,config,fab->get_resource_accessor(),true);
            gb::material::set_textures(fallback,config,fab->get_resource_accessor(),true);
            fallback->set_texture(black,gb::e_shader_sampler_01);main->set_bypass_material(fallback);
            RenderDebugAttachment attachment;attachment.name="Scene output (before inspector)";attachment.texture=output_texture;
            if(output_texture){attachment.width=output_texture->get_width();attachment.height=output_texture->get_height();}
            model->passes.back().attachments={attachment};
        }
        model->set_enabled=[prefs](const std::string& id,bool enabled){prefs->renderPasses[id]=enabled;};
        update();
        if(std::getenv("GB_RENDER_DEBUG_LOG"))for(size_t i=0;i<model->passes.size();++i)std::cout<<"Render pass["<<i<<"] "<<model->passes[i].id<<" enabled="<<model->passes[i].enabled<<" attachments="<<model->passes[i].attachments.size()<<std::endl;
        if(std::getenv("GB_RENDER_DEBUG_LOG"))for(const auto& pass:model->passes)for(const auto& attachment:pass.attachments)
            std::cout<<"Render attachment "<<pass.id<<" "<<attachment.name<<" size="<<attachment.width<<"x"<<attachment.height<<std::endl;
        // The inspector's own target is deliberately absent from its UI list,
        // but still participates in XML-to-GPU resolution diagnostics.
        if(std::getenv("GB_RENDER_DEBUG_LOG"))for(const auto& technique:pipeline->get_ws_techniques())if(ws_name(technique->get_name())=="ws.debug")
            for(const auto& texture:technique->get_color_attachments_texture())if(texture)
                std::cout<<"Render attachment ws.debug "<<texture->get_guid()<<" size="<<texture->get_width()<<"x"<<texture->get_height()<<std::endl;
    }
    void update(){
        for(size_t i=0;i<techniques.size();++i){
            const auto found=settings->renderPasses.find(techniques[i].first);
            const bool enabled=found==settings->renderPasses.end()||found->second;
            if(std::getenv("GB_RENDER_DEBUG_LOG")&&techniques[i].second->get_is_enabled()!=enabled)std::cout<<"Render pass toggle "<<techniques[i].first<<" enabled="<<enabled<<std::endl;
            techniques[i].second->set_enabled(enabled);model->passes[i].enabled=enabled;
            model->passes[i].triangles=gb::render_statistics::last_pass(techniques[i].second->get_name()).triangles;
            if(techniques[i].first=="final"&&!model->passes[i].attachments.empty())
                model->passes[i].attachments[0].texture=enabled?output_texture:black;
        }
    }
};
RenderDebugController::RenderDebugController(const gb::scene_fabricator_shared_ptr& fab,const std::shared_ptr<gb::render_pipeline>& pipeline,
                                           const std::shared_ptr<StylizedPostProcessSettings>& settings):impl_(std::make_shared<implementation>(fab,pipeline,settings)){}
std::shared_ptr<RenderDebugState> RenderDebugController::state()const{return impl_->model;}
void RenderDebugController::update(){impl_->update();}
}
