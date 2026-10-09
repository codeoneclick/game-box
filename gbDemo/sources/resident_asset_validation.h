#pragma once
#include "../../gbCore/libraries/ufbx/ufbx.h"
#include <array>
#include <algorithm>
#include <cmath>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
namespace game {
// Validate before the legacy synchronous loader, whose failure path asserts.
inline std::string resident_bone_path(const ufbx_node* n,const ufbx_node* root){
    return !n||n==root?"":resident_bone_path(n->parent,root)+"/"+std::string(n->name.data,n->name.length);
}
inline void validate_resident_fbx_files(const std::array<std::string,3>& files){
    using scene_ptr=std::unique_ptr<ufbx_scene,decltype(&ufbx_free_scene)>;
    std::set<std::string> reference;
    for(size_t index=0;index<files.size();++index){
        ufbx_load_opts opts={};opts.target_axes={UFBX_COORDINATE_AXIS_POSITIVE_X,UFBX_COORDINATE_AXIS_POSITIVE_Y,UFBX_COORDINATE_AXIS_POSITIVE_Z};
        opts.target_unit_meters=1;opts.space_conversion=index?UFBX_SPACE_CONVERSION_ADJUST_TRANSFORMS:UFBX_SPACE_CONVERSION_MODIFY_GEOMETRY;
        opts.geometry_transform_handling=UFBX_GEOMETRY_TRANSFORM_HANDLING_MODIFY_GEOMETRY;
        ufbx_error error={};scene_ptr scene(ufbx_load_file(files[index].c_str(),&opts,&error),&ufbx_free_scene);
        if(!scene)throw std::runtime_error("Invalid resident FBX: "+files[index]);
        std::set<std::string> paths;
        for(auto node:scene->nodes)if(node->bone)for(auto parent=node;parent&&parent!=scene->root_node;parent=parent->parent)paths.insert(resident_bone_path(parent,scene->root_node));
        if(paths.empty()||paths.size()>64)throw std::runtime_error("Invalid resident skeleton");
        if(!index){
            reference=paths;bool skinned=false;
            for(auto mesh:scene->meshes)if(mesh->num_triangles&&mesh->vertex_uv.exists&&mesh->skin_deformers.count){
                skinned=true;for(auto vertex:mesh->skin_deformers[0]->vertices)if(!vertex.num_weights)throw std::runtime_error("Resident has unweighted vertices");
            }
            if(!skinned)throw std::runtime_error("Resident skinned mesh/UV missing");
        }else{
            if(paths!=reference||scene->anim_stacks.count!=1)throw std::runtime_error("Resident clip skeleton/stack mismatch");
            auto stack=scene->anim_stacks[0];double duration=stack->time_end-stack->time_begin;
            const double rate=scene->settings.frames_per_second;
            if(!std::isfinite(duration)||duration<0||duration>10||!std::isfinite(rate)||rate<1||rate>240)throw std::runtime_error("Resident clip timing invalid");
            int fps=std::max(1,static_cast<int>(std::round(rate)));
            int frames=static_cast<int>(std::floor(duration*fps+.5))+1;
            if(frames<(index==1?90:24))throw std::runtime_error("Resident clip too short");
            for(int frame=0;frame<frames;++frame){
                scene_ptr pose(ufbx_evaluate_scene(scene.get(),stack->anim,std::min(stack->time_begin+double(frame)/fps,stack->time_end),nullptr,&error),&ufbx_free_scene);
                if(!pose)throw std::runtime_error("Resident clip evaluation failed");
            }
        }
    }
}
}
