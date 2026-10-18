#include "ces_ocean_system.h"
#include "ocean_shader_uniforms.h"
#include "camera_3d.h"
#include "ces_geometry_3d_component.h"
#include "mesh_3d.h"
#include "ibo.h"
#include "ces_buoyancy_component.h"
#include "ces_transformation_3d_component.h"
#include <cstdlib>
#include <algorithm>
#include <chrono>
namespace game {
    ces_ocean_system::ces_ocean_system(const ocean_settings& settings):m_settings(settings) {
        m_settings.validate();set_order(6);
        add_required_component_guid(m_buoyancy_mask,ces_buoyancy_component::class_guid());
        add_required_components_mask(m_buoyancy_mask);
        if(const char* value=std::getenv("GB_OCEAN_FIXED_TIME")) m_fixed_time=std::strtof(value,nullptr);
        if(const char* value=std::getenv("GB_OCEAN_DEBUG")) m_debug_mode=std::atoi(value);
    }
    void ces_ocean_system::add_surface(const gb::ces_entity_shared_ptr& water,const gb::ces_entity_shared_ptr& foam,
                                       const terrain_mesh_data& source,bool horizon) {
        surface entry;entry.water=water;entry.foam=foam;entry.horizon=horizon;
        rebuild_surface(entry,source);m_surfaces.push_back(std::move(entry));
        add_receiver(water);if(foam)add_receiver(foam);
    }
    void ces_ocean_system::rebuild_surface(surface& entry,const terrain_mesh_data& source) {
        auto water=entry.water.lock();if(!water)return;
        entry.source=source;entry.tree=std::make_shared<ocean_mesh_quadtree>(source);entry.selected={};
        entry.original=water->get_component<gb::ces_geometry_3d_component>()->get_mesh()->as_3d();
        // Keep original vertices and triangle order. Only the submitted index lists change.
        // Dynamic IBOs use the engine's existing per-frame Vulkan buffer snapshots.
        const auto install=[&](const gb::ces_entity_shared_ptr& entity,std::shared_ptr<gb::ibo>& indices,std::shared_ptr<gb::mesh_3d>& mesh){
            if(!entity)return;
            indices=std::make_shared<gb::ibo>(static_cast<ui32>(source.indices.size()),gb::gl::constant::dynamic_draw);
            std::copy(source.indices.begin(),source.indices.end(),indices->lock());indices->unlock();
            auto geometry=entity->get_component<gb::ces_geometry_3d_component>();
            mesh=gb::mesh_3d::construct(std::string(entity->tag)+".culled",entry.original->get_vbo(),indices,
                entry.original->get_max_bound(),entry.original->get_min_bound());geometry->set_mesh(mesh);
            // The quadtree tests the actual shader-displaced envelope and projection.
            geometry->bounding_mode=gb::ces_geometry_component::e_none;
        };
        install(water,entry.water_indices,entry.water_mesh);install(entry.foam.lock(),entry.foam_indices,entry.foam_mesh);
        entry.last_water=source.indices;entry.last_foam=source.indices;m_cull_valid=false;
    }
    void ces_ocean_system::reset_surface(const gb::ces_entity_shared_ptr& water,const terrain_mesh_data& source) {
        for(auto& entry:m_surfaces)if(entry.water.lock()==water){
            // Load/rebuild follows the same GPU retirement grace as interaction meshes.
            for(const auto& mesh:{entry.original,entry.water_mesh,entry.foam_mesh})
                if(mesh)m_retired_interaction_meshes.push_back({12,mesh});
            if(auto foam=entry.foam.lock())if(auto mesh=foam->get_component<gb::ces_geometry_3d_component>()->get_mesh())
                m_retired_interaction_meshes.push_back({12,mesh->as_3d()});
            rebuild_surface(entry,source);return;
        }
    }
    void ces_ocean_system::update_surface_visibility(const glm::mat4& matrix) {
        const bool enabled=!std::getenv("GB_OCEAN_CULLING")||std::string(std::getenv("GB_OCEAN_CULLING"))!="0";
        std::array<float,16> values;std::copy(&matrix[0][0],&matrix[0][0]+16,values.begin());
        if(m_cull_valid&&(!enabled||values==m_cull_matrix))return;
        m_cull_matrix=values;m_cull_valid=true;
        const auto started=std::chrono::steady_clock::now();
        float amplitude=0;for(const auto& wave:m_settings.waves)amplitude+=wave.amplitude;
        const float envelope=std::max(.001f,1.09f*amplitude)+.001f;
        const float minimum=m_settings.mean_level-envelope+std::min(0.f,m_settings.foam_height_offset);
        const float maximum=m_settings.mean_level+envelope+std::max(0.f,m_settings.foam_height_offset);
        // Outside this support, both shader distance samples return sentinel 320.
        // Unusually broad future settings fall back to frustum-only foam pruning.
        const float width=m_settings.foam_mid_max_width*1.32f*1.3f,contact=m_settings.foam_inner_distance*2.f;
        const float support=(width+contact+4.2f<319.f&&contact+.25f<319.f)?m_settings.interaction_size*.5f+m_settings.foam_warp_strength*.5f+.01f:0.f;
        size_t water_triangles=0,foam_triangles=0,tested=0;
        for(auto& entry:m_surfaces){
            auto& selected=entry.selected;
            const bool changed=enabled?entry.tree->select(values,minimum,maximum,entry.horizon,support,selected):true;
            if(!enabled){selected.water=entry.source.indices;if(!entry.horizon)selected.foam=entry.source.indices;}
            const auto submit=[enabled,&entry](const gb::ces_entity_shared_ptr& entity,const std::shared_ptr<gb::ibo>& buffer,
                                 const std::shared_ptr<gb::mesh_3d>& filtered,
                                 const std::vector<uint16_t>& next,std::vector<uint16_t>& previous){
                if(!entity)return;
                entity->get_component<gb::ces_geometry_3d_component>()->bounding_mode=enabled?
                    gb::ces_geometry_component::e_none:gb::ces_geometry_component::e_box;
                const bool visible=!next.empty();entity->visible=visible;entity->visible_in_next_frame=visible;
                const bool full=next.size()==entry.source.indices.size();
                // Fully selected nodes keep the original device-local index buffer.
                entity->get_component<gb::ces_geometry_3d_component>()->set_mesh(full?entry.original:filtered);
                if(next!=previous){if(visible&&!full){std::copy(next.begin(),next.end(),buffer->lock());buffer->unlock(static_cast<ui32>(next.size()));}previous=next;}
            };
            if(changed){
                submit(entry.water.lock(),entry.water_indices,entry.water_mesh,selected.water,entry.last_water);
                submit(entry.foam.lock(),entry.foam_indices,entry.foam_mesh,selected.foam,entry.last_foam);
            }
            water_triangles+=selected.water.size()/3;foam_triangles+=selected.foam.size()/3;tested+=selected.tested_nodes;
        }
        if(std::getenv("GB_OCEAN_CULL_LOG"))std::cout<<"Ocean culling: water="<<water_triangles<<" foam="<<foam_triangles<<" nodes="<<tested
            <<" cpu_ms="<<std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-started).count()<<std::endl;
    }
    void ces_ocean_system::add_interaction(const gb::ces_entity_shared_ptr& source,const gb::ces_entity_shared_ptr& mask) {
        auto mesh=source->get_component<gb::ces_geometry_3d_component>()->get_mesh();
        m_interactions.push_back({source,mask,mesh?mesh->as_3d():nullptr});add_receiver(mask);
    }
    void ces_ocean_system::set_mean_level(float level){auto settings=m_settings;settings.mean_level=level;settings.validate();m_settings=settings;m_cull_valid=false;}
    void ces_ocean_system::set_world_dimensions(float world_size,float water_size) {
        auto settings=m_settings;settings.interaction_size=world_size;
        settings.water_detail_radius=water_size*.5f;settings.validate();m_settings=settings;m_cull_valid=false;
    }
    void ces_ocean_system::add_receiver(const gb::ces_entity_shared_ptr& entity) {
        auto uniforms=entity->add_component<gb::ces_shader_uniforms_component>();
        uniforms->construct_uniforms_buffer<ocean_shader_uniforms>(gb::ces_shader_uniforms_component::e_shader_uniform_mode::e_vertex,"ocean",2);
        uniforms->construct_uniforms_buffer<ocean_shader_uniforms>(gb::ces_shader_uniforms_component::e_shader_uniform_mode::e_fragment,"ocean",0);
        m_receivers.push_back(entity);
    }
    void ces_ocean_system::on_feed(const gb::ces_entity_shared_ptr& root,f32 dt) {
        for(auto& retired:m_retired_interaction_meshes)--retired.first;
        while(!m_retired_interaction_meshes.empty()&&m_retired_interaction_meshes.front().first<=0)m_retired_interaction_meshes.pop_front();
        const float accepted_dt=std::isfinite(dt)?glm::clamp(dt,0.f,.25f):0.f;
        const double previous_time=m_time;
        m_time+=accepted_dt;
        const float time=m_fixed_time>=0 && std::isfinite(m_fixed_time)?m_fixed_time:static_cast<float>(m_time);
        enumerate_entities_with_components(m_buoyancy_mask,[&](const gb::ces_entity_shared_ptr& entity){
            auto buoyancy=entity->get_component<ces_buoyancy_component>();
            if(m_fixed_time<0 || !std::isfinite(m_fixed_time))step_buoyancy(m_settings,buoyancy->settings,buoyancy->probes,previous_time,accepted_dt,buoyancy->state);
            else buoyancy->state.position.y=sample_ocean(m_settings,buoyancy->state.position.x,buoyancy->state.position.z,time).position.y;
            auto transform=entity->get_component<gb::ces_transformation_3d_component>();
            const auto& s=buoyancy->state;
            transform->set_position(glm::vec3(s.position.x,s.position.y,s.position.z));
            transform->set_rotation(glm::vec3(s.pitch,s.yaw,s.roll));
        });
        for(auto it=m_interactions.begin();it!=m_interactions.end();) {
            auto source=it->source.lock(),mask=it->mask.lock();
            if(!source||!mask){
                if(it->mesh)m_retired_interaction_meshes.push_back({12,it->mesh});
                if(mask){mask->visible=false;mask->visible_in_next_frame=false;auto geometry=mask->get_component<gb::ces_geometry_3d_component>();if(auto old=geometry->get_mesh())if(old->as_3d()!=it->mesh)m_retired_interaction_meshes.push_back({12,old->as_3d()});geometry->set_mesh(nullptr);}
                it=m_interactions.erase(it);continue;
            }
            mask->visible=source->get_is_visible();mask->visible_in_next_frame=source->get_is_visible_in_next_frame();
            mask->get_component<gb::ces_transformation_3d_component>()->update_absolute_transformation(source->get_component<gb::ces_transformation_3d_component>()->get_absolute_transformation());
            auto source_mesh=source->get_component<gb::ces_geometry_3d_component>()->get_mesh();
            auto mask_geometry=mask->get_component<gb::ces_geometry_3d_component>();
            if(mask_geometry->get_mesh()!=source_mesh) {
                if(auto old=mask_geometry->get_mesh())m_retired_interaction_meshes.push_back({12,old->as_3d()});
                mask_geometry->set_mesh(source_mesh?source_mesh->as_3d():nullptr);
            }
            it->mesh=source_mesh?source_mesh->as_3d():nullptr;
            ++it;
        }
        auto camera=get_current_camera_3d();if(!camera)return;
        const auto view=camera->get_mat_v(),projection=camera->get_mat_p();
        update_surface_visibility(projection*view);
        if(auto techniques=root->get_component<gb::ces_render_technique_uniforms_component>()) {
            if(auto atmosphere=techniques->get_uniforms("compose")){
                atmosphere->set(view,"view");atmosphere->set(projection,"projection");
                atmosphere->set(glm::vec4(camera->get_position(),1),"camera");
                const char* enabled=std::getenv("GB_ATMOSPHERE");
                atmosphere->set(glm::vec4(time,m_settings.mean_level,enabled&&std::string(enabled)=="0"?0.f:1.f,static_cast<float>(m_debug_mode)),"time_level");
                const char* edges=std::getenv("GB_EDGE_DETECT");
                const auto d=stylized_lighting_settings().sun_direction();
                atmosphere->set(glm::vec4(d[0],d[1],d[2],edges&&std::string(edges)=="0"?0.f:1.f),"sun");
            }
            if(auto ao=techniques->get_uniforms("ss.ao")) {
                ao->set(view,"mat_v");
                ao->set(projection,"mat_p");
            }
        }
        if(m_debug_mode==5&&!m_diagnostics_reported){
            for(auto rest:{glm::vec2(150,0),glm::vec2(-150,0),glm::vec2(0,180),glm::vec2(-160,80)}){
                const auto s=evaluate_ocean(m_settings,rest.x,rest.y,time);
                auto clip=projection*view*glm::vec4(s.position.x,s.position.y,s.position.z,1);
                std::cout<<"Ocean height probe uv="<<(.5f+.5f*clip.x/clip.w)<<","<<(.5f-.5f*clip.y/clip.w)<<" height="<<s.position.y<<std::endl;
            }
            m_diagnostics_reported=true;
        }
        // Presentation changes never mutate the wave/buoyancy model or meshes.
        auto render_settings=m_settings;
        render_settings.reflection_strength*=m_optics_intensity.x;
        render_settings.refraction_strength*=m_optics_intensity.y;
        render_settings.caustic_strength*=m_optics_intensity.z;
        for(auto it=m_receivers.begin();it!=m_receivers.end();) {
            if(auto entity=it->lock()) {
                if(!entity->get_is_visible()||!entity->get_is_visible_in_next_frame()){++it;continue;}
                auto uniforms=entity->get_component<gb::ces_shader_uniforms_component>();
                auto buoyancy=entity->get_component<ces_buoyancy_component>();
                for(auto mode:{gb::ces_shader_uniforms_component::e_shader_uniform_mode::e_vertex,gb::ces_shader_uniforms_component::e_shader_uniform_mode::e_fragment})
                    std::static_pointer_cast<ocean_shader_uniforms>(uniforms->get_uniforms(mode,"ocean",mode==gb::ces_shader_uniforms_component::e_shader_uniform_mode::e_vertex?2:0))->update(render_settings,time,camera->get_position(),view,projection,buoyancy?buoyancy->state.wetness:0,m_debug_mode);
                ++it;
            } else it=m_receivers.erase(it);
        }
    }
}


