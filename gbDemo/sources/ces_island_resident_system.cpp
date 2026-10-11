#include "ces_island_resident_system.h"
#include "ces_island_resident_component.h"
#include "ces_animation_3d_mixer_component.h"
#include "ces_transformation_3d_component.h"
#include "resident_shader_uniforms.h"
#include "shape_3d.h"
#include "camera_3d.h"
#include "ces_material_component.h"
#include "material.h"
#include "ces_geometry_3d_component.h"
#include "mesh_3d.h"
#include "frustum_3d.h"
#include "vbo.h"
#include <cstdlib>
namespace game {
ces_island_resident_system::ces_island_resident_system(std::vector<resident_navigation> nav):m_navigations(std::move(nav)){
    set_order(6);
    add_required_component_guid(m_mask,ces_island_resident_component::class_guid());
    add_required_component_guid(m_mask,gb::ces_animation_3d_mixer_component::class_guid());
    add_required_component_guid(m_mask,gb::ces_transformation_3d_component::class_guid());
    add_required_component_guid(m_mask,gb::ces_shader_uniforms_component::class_guid());add_required_components_mask(m_mask);
    if(const auto value=std::getenv("GB_RESIDENT_POSE")){if(std::string(value)=="idle"||std::string(value)=="walk")m_fixed_pose=value;}
    if(const auto value=std::getenv("GB_RESIDENT_TIME"))m_fixed_time=std::strtof(value,nullptr);
    m_debug=std::getenv("GB_RESIDENT_DEBUG")!=nullptr;
}
void ces_island_resident_system::on_feed_start(f32 dt){
    enumerate_entities_with_components(m_mask,[&](const gb::ces_entity_shared_ptr& entity){
        auto resident=entity->get_component<ces_island_resident_component>();auto& state=resident->state;
        if(resident->terrace_index>=m_navigations.size()){state.available=false;state.walking=false;std::cout<<"RESIDENT TERRACE INVALID"<<std::endl;return;}
        const auto& navigation=m_navigations[resident->terrace_index];
        const auto previous=state.position;
        if(m_fixed_pose.empty())step_resident(navigation,dt,state);
        else state.walking=m_fixed_pose=="walk";
        if(!state.available)return;
        auto transform=entity->get_component<gb::ces_transformation_3d_component>();
        const glm::vec3 position(state.position.x,navigation.shore_y,state.position.z);
        if (transform->get_position()!=position) transform->set_position(position);
        // Navigation uses radians; the engine's legacy GLM transform uses degrees.
        const glm::vec3 rotation(0,glm::degrees(state.yaw),0);
        if (transform->get_rotation()!=rotation) transform->set_rotation(rotation);
        if(m_debug) {
            const glm::vec2 movement(state.position.x-previous.x,state.position.z-previous.z);
            if(glm::length(movement)>1.e-6f) {
                const auto forward=transform->get_absolute_transformation()*glm::vec4(0,0,1,0);
                if(glm::dot(glm::normalize(movement),glm::normalize(glm::vec2(forward.x,forward.z)))<.999f)
                    std::cout<<"RESIDENT FACING MISMATCH"<<std::endl;
            }
        }
        const std::string clip=state.walking?"walk":"idle";
        if(resident->animation!=clip){std::static_pointer_cast<gb::shape_3d>(entity)->play_animation(clip,true);resident->animation=clip;
            if(m_debug)std::cout<<"Resident mode="<<clip<<std::endl;}
        if(m_fixed_time>=0&&std::isfinite(m_fixed_time)){
            auto mixer=entity->get_component<gb::ces_animation_3d_mixer_component>();mixer->reset_animation_time(0);
            mixer->update_animation_time(0,std::max(0.f,m_fixed_time-dt));
        }
    });
}
void ces_island_resident_system::on_feed(const gb::ces_entity_shared_ptr&,f32 dt){
    m_elapsed+=std::isfinite(dt)?std::max(0.f,std::min(dt,.1f)):0;
    auto camera=get_current_camera_3d();if(!camera)return;const auto view=camera->get_mat_v();
    const bool report=m_debug&&m_elapsed>=m_next_report;
    std::vector<size_t> counts(report?m_navigations.size():0,0);size_t unsafe=0,walkers=0;
    enumerate_entities_with_components(m_mask,[&](const gb::ces_entity_shared_ptr& entity){
        auto resident=entity->get_component<ces_island_resident_component>();
        if(resident->terrace_index>=m_navigations.size())return;
        const auto& navigation=m_navigations[resident->terrace_index];
        if(report){++counts[resident->terrace_index];walkers+=resident->state.walking?1:0;
            auto transform=entity->get_component<gb::ces_transformation_3d_component>();
            if(!resident->state.available||!navigation.is_safe(resident->state.position)||std::abs(transform->get_position().y-navigation.shore_y)>.001f)++unsafe;
        }
        auto mixer=entity->get_component<gb::ces_animation_3d_mixer_component>();
        auto uniforms=entity->get_component<gb::ces_shader_uniforms_component>();
        auto bones=std::static_pointer_cast<resident_bone_uniforms>(uniforms->get_uniforms(gb::ces_shader_uniforms_component::e_shader_uniform_mode::e_vertex,"resident.bones",2));
        // Ground the final interpolated/blended pose. Shared contact points
        // avoid scanning the full mesh for each member of the crowd.
        float ground_height=0.f;
        if(resident->foot_contacts&&!resident->foot_contacts->empty()) {
            ground_height=std::numeric_limits<float>::max();
            for(const auto& contact:*resident->foot_contacts)
                ground_height=std::min(ground_height,(mixer->get_transformations()[contact.first]*glm::vec4(contact.second,1)).y);
        }
        bones->update(mixer->get_transformations(),mixer->get_transformation_size(),ground_height);
        // Both legacy bulk setters skip arrays; the individual material setter
        // supports them. Keep this adapter local rather than changing bulk semantics.
        entity->get_component<gb::ces_material_component>()->get_material("ws.base")->set_custom_shader_uniform(static_cast<glm::mat4*>(bones->get_values()),64,"u_bones");
        std::static_pointer_cast<resident_color_uniforms>(uniforms->get_uniforms(gb::ces_shader_uniforms_component::e_shader_uniform_mode::e_fragment,"resident.color",0))->update(view,resident->clothing_color);
        if(m_debug&&m_elapsed>=m_next_report){
            const auto& s=resident->state;std::cout<<"Resident position="<<s.position.x<<","<<navigation.shore_y<<","<<s.position.z<<" safe="<<navigation.is_safe(s.position)<<" bones="<<mixer->get_transformation_size()<<std::endl;
            m_next_report=m_elapsed+1;
            auto geometry=entity->get_component<gb::ces_geometry_3d_component>();
            auto mesh=std::static_pointer_cast<gb::mesh_3d>(geometry->get_mesh());
            auto gpu=static_cast<glm::mat4*>(bones->get_values());
            for(int b=0;b<mixer->get_transformation_size();++b){bool finite=true;for(int a=0;a<4;++a)for(int c=0;c<4;++c)finite=finite&&std::isfinite(gpu[b][a][c]);if(!finite)std::cout<<"NONFINITE BONE "<<b<<std::endl;}
            glm::vec3 low(1e9f),high(-1e9f);float minw=1e9f,maxw=-1e9f;
            const auto vertices=mesh->get_vbo()->lock<gb::vbo::vertex_attribute_PT4B>();
            for(ui32 i=0;i<mesh->get_vbo()->get_used_size();++i){const auto& v=vertices[i];glm::vec4 p(0);
                float total=0;for(int j=0;j<4;++j)total+=v.m_bone_weights[j];
                for(int j=0;j<4;++j)if(v.m_bone_weights[j])p+=gpu[v.m_bone_ids[j]]*glm::vec4(v.m_position,1)*(v.m_bone_weights[j]/total);
                low=glm::min(low,glm::vec3(p));high=glm::max(high,glm::vec3(p));
                minw=std::min(minw,p.w);maxw=std::max(maxw,p.w);
            }
            const auto transform=entity->get_component<gb::ces_transformation_3d_component>();
            auto clip=camera->get_mat_p()*view*transform->get_absolute_transformation()*glm::vec4((low+high)*.5f,1);
            std::cout<<"Resident skin bounds="<<low.x<<","<<low.y<<","<<low.z<<" to "<<high.x<<","<<high.y<<","<<high.z
                <<" w="<<minw<<","<<maxw<<" projected="<<clip.x/clip.w<<","<<clip.y/clip.w<<" sphere="<<camera->get_frustum()->is_sphere_in_frustum(transform->get_absolute_position(),2.2f)<<std::endl;
        }
    });
    if(report){
        std::cout<<"Resident crowd report: walkers="<<walkers<<" unsafe="<<unsafe<<" counts=";
        for(auto count:counts)std::cout<<count<<",";
        std::cout<<std::endl;
    }
}
}
