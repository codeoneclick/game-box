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
ces_island_resident_system::ces_island_resident_system(resident_navigation nav):m_navigation(std::move(nav)){
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
        if(m_fixed_pose.empty())step_resident(m_navigation,dt,state);
        else state.walking=m_fixed_pose=="walk";
        if(!state.available)return;
        auto transform=entity->get_component<gb::ces_transformation_3d_component>();
        transform->set_position(glm::vec3(state.position.x,m_navigation.shore_y,state.position.z));
        transform->set_rotation(glm::vec3(0,state.yaw,0));
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
    enumerate_entities_with_components(m_mask,[&](const gb::ces_entity_shared_ptr& entity){
        auto resident=entity->get_component<ces_island_resident_component>();
        auto mixer=entity->get_component<gb::ces_animation_3d_mixer_component>();
        auto uniforms=entity->get_component<gb::ces_shader_uniforms_component>();
        auto bones=std::static_pointer_cast<resident_bone_uniforms>(uniforms->get_uniforms(gb::ces_shader_uniforms_component::e_shader_uniform_mode::e_vertex,"resident.bones",2));
        bones->update(mixer->get_transformations(),mixer->get_transformation_size());
        // Both legacy bulk setters skip arrays; the individual material setter
        // supports them. Keep this adapter local rather than changing bulk semantics.
        entity->get_component<gb::ces_material_component>()->get_material("ws.base")->set_custom_shader_uniform(static_cast<glm::mat4*>(bones->get_values()),64,"u_bones");
        std::static_pointer_cast<resident_color_uniforms>(uniforms->get_uniforms(gb::ces_shader_uniforms_component::e_shader_uniform_mode::e_fragment,"resident.color",0))->update(view,resident->clothing_color);
        if(m_debug&&m_elapsed>=m_next_report){
            const auto& s=resident->state;std::cout<<"Resident position="<<s.position.x<<","<<m_navigation.shore_y<<","<<s.position.z<<" safe="<<m_navigation.is_safe(s.position)<<" bones="<<mixer->get_transformation_size()<<std::endl;
            m_next_report=m_elapsed+1;
            auto geometry=entity->get_component<gb::ces_geometry_3d_component>();
            auto mesh=std::static_pointer_cast<gb::mesh_3d>(geometry->get_mesh());
            auto gpu=static_cast<glm::mat4*>(bones->get_values());
            for(int b=0;b<mixer->get_transformation_size();++b){bool finite=true;for(int a=0;a<4;++a)for(int c=0;c<4;++c)finite=finite&&std::isfinite(gpu[b][a][c]);if(!finite)std::cout<<"NONFINITE BONE "<<b<<std::endl;}
            glm::vec3 low(1e9f),high(-1e9f);float minw=1e9f,maxw=-1e9f;
            const auto vertices=mesh->get_vbo()->lock<gb::vbo::vertex_attribute_PT4B>();
            for(ui32 i=0;i<mesh->get_vbo()->get_used_size();++i){const auto& v=vertices[i];glm::vec4 p(0);
                float total=0;for(int j=0;j<4;++j)total+=v.m_bone_weights[j];
                for(int j=0;j<4;++j)if(v.m_bone_weights[j])p+=mixer->get_transformations()[v.m_bone_ids[j]]*glm::vec4(v.m_position,1)*(v.m_bone_weights[j]/total);
                low=glm::min(low,glm::vec3(p));high=glm::max(high,glm::vec3(p));
                minw=std::min(minw,p.w);maxw=std::max(maxw,p.w);
            }
            const auto transform=entity->get_component<gb::ces_transformation_3d_component>();
            auto clip=camera->get_mat_p()*view*transform->get_absolute_transformation()*glm::vec4((low+high)*.5f,1);
            std::cout<<"Resident skin bounds="<<low.x<<","<<low.y<<","<<low.z<<" to "<<high.x<<","<<high.y<<","<<high.z
                <<" w="<<minw<<","<<maxw<<" projected="<<clip.x/clip.w<<","<<clip.y/clip.w<<" sphere="<<camera->get_frustum()->is_sphere_in_frustum(transform->get_absolute_position(),2.2f)<<std::endl;
        }
    });
}
}
