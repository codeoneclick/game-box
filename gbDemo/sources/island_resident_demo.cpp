#include "island_resident_demo.h"
#include "game_transition.h"
#include "scene_fabricator.h"
#include "shape_3d.h"
#include "mesh_3d.h"
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
    glm::vec4 color(int index){static const glm::vec4 colors[]={glm::vec4(.055f,.23f,.43f,1),glm::vec4(.12f,.32f,.09f,1),glm::vec4(.48f,.085f,.055f,1),glm::vec4(.52f,.28f,.045f,1)};return colors[index%4];}
    class resident_color_input final:public gb::input_context_listener {
        gb::ces_entity_weak_ptr m_entity;bool m_pressed=false;
    public:
        explicit resident_color_input(const gb::ces_entity_shared_ptr& e):m_entity(e){}
        void on_gr_pressed(const glm::ivec2&,const glm::ivec2&,gb::e_input_source,ui32)override{}
        void on_gr_released(const glm::ivec2&,const glm::ivec2&,gb::e_input_source,ui32)override{}
        void on_gr_moved(const glm::ivec2&,const glm::ivec2&,const glm::ivec2&,ui32)override{}
        void on_gr_dragged(const glm::ivec2&,const glm::ivec2&,const glm::ivec2&,gb::e_input_source,ui32)override{}
        void on_key_down(i32 key)override{
            if((key=='C'||key=='c')&&!m_pressed){m_pressed=true;if(auto entity=m_entity.lock()){
                auto c=entity->get_component<ces_island_resident_component>();if(c){c->color_index=(c->color_index+1)%4;c->clothing_color=color(c->color_index);std::cout<<"Resident clothing color="<<c->color_index<<std::endl;}}}
        }
        void on_key_up(i32 key)override{if(key=='C'||key=='c')m_pressed=false;}
        void on_virtual_keyboard_input(const std::string&)override{}
        void on_virtual_keyboard_backspace()override{}
        void on_virtual_keyboard_hidden()override{}
    };
}
resident_demo_handles create_island_resident_demo(const gb::scene_fabricator_shared_ptr& fabricator,const terrain_mesh_data& terrain,float y){
    resident_demo_handles result;auto navigation=build_resident_navigation(terrain,y);
    auto state=create_resident_state(navigation,1337);if(!state.available)throw std::runtime_error("No safe resident shore spawn");
    std::array<std::string,3> files={"island_resident.fbx","island_resident_idle.fbx","island_resident_walk.fbx"};
    for(auto& filename:files){std::ifstream direct(filename,std::ios::binary);if(!direct)filename=bundlepath()+filename;}
    validate_resident_fbx_files(files);
    auto shape=fabricator->create_shape_3d("island_resident.xml");
    auto geometry=shape->get_component<gb::ces_geometry_3d_component>();
    auto mesh=std::static_pointer_cast<gb::mesh_3d>(geometry->get_mesh());
    auto mixer=shape->get_component<gb::ces_animation_3d_mixer_component>();
    if(!mixer||mixer->get_transformation_size()<1||mixer->get_transformation_size()>64)throw std::runtime_error("Resident skeleton/animations missing");
    const float height=mesh->get_max_bound().y-mesh->get_min_bound().y;
    if(std::abs(height-1.9f)>.05f)throw std::runtime_error("Resident FBX scale mismatch");
    geometry->bounding_mode=gb::ces_geometry_component::e_radius;geometry->set_bounding_radius(2.2f);
    shape->tag="island.resident";
    if(std::getenv("GB_RESIDENT_DEBUG")){
        auto draw_reported=std::make_shared<bool>(false);
        shape->get_component<gb::ces_material_component>()->set_bind_material_imposer_callback([draw_reported](const gb::material_shared_ptr&){
            if(!*draw_reported){std::cout<<"Resident material drawn"<<std::endl;*draw_reported=true;}
        });
    }
    auto resident=shape->add_component<ces_island_resident_component>();resident->state=std::move(state);
    if(const auto value=std::getenv("GB_RESIDENT_COLOR"))resident->color_index=std::max(0,std::min(3,std::atoi(value)));
    resident->clothing_color=color(resident->color_index);
    const auto& p=resident->state.position;result.focus=glm::vec3(p.x,y+.95f,p.z);
    shape->get_component<gb::ces_transformation_3d_component>()->set_position(glm::vec3(p.x,y,p.z));
    auto uniforms=shape->add_component<gb::ces_shader_uniforms_component>();
    uniforms->construct_uniforms_buffer<resident_bone_uniforms>(gb::ces_shader_uniforms_component::e_shader_uniform_mode::e_vertex,"resident.bones",2);
    uniforms->construct_uniforms_buffer<resident_color_uniforms>(gb::ces_shader_uniforms_component::e_shader_uniform_mode::e_fragment,"resident.color",0);
    shape->play_animation("idle",true);resident->animation="idle";
    result.shape=shape;result.system=std::make_shared<ces_island_resident_system>(std::move(navigation));
    result.input=std::make_shared<resident_color_input>(shape);
    std::cout<<"Resident initialized: height="<<height<<" bones="<<mixer->get_transformation_size()<<" clothing="<<resident->color_index<<" spawn="<<p.x<<","<<p.z<<std::endl;
    return result;
}
}
