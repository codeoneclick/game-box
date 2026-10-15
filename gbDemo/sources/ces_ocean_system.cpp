#include "ces_ocean_system.h"
#include "ocean_shader_uniforms.h"
#include "camera_3d.h"
#include "ces_geometry_3d_component.h"
#include "mesh_3d.h"
#include "ces_buoyancy_component.h"
#include "ces_transformation_3d_component.h"
#include <cstdlib>
namespace game {
    ces_ocean_system::ces_ocean_system(const ocean_settings& settings):m_settings(settings) {
        m_settings.validate();set_order(6);
        add_required_component_guid(m_buoyancy_mask,ces_buoyancy_component::class_guid());
        add_required_components_mask(m_buoyancy_mask);
        if(const char* value=std::getenv("GB_OCEAN_FIXED_TIME")) m_fixed_time=std::strtof(value,nullptr);
        if(const char* value=std::getenv("GB_OCEAN_DEBUG")) m_debug_mode=std::atoi(value);
    }
    void ces_ocean_system::add_surface(const gb::ces_entity_shared_ptr& entity) {add_receiver(entity);}
    void ces_ocean_system::add_interaction(const gb::ces_entity_shared_ptr& source,const gb::ces_entity_shared_ptr& mask) {
        auto mesh=source->get_component<gb::ces_geometry_3d_component>()->get_mesh();
        m_interactions.push_back({source,mask,mesh?mesh->as_3d():nullptr});add_receiver(mask);
    }
    void ces_ocean_system::set_mean_level(float level){auto settings=m_settings;settings.mean_level=level;settings.validate();m_settings=settings;}
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
        for(auto it=m_receivers.begin();it!=m_receivers.end();) {
            if(auto entity=it->lock()) {
                auto uniforms=entity->get_component<gb::ces_shader_uniforms_component>();
                auto buoyancy=entity->get_component<ces_buoyancy_component>();
                for(auto mode:{gb::ces_shader_uniforms_component::e_shader_uniform_mode::e_vertex,gb::ces_shader_uniforms_component::e_shader_uniform_mode::e_fragment})
                    std::static_pointer_cast<ocean_shader_uniforms>(uniforms->get_uniforms(mode,"ocean",mode==gb::ces_shader_uniforms_component::e_shader_uniform_mode::e_vertex?2:0))->update(m_settings,time,camera->get_position(),view,projection,buoyancy?buoyancy->state.wetness:0,m_debug_mode);
                ++it;
            } else it=m_receivers.erase(it);
        }
    }
}


