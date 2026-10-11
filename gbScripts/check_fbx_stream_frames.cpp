#include "animation_3d_sequence_serializer_fbx.h"
#include "animation_3d_sequence.h"
#include <ufbx.h>
#include <iostream>
#include <stdexcept>
#include <cmath>
std::string bundlepath(){return "";}
static void require(bool ok,const char* why){if(!ok)throw std::runtime_error(why);}
static std::string node_path(const ufbx_node* n,const ufbx_node* root){return !n||n==root?"":node_path(n->parent,root)+"/"+std::string(n->name.data,n->name.length);}
int main(int argc,char** argv){try{
 require(argc==2,"repo path required");
 for(const auto name:{"island_resident_idle.fbx","island_resident_walk.fbx"}){
  const auto filename=std::string(argv[1])+"/gbBundle/Meshes/"+name;
  auto output=std::make_shared<gb::animation_3d_sequence>(filename);
  gb::animation_3d_sequence_serializer_fbx serializer(filename,output);serializer.serialize(nullptr);
  require(serializer.get_status()==gb::e_serializer_status_success&&output->is_loaded(),"production FBX stream import failed");
  ufbx_load_opts opts={};opts.target_axes={UFBX_COORDINATE_AXIS_POSITIVE_X,UFBX_COORDINATE_AXIS_POSITIVE_Y,UFBX_COORDINATE_AXIS_POSITIVE_Z};
  opts.target_unit_meters=1;opts.space_conversion=UFBX_SPACE_CONVERSION_ADJUST_TRANSFORMS;opts.geometry_transform_handling=UFBX_GEOMETRY_TRANSFORM_HANDLING_MODIFY_GEOMETRY;
  ufbx_error error={};auto original=ufbx_load_file(filename.c_str(),&opts,&error);require(original!=nullptr,"reference import failed");
  const auto stack=original->anim_stacks[0];const auto rig=output->get_rig();require(rig!=nullptr,"rig missing");
  for(int frame_index=0;frame_index<output->get_num_frames();++frame_index){
   const auto time=std::min(stack->time_begin+double(frame_index)/output->get_animation_fps(),stack->time_end);
   auto pose=ufbx_evaluate_scene(original,stack->anim,time,nullptr,&error);require(pose!=nullptr,"reference evaluation failed");
   auto frame=output->get_frame(frame_index);require(frame->get_num_bones()==rig->bones.size(),"bone count changed");
   for(size_t bone=0;bone<rig->bones.size();++bone){const ufbx_node* reference=nullptr;
    for(const auto node:pose->nodes)if(node_path(node,pose->root_node)==rig->bones[bone].path){reference=node;break;}
    require(reference!=nullptr,"bone path changed");const auto transform=ufbx_matrix_to_transform(&reference->node_to_world);
    const auto p=frame->get_position(static_cast<ui32>(bone));const auto s=frame->get_scale(static_cast<ui32>(bone));const auto q=frame->get_rotation(static_cast<ui32>(bone));
    require(p.x==float(transform.translation.x)&&p.y==float(transform.translation.y)&&p.z==float(transform.translation.z),"sampled translation changed");
    require(s.x==float(transform.scale.x)&&s.y==float(transform.scale.y)&&s.z==float(transform.scale.z),"sampled scale changed");
    require(q.x==float(transform.rotation.x)&&q.y==float(transform.rotation.y)&&q.z==float(transform.rotation.z)&&q.w==float(transform.rotation.w),"sampled rotation changed");
   }ufbx_free_scene(pose);
  }ufbx_free_scene(original);std::cout<<name<<" exact frames="<<output->get_num_frames()<<"\n";
 }
 std::cout<<"PASS production streaming FBX import, all sampled bone poses match direct ufbx exactly\n";return 0;
 }catch(const std::exception& e){std::cerr<<"FAIL "<<e.what()<<"\n";return 1;}}
