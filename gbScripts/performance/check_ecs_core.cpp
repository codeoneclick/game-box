#include "ces_transformation_3d_component.h"
#include "ces_animation_3d_mask_component.h"
#include "ces_skeleton_3d_component.h"
#include "ces_animation_3d_mixer_component.h"
#include "ces_animation_3d_system.h"
#include "ces_systems_feeder.h"
#include "scene_graph.h"
#include "animation_3d_sequence.h"
#include "mesh_3d.h"
#include "bone_3d.h"
#include <type_traits>
#include <iostream>
#include <cassert>

class probe_feeder final:public gb::ces_systems_feeder {
public:void tick(){on_update(0.f);}
};
class probe_query final:public gb::ces_base_system {
    std::bitset<255> mask;
public:
    STTI_CLASS_GUID(probe_query,gb::ces_base_system::g_guids_container)
    std::vector<gb::ces_entity_shared_ptr> found;
    probe_query(){add_required_component_guid(mask,gb::ces_animation_3d_mixer_component::class_guid());add_required_components_mask(mask);}
    void on_feed_start(f32)override{}
    void on_feed(const gb::ces_entity_shared_ptr&,f32)override{found.clear();enumerate_entities_with_components(mask,[&](const gb::ces_entity_shared_ptr& e){found.push_back(e);});}
    void on_feed_end(f32)override{}
};
int main(){
    static_assert(std::is_reference<decltype(std::declval<gb::ces_skeleton_3d_component>().get_root_bones())>::value,"No root-set copy");
    gb::animation_3d_mask weights;
    weights.add_bone_mask("root",.25f);weights.add_bone_mask("root",.75f);
    assert(weights.get_bone_weight("root")==.25f&&weights.get_bone_weight("unknown")==0.f);
    gb::ces_transformation_3d_component transform;
    transform.set_position(glm::vec3(1,2,3));
    const auto parent=glm::translate(glm::mat4(1),glm::vec3(4,5,6));
    transform.update_absolute_transformation(parent);
    const auto expected=transform.get_absolute_transformation();const auto version=transform.get_absolute_matrix_version();
    transform.update_absolute_transformation(parent);
    assert(transform.get_absolute_transformation()==expected&&transform.get_absolute_matrix_version()==version);
    auto scene=gb::ces_entity::construct<gb::scene_graph>(nullptr);
    auto feeder=std::make_shared<probe_feeder>();scene->set_systems_feeder(feeder);feeder->set_root(scene);
    auto animation=std::make_shared<gb::ces_animation_3d_system>();feeder->add_system(animation);
    auto query=std::make_shared<probe_query>();feeder->add_system(query);
    auto rig=std::make_shared<gb::skeleton_3d_transfering_data>(2);rig->add_bone(0,-1,"root");rig->add_bone(1,0,"child");
    auto frame=[](f32 x){return std::make_shared<gb::frame_3d_data>(std::vector<glm::quat>(2,glm::quat(1,0,0,0)),std::vector<glm::vec3>{glm::vec3(x,0,0),glm::vec3(x,2,0)},std::vector<glm::vec3>(2,glm::vec3(1)));};
    auto bind=std::make_shared<gb::sequence_3d_transfering_data>("bindpose",30,std::vector<gb::frame_3d_data_shared_ptr>{frame(0)});
    auto data=std::make_shared<gb::sequence_3d_transfering_data>("walk",30,std::vector<gb::frame_3d_data_shared_ptr>{frame(0),frame(1),frame(2)});
    auto sequence=gb::animation_3d_sequence::construct("test.walk",data);
    const auto revision=data->get_frame(1)->get_revision();
    data->get_frame(1)->update_position(0,data->get_frame(1)->get_position(0));
    assert(data->get_frame(1)->get_revision()>revision);
    std::vector<gb::ces_entity_shared_ptr> entities;
    for(int i=0;i<3;++i){
        auto e=gb::ces_entity::construct<gb::ces_entity>();
        auto skeleton=e->add_component<gb::ces_skeleton_3d_component>();skeleton->setup(rig,bind);
        auto mixer=e->add_component<gb::ces_animation_3d_mixer_component>();mixer->setup(rig,bind);
        gb::ces_animation_3d_system::bind_pose(mixer,skeleton);
        mixer->add_animation_sequence(sequence);mixer->set_animation("walk",true);mixer->validate_current_animation_sequence(0);
        mixer->set_should_blend_with_previous_sequence(false);mixer->reset_animation_time(0);mixer->update_animation_time(0,(i==2?.75f:.5f)/30.f);
        scene->add_child(e);entities.push_back(e);
    }
    assert(query->found.empty());feeder->tick();assert(query->found==entities);
    for(size_t i=0;i<entities.size();++i){auto mixer=entities[i]->get_component<gb::ces_animation_3d_mixer_component>();
        for(int bone=0;bone<2;++bone)assert(mixer->get_transformations()[bone]==glm::translate(glm::mat4(1),glm::vec3(i==2?.75f:.5f,0,0)));}
    // Public frame edits from a callback between consecutive instances invalidate sampling reuse.
    auto edited_mixer=entities[1]->get_component<gb::ces_animation_3d_mixer_component>();
    edited_mixer->update_previous_played_frame(0,100);
    edited_mixer->add_animation_ended_callback(entities[1],[data](const gb::ces_entity_shared_ptr& e,const std::string&,bool){
        data->get_frame(1)->update_position(0,glm::vec3(3,0,0));
        data->get_frame(1)->update_position(1,glm::vec3(3,2,0));
        e->get_component<gb::ces_animation_3d_mixer_component>()->remove_animation_ended_callback(e);
    });
    feeder->tick();
    for(size_t i=0;i<entities.size();++i){auto mixer=entities[i]->get_component<gb::ces_animation_3d_mixer_component>();
        for(int bone=0;bone<2;++bone)assert(mixer->get_transformations()[bone]==glm::translate(glm::mat4(1),glm::vec3(i==0?.5f:i==1?1.5f:2.25f,0,0)));}
    // Multiple structural changes keep last-append query ordering next tick.
    auto mixer0=entities[0]->get_component<gb::ces_animation_3d_mixer_component>();
    entities[0]->remove_component(mixer0);entities[0]->add_component(mixer0);
    auto mixer1=entities[1]->get_component<gb::ces_animation_3d_mixer_component>();
    entities[1]->remove_component(mixer1);entities[1]->add_component(mixer1);
    entities[0]->remove_component(mixer0);entities[0]->add_component(mixer0);
    assert(query->found==entities);feeder->tick();
    assert(query->found==std::vector<gb::ces_entity_shared_ptr>({entities[2],entities[1],entities[0]}));
    std::vector<gb::ces_entity_shared_ptr> bulk;
    for(int i=0;i<2100;++i){
        auto e=gb::ces_entity::construct<gb::ces_entity>();e->add_component<gb::ces_animation_3d_mixer_component>();
        scene->add_child(e);bulk.push_back(e);
    }
    feeder->tick();assert(query->found.size()==2103);
    for(const auto& e:bulk)scene->remove_child(e);
    assert(query->found.size()==2103);feeder->tick();assert(query->found.size()==3);
    std::cout<<"ECS core checks passed: exact poses, query order/next-tick, borrowed roots, mask duplicates, transform versions\n";
}
