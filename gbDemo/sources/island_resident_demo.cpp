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
#include "terrain_editor_state.h"
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
resident_demo_handles create_island_resident_demo(const gb::scene_fabricator_shared_ptr& fabricator,const terrain_mesh_data& terrain,const terrain_config& config,const std::shared_ptr<TerrainEditorState>& editor){
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
    // Higher editor terraces start empty, but remain reachable through stairs.
    for(size_t level=levels;level<16;++level)
        navigations.push_back(build_resident_navigation(terrain,level*config.layer_height));
    // Guarantee a visible, reachable tree and a short initial approach on every
    // populated dry terrace, without changing any loaded level file.
    for(size_t level=0;level<levels;level++){
        if(populations[level].empty())continue;
        auto& nav=navigations[level];auto& worker=populations[level].front();
        const EditorTree* chosen=nullptr;float nearest=1.e30f;
        for(const auto& tree:editor->level.trees){if(std::abs(tree.position[1]-nav.shore_y)>.01f||!nav.is_safe({tree.position[0],tree.position[2]}))continue;const float dx=tree.position[0]-worker.position.x,dz=tree.position[2]-worker.position.z;if(dx*dx+dz*dz<nearest){nearest=dx*dx+dz*dz;chosen=&tree;}}
        if(!chosen){
            for(int radius=3;radius<=12&&!chosen;radius++)for(int side=0;side<16&&!chosen;side++){const float a=side*.3926990817f;resident_point p={worker.position.x+std::sin(a)*radius,worker.position.z+std::cos(a)*radius};if(!nav.is_safe(p)||editor->level_at(p.x,p.z)!=int(level)+1)continue;
                EditorTree tree;tree.id=editor->level.next_tree_id++;tree.position={{p.x,nav.shore_y,p.z}};tree.type=TreeType::Oak;editor->level.trees.push_back(tree);chosen=&editor->level.trees.back();
            }
        }
        if(!chosen)throw std::runtime_error("No safe woodcutting tree on terrace");
        for(int side=0;side<16;side++){const float a=side*.3926990817f;resident_point p={chosen->position[0]+std::sin(a)*3.2f,chosen->position[2]+std::cos(a)*3.2f};resident_point stand={chosen->position[0]+std::sin(a)*1.5f,chosen->position[2]+std::cos(a)*1.5f};if(!nav.is_safe(p)||!nav.is_safe(stand)||!nav.segment_is_safe(p,stand))continue;worker.position=p;worker.shared_reachable.reset();worker.reachable=nav.reachable_points(p);break;}
        std::cout<<"Woodcutter terrace="<<level+1<<" tree="<<chosen->id<<" tree_position="<<chosen->position[0]<<","<<chosen->position[2]<<std::endl;
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
        resident->terrace_index=level;resident->woodcutter=&state==&populations[level].front();
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
        if(resident->woodcutter){
            auto axe=fabricator->create_shape_3d("woodcutter_axe.xml");axe->tag="island.axe."+std::to_string(level);
            axe->get_component<gb::ces_geometry_3d_component>()->bounding_mode=gb::ces_geometry_component::e_none;
            auto axe_mesh=std::static_pointer_cast<gb::mesh_3d>(axe->get_component<gb::ces_geometry_3d_component>()->get_mesh());
            const auto& body_bones=mesh->get_skeleton_data()->get_bones_raw_data();const auto& axe_bones=axe_mesh->get_skeleton_data()->get_bones_raw_data();
            if(body_bones.size()!=axe_bones.size())throw std::runtime_error("Axe skeleton differs from resident");
            for(size_t i=0;i<body_bones.size();i++)if(body_bones[i]->get_bone_name()!=axe_bones[i]->get_bone_name())throw std::runtime_error("Axe bone indices differ from resident");
            const auto body_buffer=mesh->get_vbo();const auto body_vertices=body_buffer->lock<gb::vbo::vertex_attribute_PT4B>();
            for(int hand=0;hand<2;hand++){
                int id=-1;for(const auto& bone:body_bones)if(bone->get_bone_name()==(hand?"hand_l":"hand_r"))id=bone->get_bone_id();
                glm::vec3 low(std::numeric_limits<float>::max()),high(-std::numeric_limits<float>::max());bool found=false;
                for(ui32 v=0;v<body_buffer->get_used_size();v++)for(int w=0;w<4;w++)if(body_vertices[v].m_bone_ids[w]==id&&body_vertices[v].m_bone_weights[w]==255){low=glm::min(low,body_vertices[v].m_position);high=glm::max(high,body_vertices[v].m_position);found=true;}
                if(id<0||!found)throw std::runtime_error("Axe palm attachment missing");
                resident->axe_hands[hand]={id,(low+high)*.5f};
            }
            // The exporter's long handle axis is the longest mesh extent.
            const auto extent=axe_mesh->get_max_bound()-axe_mesh->get_min_bound();
            int axis=extent.y>extent.x?1:0;if(extent.z>extent[axis])axis=2;
            resident->axe_axis_bind=glm::vec3(0);resident->axe_axis_bind[axis]=1;
            int edge=(axis+1)%3;if(extent[(axis+2)%3]>extent[edge])edge=(axis+2)%3;
            const auto grip=resident->axe_hands[0].second;
            const auto low=axe_mesh->get_min_bound()-grip,high=axe_mesh->get_max_bound()-grip;
            // The head and cutting edge extend farther from the grip than the
            // butt and rear of the handle. Preserve these signed asset axes.
            resident->axe_axis_bind[axis]=std::abs(high[axis])>std::abs(low[axis])?1.f:-1.f;
            resident->axe_edge_bind=glm::vec3(0);resident->axe_edge_bind[edge]=std::abs(high[edge])>std::abs(low[edge])?1.f:-1.f;
            auto u=axe->add_component<gb::ces_shader_uniforms_component>();u->construct_uniforms_buffer<resident_bone_uniforms>(gb::ces_shader_uniforms_component::e_shader_uniform_mode::e_vertex,"resident.bones",2);
            auto c=u->construct_uniforms_buffer<resident_color_uniforms>(gb::ces_shader_uniforms_component::e_shader_uniform_mode::e_fragment,"resident.color",0);c->set_outline_id(8500000.f+level);
            shape->add_child(axe);resident->axe=axe;
        }
        result.shapes.push_back(shape);
    }
    result.system=std::make_shared<ces_island_resident_system>(std::move(navigations));result.system->set_woodcutting_state(editor);
    if(clothing_customization_enabled)result.input=std::make_shared<resident_color_input>(result.shapes);
    std::cout<<"Resident crowd initialized: count="<<result.shapes.size()<<" terraces="<<levels<<std::endl;
    for(size_t level=0;level<levels;++level)std::cout<<"Resident terrace="<<level<<" count="<<populations[level].size()<<" height="<<(level?level*config.layer_height:config.shore_height)<<std::endl;
    return result;
}
}
