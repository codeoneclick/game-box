#include "island_resident_demo.h"
#include "game_transition.h"
#include "scene_fabricator.h"
#include "shape_3d.h"
#include "mesh_3d.h"
#include "bone_3d.h"
#include "vbo.h"
#include "ces_geometry_3d_component.h"
#include "ces_transformation_3d_component.h"
#include "ces_island_resident_component.h"
#include "ces_island_resident_system.h"
#include "resident_shader_uniforms.h"
#include "ces_animation_3d_mixer_component.h"
#include "input_context.h"
#include "ces_material_component.h"
#include "resident_asset_validation.h"
#include "common.h"
#include <fstream>
#include <stdexcept>
#include <cstdlib>
namespace game {
namespace {
    // Temporarily preserve the final Woodsman texture, including C/env input.
    constexpr bool clothing_customization_enabled=false;
    glm::vec4 color(int index){static const glm::vec4 colors[]={glm::vec4(.055f,.23f,.43f,1),glm::vec4(.12f,.32f,.09f,1),glm::vec4(.48f,.085f,.055f,1),glm::vec4(.52f,.28f,.045f,1)};return colors[index%4];}
    class resident_color_input final:public gb::input_context_listener {
        std::vector<gb::ces_entity_weak_ptr> m_entities;bool m_pressed=false;
    public:
        explicit resident_color_input(const std::vector<gb::shape_3d_shared_ptr>& entities){for(const auto& e:entities)m_entities.push_back(e);}
        void on_gr_pressed(const glm::ivec2&,const glm::ivec2&,gb::e_input_source,ui32)override{}
        void on_gr_released(const glm::ivec2&,const glm::ivec2&,gb::e_input_source,ui32)override{}
        void on_gr_moved(const glm::ivec2&,const glm::ivec2&,const glm::ivec2&,ui32)override{}
        void on_gr_dragged(const glm::ivec2&,const glm::ivec2&,const glm::ivec2&,gb::e_input_source,ui32)override{}
        void on_key_down(i32 key)override{
            if((key=='C'||key=='c')&&!m_pressed){m_pressed=true;bool reported=false;for(const auto& weak:m_entities)if(auto entity=weak.lock()){
                auto c=entity->get_component<ces_island_resident_component>();if(c){c->color_index=(c->color_index+1)%4;c->clothing_color=color(c->color_index);
                    if(!reported){std::cout<<"Resident clothing color="<<c->color_index<<std::endl;reported=true;}}}}
        }
        void on_key_up(i32 key)override{if(key=='C'||key=='c')m_pressed=false;}
        void on_virtual_keyboard_input(const std::string&)override{}
        void on_virtual_keyboard_backspace()override{}
        void on_virtual_keyboard_hidden()override{}
    };
}
resident_demo_handles create_island_resident_demo(const gb::scene_fabricator_shared_ptr& fabricator,const terrain_mesh_data& terrain,const terrain_config& config){
    resident_demo_handles result;
    const auto count_override=std::getenv("GB_RESIDENT_COUNT");
    const size_t total=count_override?static_cast<size_t>(std::max(0,std::min(200,std::atoi(count_override)))):200;
    const size_t levels=static_cast<size_t>(config.layers_count);
    if(!levels)throw std::runtime_error("No dry terraces for resident population");
    std::vector<resident_navigation> navigations;
    std::vector<std::vector<resident_state>> populations;
    for(size_t level=0;level<levels;++level){
        const float y=level?level*config.layer_height:config.shore_height;
        navigations.push_back(build_resident_navigation(terrain,y));
        populations.push_back(create_resident_population(navigations.back(),total/levels+(level<total%levels?1:0),config.seed+static_cast<uint32_t>(level)));
    }
    std::array<std::string,3> files={"woodsman.fbx","character_idle.fbx","character_walking.fbx"};
    for(auto& filename:files){std::ifstream direct(filename,std::ios::binary);if(!direct)filename=bundlepath()+filename;}
    validate_resident_fbx_files(files,true);
    auto draw_reported=std::make_shared<bool>(false);
    std::shared_ptr<std::vector<std::pair<int,glm::vec3>>> foot_contacts;
    for(size_t level=0;level<levels;++level)for(auto& state:populations[level]){
        const float y=navigations[level].shore_y;
        auto shape=fabricator->create_shape_3d("island_resident.xml");
        auto geometry=shape->get_component<gb::ces_geometry_3d_component>();
        auto mesh=std::static_pointer_cast<gb::mesh_3d>(geometry->get_mesh());
        auto mixer=shape->get_component<gb::ces_animation_3d_mixer_component>();
        if(!mixer||mixer->get_transformation_size()<1||mixer->get_transformation_size()>64)throw std::runtime_error("Resident skeleton/animations missing");
        const float height=mesh->get_max_bound().y-mesh->get_min_bound().y;
        if(std::abs(height-1.9f)>.05f)throw std::runtime_error("Resident FBX scale mismatch");
        if(!foot_contacts) {
            foot_contacts=std::make_shared<std::vector<std::pair<int,glm::vec3>>>();
            std::vector<int> feet;
            for(const auto& bone:mesh->get_skeleton_data()->get_bones_raw_data())
                if(bone->get_bone_name()=="foot_l"||bone->get_bone_name()=="foot_r")feet.push_back(bone->get_bone_id());
            const auto buffer=mesh->get_vbo();const auto vertices=buffer->lock<gb::vbo::vertex_attribute_PT4B>();
            for(ui32 i=0;i<buffer->get_used_size();++i)for(int j=0;j<4;++j) {
                const auto& v=vertices[i];const int id=v.m_bone_ids[j];
                if(v.m_bone_weights[j]!=255||std::find(feet.begin(),feet.end(),id)==feet.end())continue;
                const auto duplicate=std::find_if(foot_contacts->begin(),foot_contacts->end(),[&](const std::pair<int,glm::vec3>& c){return c.first==id&&glm::length(c.second-v.m_position)<1.e-6f;});
                if(duplicate==foot_contacts->end())foot_contacts->emplace_back(id,v.m_position);
            }
            if(foot_contacts->empty())throw std::runtime_error("Resident foot contact skin missing");
        }
        geometry->bounding_mode=gb::ces_geometry_component::e_radius;geometry->set_bounding_radius(2.2f);
        shape->tag=result.shapes.empty()?"island.resident":"island.resident."+std::to_string(result.shapes.size());
        if(std::getenv("GB_RESIDENT_DEBUG")){
            shape->get_component<gb::ces_material_component>()->set_bind_material_imposer_callback([draw_reported](const gb::material_shared_ptr&){
                if(!*draw_reported){std::cout<<"Resident material drawn"<<std::endl;*draw_reported=true;}
            });
        }
        auto resident=shape->add_component<ces_island_resident_component>();resident->state=std::move(state);
        resident->foot_contacts=foot_contacts;
        resident->terrace_index=level;
        if(clothing_customization_enabled)if(const auto value=std::getenv("GB_RESIDENT_COLOR"))resident->color_index=std::max(0,std::min(3,std::atoi(value)));
        resident->clothing_color=clothing_customization_enabled?color(resident->color_index):glm::vec4(1.f);
        const auto& p=resident->state.position;if(result.shapes.empty())result.focus=glm::vec3(p.x,y+.95f,p.z);
        shape->get_component<gb::ces_transformation_3d_component>()->set_position(glm::vec3(p.x,y,p.z));
        auto uniforms=shape->add_component<gb::ces_shader_uniforms_component>();
        uniforms->construct_uniforms_buffer<resident_bone_uniforms>(gb::ces_shader_uniforms_component::e_shader_uniform_mode::e_vertex,"resident.bones",2);
        auto color_uniforms=uniforms->construct_uniforms_buffer<resident_color_uniforms>(gb::ces_shader_uniforms_component::e_shader_uniform_mode::e_fragment,"resident.color",0);
        color_uniforms->set_outline_id(8000000.f+static_cast<float>(result.shapes.size()));
        shape->play_animation("idle",true);resident->animation="idle";
        if(result.shapes.empty())std::cout<<"Resident initialized: height="<<height<<" bones="<<mixer->get_transformation_size()<<" clothing="<<resident->color_index<<" spawn="<<p.x<<","<<p.z<<std::endl;
        result.shapes.push_back(shape);
    }
    result.system=std::make_shared<ces_island_resident_system>(std::move(navigations));
    if(clothing_customization_enabled)result.input=std::make_shared<resident_color_input>(result.shapes);
    std::cout<<"Resident crowd initialized: count="<<result.shapes.size()<<" terraces="<<levels<<std::endl;
    for(size_t level=0;level<levels;++level)std::cout<<"Resident terrace="<<level<<" count="<<populations[level].size()<<" height="<<(level?level*config.layer_height:config.shore_height)<<std::endl;
    return result;
}
}
