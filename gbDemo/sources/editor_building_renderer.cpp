#include "editor_building_renderer.h"
#include "terrain_editor_state.h"
#include "terrain_editor_input.h"
#include "scene_fabricator.h"
#include "shape_3d.h"
#include "ces_transformation_3d_component.h"
#include "ces_shader_uniforms_component.h"
#include "ces_geometry_3d_component.h"
#include "shader.h"
#include "mesh_3d.h"
#include "vbo.h"
#include "ibo.h"
#include <algorithm>
#include "stylized_lighting_uniforms.h"
#include <array>
#include <map>
namespace game {
namespace {
class building_uniforms final : public gb::ces_shader_uniforms_component::shader_uniforms {
    struct values {glm::vec4 tint,sun;glm::mat4 view;StylizedLightingUniformValues lighting;} values_;
public:
    building_uniforms(gb::ces_shader_uniforms_component::e_shader_uniform_mode mode,const std::string& name):shader_uniforms(mode,name){
        m_uniforms["building_tint"]=std::make_shared<gb::shader_uniform>(gb::e_uniform_type_vec4);
        m_uniforms["building_sun"]=std::make_shared<gb::shader_uniform>(gb::e_uniform_type_vec4);
        m_uniforms["building_view"]=std::make_shared<gb::shader_uniform>(gb::e_uniform_type_mat4);
        for(const auto& key:lighting_uniform_names())m_uniforms[key]=std::make_shared<gb::shader_uniform>(gb::e_uniform_type_vec4);
        values_.tint=glm::vec4(0);values_.sun=glm::vec4(.35f,.86f,.38f,0);values_.view=glm::mat4(1);
    }
    void update(glm::vec4 tint,const glm::mat4& view,const glm::vec3& camera){const auto d=stylized_lighting_settings().sun_direction();values_.sun=glm::vec4(d[0],d[1],d[2],values_.sun.w);values_.tint=tint;values_.view=view;values_.lighting=make_lighting_uniforms(camera);set(tint,"building_tint");set(values_.sun,"building_sun");set(view,"building_view");set_lighting_uniforms(*this,values_.lighting);}
    void set_outline_id(float id){values_.sun.w=id;}
    void* get_values()override{return &values_;}
    ui32 get_values_size()override{static_assert(sizeof(values)==208,"Building shader uniform layout");return sizeof(values);}
};
const std::array<const char*,3> configurations={{"building.forester.xml","building.stonecutter.xml","building.woodcutter.xml"}};
}
struct EditorBuildingRenderer::implementation {
    struct entry {gb::shape_3d_shared_ptr shape;std::shared_ptr<building_uniforms> uniforms;BuildingType type;};
    gb::scene_fabricator_shared_ptr fabricator;
    std::shared_ptr<TerrainEditorState> state;
    gb::ces_entity_shared_ptr root=gb::ces_entity::construct<gb::ces_entity>();
    std::map<uint32_t,entry> instances;
    std::array<entry,3> previews;
    gb::shape_3d_shared_ptr aura,stairs_preview;
    std::shared_ptr<building_uniforms> aura_uniforms;
    uint32_t next_outline_id=12000000;
    entry make(BuildingType type){
        entry result;result.type=type;result.shape=fabricator->create_shape_3d(configurations.at(static_cast<size_t>(type)));result.shape->tag="editor.building.preview";
        result.uniforms=result.shape->add_component<gb::ces_shader_uniforms_component>()->construct_uniforms_buffer<building_uniforms>(gb::ces_shader_uniforms_component::e_shader_uniform_mode::e_fragment,"building.preview",0);
        if(next_outline_id>=16777216u)throw std::overflow_error("Building outline identity limit reached");
        result.uniforms->set_outline_id(static_cast<float>(next_outline_id++));
        root->add_child(result.shape);return result;
    }
    implementation(const gb::scene_fabricator_shared_ptr& f,const std::shared_ptr<TerrainEditorState>& s):fabricator(f),state(s){
        root->tag="editor.buildings";
        for(int i=0;i<3;++i){previews[i]=make(static_cast<BuildingType>(i));previews[i].shape->visible=false;}
        aura=fabricator->create_shape_3d("building.aura.xml","plane");
        // The primitive plane has batching-only buffers. This material draws
        // directly, so provide its own ordinary GPU buffers on every backend.
        auto vb=std::make_shared<gb::vbo>(std::make_shared<gb::vbo::vertex_declaration_PTNTC>(4),gb::gl::constant::static_draw);
        auto vertices=vb->lock<gb::vbo::vertex_attribute_PTNTC>();
        for(int i=0;i<4;++i){vertices[i]={};vertices[i].m_position=glm::vec3(i%2?.5f:-.5f,i/2?.5f:-.5f,0);vertices[i].m_texcoord=glm::packHalf2x16(glm::vec2(i%2,i/2));vertices[i].m_color=glm::u8vec4(255);}
        vb->unlock();auto ib=std::make_shared<gb::ibo>(6,gb::gl::constant::static_draw);auto indices=ib->lock();
        const uint16_t order[6]={0,1,2,1,3,2};std::copy(order,order+6,indices);ib->unlock();
        aura->get_component<gb::ces_geometry_3d_component>()->set_mesh(gb::mesh_3d::construct("editor.placement.aura",vb,ib,glm::vec3(.5f,.5f,0),glm::vec3(-.5f,-.5f,0)));
        aura->get_component<gb::ces_geometry_3d_component>()->bounding_mode=gb::ces_geometry_component::e_none;
        aura_uniforms=aura->add_component<gb::ces_shader_uniforms_component>()->construct_uniforms_buffer<building_uniforms>(gb::ces_shader_uniforms_component::e_shader_uniform_mode::e_fragment,"building.preview",0);
        aura->visible=false;root->add_child(aura);
    }
    void update(const TerrainEditorInput& input,const glm::mat4& view){
        const auto camera=glm::vec3(glm::inverse(view)[3]);
        for(auto it=instances.begin();it!=instances.end();){
            bool present=false;for(const auto& b:state->level.buildings)if(b.id==it->first){present=true;break;}
            if(!present){root->remove_child(it->second.shape);it=instances.erase(it);}else ++it;
        }
        for(const auto& b:state->level.buildings){
            auto found=instances.find(b.id);
            if(found!=instances.end()&&found->second.type!=b.type){root->remove_child(found->second.shape);instances.erase(found);found=instances.end();}
            if(found==instances.end())found=instances.emplace(b.id,make(b.type)).first;
            auto& e=found->second;e.shape->tag="island.building."+std::to_string(b.id);auto t=e.shape->get_component<gb::ces_transformation_3d_component>();
            t->set_position(glm::vec3(b.position[0],b.position[1],b.position[2]));t->set_rotation(glm::vec3(0,glm::degrees(b.yaw),0));e.uniforms->update(glm::vec4(0),view,camera);
        }
        const bool show=input.building_preview_visible();
        const bool show_stairs=input.stairs_preview_visible();
        for(size_t i=0;i<previews.size();++i)previews[i].shape->visible=show&&i==static_cast<size_t>(state->selected_building);
        aura->visible=show||show_stairs;
        if(stairs_preview)stairs_preview->visible=show_stairs;
        if(show_stairs){
            const auto position=input.brush_position();EditorStairs candidate;
            const bool valid=state->stairs_preview(position.x,position.z,candidate);
            if(stairs_preview){
                const auto& config=state->level.terrain.config;
                const float rise=candidate.lower_terrace==1?config.layer_height-config.shore_height:config.layer_height;
                auto transform=stairs_preview->get_component<gb::ces_transformation_3d_component>();
                transform->set_position(glm::vec3(candidate.position[0],candidate.position[1]+.025f,candidate.position[2]));
                transform->set_rotation(glm::vec3(0,glm::degrees(candidate.yaw),0));
                transform->set_scale(glm::vec3(1,rise/terrain_config().layer_height,1));
            }
            const auto tint=valid?glm::vec4(.12f,1.f,.12f,1.f):glm::vec4(1.f,.035f,.015f,1.f);
            auto at=aura->get_component<gb::ces_transformation_3d_component>();
            at->set_position(glm::vec3(candidate.position[0],candidate.position[1]+.04f,candidate.position[2]));
            at->set_rotation(glm::vec3(0,glm::degrees(candidate.yaw),0));
            // Keep the luminous ring outside the solid tread footprint.
            at->set_scale(glm::vec3(stairs_width+1.2f,1,stairs_run+1.2f));
            aura_uniforms->update(tint,glm::inverse(at->get_absolute_transformation()),camera);

            return;
        }
        if(!show)return;
        const auto position=input.brush_position();const bool valid=state->can_place_building(position.x,position.z);
        if(std::getenv("GB_BUILDING_PLACEMENT_TEST")){
            std::cout<<"Building placement preview valid="<<valid<<" buildings="<<state->level.buildings.size()<<" surface_revision="<<state->building_surface_revision<<" terrain_revision="<<state->terrain_revision<<std::endl;
        }
        const auto tint=valid?glm::vec4(.12f,1.f,.12f,.32f):glm::vec4(1.f,.035f,.015f,.38f);
        auto& preview=previews.at(static_cast<size_t>(state->selected_building));auto t=preview.shape->get_component<gb::ces_transformation_3d_component>();
        t->set_position(position);t->set_rotation(glm::vec3(0,glm::degrees(state->building_yaw),0));preview.uniforms->update(tint,view,camera);
        const auto& definition=building_definition(state->selected_building);auto at=aura->get_component<gb::ces_transformation_3d_component>();
        at->set_position(position+glm::vec3(0,.04f,0));at->set_rotation(glm::vec3(0,glm::degrees(state->building_yaw),0));
        at->set_scale(glm::vec3((definition.half_width+.5f)*2.f,1,(definition.half_depth+.5f)*2.f));
        aura_uniforms->update(glm::vec4(tint.x,tint.y,tint.z,1),glm::inverse(at->get_absolute_transformation()),camera);
    }
};
EditorBuildingRenderer::EditorBuildingRenderer(const gb::scene_fabricator_shared_ptr& f,const std::shared_ptr<TerrainEditorState>& s):impl_(std::make_shared<implementation>(f,s)){}
gb::ces_entity_shared_ptr EditorBuildingRenderer::root()const{return impl_->root;}
void EditorBuildingRenderer::set_stairs_preview(const gb::shape_3d_shared_ptr& shape){
    if(impl_->stairs_preview)impl_->root->remove_child(impl_->stairs_preview);
    impl_->stairs_preview=shape;if(shape){shape->visible=false;impl_->root->add_child(shape);}
}
void EditorBuildingRenderer::update(const TerrainEditorInput& input,const glm::mat4& view){impl_->update(input,view);}
}
