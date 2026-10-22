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
#include "terrain_editor_state.h"
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <stdexcept>
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
void ces_island_resident_system::refresh_navigation(const std::vector<terrain_mesh_data>& parts,const std::vector<float>& heights,const std::vector<float>& terrace_heights){
    if(!terrace_heights.empty()&&(terrace_heights.size()!=m_navigations.size()||
       std::any_of(terrace_heights.begin(),terrace_heights.end(),[](float y){return !std::isfinite(y);})))
        throw std::invalid_argument("Invalid resident terrace height replacement");
    std::vector<uint8_t> changed(m_navigations.size(),0);
    // Preserve terrace indices and each terrace's physical height. Parts carry
    // separate local index buffers and are never concatenated into 16-bit indices.
    for(size_t i=0;i<m_navigations.size();++i){
        const auto& old=m_navigations[i];
        const float target_y=terrace_heights.empty()?old.shore_y:terrace_heights[i];
        bool affected=heights.empty()||std::abs(target_y-old.shore_y)>=.001f||std::any_of(heights.begin(),heights.end(),[&](float y){return std::isfinite(y)&&std::abs(y-old.shore_y)<.001f;});
        if(!affected)continue;
        m_navigations[i]=build_resident_navigation(parts,target_y,old.cell_size);
        changed[i]=1;
    }
    refresh_residents(changed);
}
void ces_island_resident_system::apply_navigation(std::vector<resident_navigation> replacement){
    if(replacement.size()!=m_navigations.size())throw std::invalid_argument("Resident navigation replacement changes terrace assignments");
    for(const auto& nav:replacement){
        if(!std::isfinite(nav.shore_y)||!std::isfinite(nav.cell_size)||nav.cell_size<.25f||nav.cell_size>4.f||
           nav.width<0||nav.height<0||nav.walkable.size()!=static_cast<size_t>(nav.width)*nav.height||
           nav.cell_triangles.size()!=nav.walkable.size()||(!nav.solid_cells.empty()&&nav.solid_cells.size()!=nav.walkable.size()))
            throw std::invalid_argument("Invalid worker-built resident navigation");
    }
    m_navigations=std::move(replacement);
    refresh_residents(std::vector<uint8_t>(m_navigations.size(),1));
}
void ces_island_resident_system::refresh_residents(const std::vector<uint8_t>& changed){
    m_stair_links=build_resident_stair_links(m_stairs,m_stairs_config,m_navigations);
    using region_ptr=std::shared_ptr<const std::vector<resident_point>>;
    std::vector<std::vector<region_ptr>> regions(m_navigations.size());
    for(size_t i=0;i<changed.size();++i)if(changed[i])regions[i].resize(m_navigations[i].walkable.size());
    enumerate_entities_with_components(m_mask,[&](const gb::ces_entity_shared_ptr& entity){
        auto resident=entity->get_component<ces_island_resident_component>();
        size_t index=resident->terrace_index;
        // Navigation updates invalidate approaches. Active flights retain their
        // continuous trajectory and retreat to a surviving landing.
        rebind_resident_stairs(m_navigations,{},index,resident->state);
        if(resident->state.stair_transit)return;
        if(index>=changed.size()||!changed[index])return;
        auto& state=resident->state;const auto& nav=m_navigations[index];
        state.route.clear();state.route_index=0;state.reachable.clear();state.shared_reachable.reset();
        state.walking=false;state.idle_remaining=2.f;resident->woodcutting={};
        state.elevation=nav.shore_y;
        state.available=nav.is_safe(state.position);
        if(!state.available){
            float nearest=std::numeric_limits<float>::max();
            if(resident->axe){resident->axe->visible=false;resident->axe->visible_in_next_frame=false;}
            const auto previous=state.position;
            for(size_t cell=0;cell<nav.walkable.size();++cell){
                if(!nav.walkable[cell])continue;
                auto candidate=nav.center(static_cast<int>(cell));
                float dx=candidate.x-previous.x,dz=candidate.z-previous.z;
                float distance=dx*dx+dz*dz;
                if(distance>=nearest||!nav.is_safe(candidate))continue;
                nearest=distance;state.position=candidate;state.available=true;
            }
        }
        entity->visible=state.available;
        if(!state.available)return;
        int at=nav.cell_at(state.position);
        if(!regions[index][at]){
            auto points=std::make_shared<const std::vector<resident_point>>(nav.reachable_points(state.position));
            for(const auto& p:*points){int cell=nav.cell_at(p);if(cell>=0)regions[index][cell]=points;}
            regions[index][at]=std::move(points);
        }
        state.shared_reachable=regions[index][at];
        auto transform=entity->get_component<gb::ces_transformation_3d_component>();
        transform->set_position(glm::vec3(state.position.x,nav.shore_y,state.position.z));
    });
}
void ces_island_resident_system::apply_stairs(const std::vector<EditorStairs>& stairs,const terrain_config& config){
    m_stairs=stairs;m_stairs_config=config;
    m_stair_links=build_resident_stair_links(m_stairs,m_stairs_config,m_navigations);
    if(m_debug)std::cout<<"Resident stair links="<<m_stair_links.size()<<" flights="<<m_stairs.size()<<std::endl;
    enumerate_entities_with_components(m_mask,[&](const gb::ces_entity_shared_ptr& entity){
        auto resident=entity->get_component<ces_island_resident_component>();
        rebind_resident_stairs(m_navigations,m_stair_links,resident->terrace_index,resident->state);
    });
}
void ces_island_resident_system::on_feed_start(f32 dt){
    std::vector<std::vector<woodcut_target>> trees(m_navigations.size());
    if(m_editor_state)for(const auto& tree:m_editor_state->level.trees)for(size_t i=0;i<m_navigations.size();i++)if(std::abs(tree.position[1]-m_navigations[i].shore_y)<.01f)trees[i].push_back({tree.id,{tree.position[0],tree.position[2]},1.5f});
    enumerate_entities_with_components(m_mask,[&](const gb::ces_entity_shared_ptr& entity){
        auto resident=entity->get_component<ces_island_resident_component>();auto& state=resident->state;
        if(resident->axe){resident->axe->visible=false;resident->axe->visible_in_next_frame=false;}
        if(resident->terrace_index>=m_navigations.size()){state.available=false;state.walking=false;entity->visible=false;return;}
        const auto previous=state.position;
        if(!state.available){entity->visible=false;return;}
        if(m_fixed_pose.empty()){
            if(resident->woodcutter)step_woodcutter(m_navigations[resident->terrace_index],dt,state,resident->woodcutting,trees[resident->terrace_index]);
            else step_resident_with_stairs(m_navigations,m_stair_links,dt,resident->terrace_index,state);
        }
        else {const auto& navigation=m_navigations[resident->terrace_index];state.elevation=navigation.shore_y;state.available=navigation.is_safe(state.position);state.walking=state.available&&m_fixed_pose=="walk";}
        entity->visible=state.available;
        if(!state.available)return;
        if(resident->axe){resident->axe->visible=true;resident->axe->visible_in_next_frame=true;}
        auto transform=entity->get_component<gb::ces_transformation_3d_component>();
        const glm::vec3 position(state.position.x,state.elevation,state.position.z);
        if (transform->get_position()!=position) transform->set_position(position);
        // Navigation uses radians; the engine's legacy GLM transform uses degrees.
        const glm::vec3 rotation(0,glm::degrees(state.yaw),0);
        if (transform->get_rotation()!=rotation) transform->set_rotation(rotation);
        if(m_debug) {
            const glm::vec2 movement(state.position.x-previous.x,state.position.z-previous.z);
            // On arrival woodcutters turn toward the trunk instead of retaining
            // their final approach direction.
            if(glm::length(movement)>1.e-6f&&!resident->woodcutting.chopping) {
                const auto forward=transform->get_absolute_transformation()*glm::vec4(0,0,1,0);
                if(glm::dot(glm::normalize(movement),glm::normalize(glm::vec2(forward.x,forward.z)))<.999f)
                    std::cout<<"RESIDENT FACING MISMATCH"<<std::endl;
            }
        }
        const std::string clip=resident->woodcutter&&resident->woodcutting.chopping&&m_fixed_pose.empty()?"chop":state.walking?"walk":"idle";
        if(resident->animation!=clip){std::static_pointer_cast<gb::shape_3d>(entity)->play_animation(clip,true);resident->animation=clip;
            if(resident->woodcutter)std::cout<<"Woodcutter mode="<<clip<<" terrace="<<resident->terrace_index+1<<" tree="<<resident->woodcutting.tree_id<<std::endl;
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
        if(!resident->state.available)return;
        const auto& navigation=m_navigations[resident->terrace_index];
        if(report){++counts[resident->terrace_index];walkers+=resident->state.walking?1:0;
            auto transform=entity->get_component<gb::ces_transformation_3d_component>();
            if(!resident->state.available||(!resident->state.stair_transit&&!navigation.is_safe(resident->state.position))||std::abs(transform->get_position().y-resident->state.elevation)>.001f)++unsafe;
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
        std::static_pointer_cast<resident_color_uniforms>(uniforms->get_uniforms(gb::ces_shader_uniforms_component::e_shader_uniform_mode::e_fragment,"resident.color",0))->update(view,resident->clothing_color,camera->get_position());
        if(resident->axe){
            const auto body_model=entity->get_component<gb::ces_transformation_3d_component>()->get_absolute_transformation();
            auto axe_transform=resident->axe->get_component<gb::ces_transformation_3d_component>();
            axe_transform->update_absolute_transformation(body_model);
            if(m_debug){const auto axe_model=axe_transform->get_absolute_transformation();for(int c=0;c<4;c++)for(int r=0;r<4;r++)if(std::abs(axe_model[c][r]-body_model[c][r])>.00001f)std::cout<<"AXE PARENT MISMATCH"<<std::endl;}
            auto axe_uniforms=resident->axe->get_component<gb::ces_shader_uniforms_component>();
            auto axe_bones=std::static_pointer_cast<resident_bone_uniforms>(axe_uniforms->get_uniforms(gb::ces_shader_uniforms_component::e_shader_uniform_mode::e_vertex,"resident.bones",2));
            axe_bones->update(static_cast<glm::mat4*>(bones->get_values()),mixer->get_transformation_size());
            if(resident->woodcutting.chopping){
                // Fit the rigid handle through both animated palms. Correct only
                // the tool palette; retain the supplied body animation unchanged.
                auto palette=static_cast<glm::mat4*>(axe_bones->get_values());
                const auto& right=resident->axe_hands[0];const auto& left=resident->axe_hands[1];
                const glm::vec3 r=glm::vec3(palette[right.first]*glm::vec4(right.second,1));
                const glm::vec3 l=glm::vec3(palette[left.first]*glm::vec4(left.second,1));
                if(glm::length(l-r)>.001f){
                    // Left palm holds the butt, right palm is nearer the head.
                    // Do not pick this sign from the rotating right wrist: that
                    // can reverse the tool end-for-end during the swing.
                    const auto target=glm::normalize(r-l);
                    // Two palms constrain only the handle axis, leaving roll
                    // ambiguous. Fix the second axis explicitly: the cutting
                    // edge faces the trunk (+Z in the resident's local space).
                    auto edge=glm::vec3(0,0,1)-target*target.z;
                    if(glm::length(edge)<.001f)edge=glm::vec3(0,-1,0)-target*(-target.y);
                    edge=glm::normalize(edge);
                    const glm::mat3 bind_frame(resident->axe_edge_bind,resident->axe_axis_bind,glm::cross(resident->axe_edge_bind,resident->axe_axis_bind));
                    const glm::mat3 pose_frame(edge,target,glm::cross(edge,target));
                    palette[right.first]=glm::translate(glm::mat4(1),(r+l)*.5f)*glm::mat4(pose_frame*glm::transpose(bind_frame))*glm::translate(glm::mat4(1),-right.second);
                    if(m_debug&&report){
                        const auto grip=glm::vec3(palette[right.first]*glm::vec4(right.second,1));
                        const auto direction=glm::normalize(glm::vec3(palette[right.first]*glm::vec4(resident->axe_axis_bind,0)));
                        const float error=std::max(glm::length(glm::cross(r-grip,direction)),glm::length(glm::cross(l-grip,direction)));
                        std::cout<<"Woodcutter two-hand grip error="<<error<<" span="<<glm::length(l-r)<<std::endl;
                        if(!std::isfinite(error)||error>.001f)std::cout<<"AXE GRIP MISMATCH"<<std::endl;
                        if(glm::dot(direction,glm::normalize(r-l))<.999f)std::cout<<"AXE HEAD DIRECTION MISMATCH"<<std::endl;
                        const auto actual_edge=glm::normalize(glm::vec3(palette[right.first]*glm::vec4(resident->axe_edge_bind,0)));
                        if(glm::dot(actual_edge,edge)<.999f||actual_edge.z<-.001f)std::cout<<"AXE EDGE MISMATCH"<<std::endl;
                    }
                }
            }
            resident->axe->get_component<gb::ces_material_component>()->get_material("ws.base")->set_custom_shader_uniform(static_cast<glm::mat4*>(axe_bones->get_values()),64,"u_bones");
            std::static_pointer_cast<resident_color_uniforms>(axe_uniforms->get_uniforms(gb::ces_shader_uniforms_component::e_shader_uniform_mode::e_fragment,"resident.color",0))->update(view,glm::vec4(1),camera->get_position());
        }
        if(m_debug&&m_elapsed>=m_next_report){
            const auto& s=resident->state;std::cout<<"Resident position="<<s.position.x<<","<<s.elevation<<","<<s.position.z<<" safe="<<(s.stair_transit||navigation.is_safe(s.position))<<" bones="<<mixer->get_transformation_size()<<std::endl;
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
        m_next_report=m_elapsed+1;
        std::cout<<"Resident crowd report: walkers="<<walkers<<" unsafe="<<unsafe<<" counts=";
        for(auto count:counts)std::cout<<count<<",";
        std::cout<<std::endl;
    }
}
}
