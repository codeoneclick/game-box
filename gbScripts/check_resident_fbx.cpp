#include "ufbx.h"
#include "../gbDemo/sources/resident_asset_validation.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
std::string path(const ufbx_node* n,const ufbx_node* root){return !n||n==root?"":path(n->parent,root)+"/"+std::string(n->name.data,n->name.length);}
int main(int argc,char** argv){
    assert(argc==2);std::vector<std::string> reference;ufbx_scene* bind_scene=nullptr;
    for(const auto name:{"island_resident.fbx","island_resident_idle.fbx","island_resident_walk.fbx"}){
        ufbx_load_opts options={};options.generate_missing_normals=true;options.clean_skin_weights=true;
        options.target_axes={UFBX_COORDINATE_AXIS_POSITIVE_X,UFBX_COORDINATE_AXIS_POSITIVE_Y,UFBX_COORDINATE_AXIS_POSITIVE_Z};
        options.target_unit_meters=1;options.space_conversion=UFBX_SPACE_CONVERSION_MODIFY_GEOMETRY;
        options.geometry_transform_handling=UFBX_GEOMETRY_TRANSFORM_HANDLING_MODIFY_GEOMETRY;
        ufbx_error error={};auto filename=std::string(argv[1])+"/"+name;auto scene=ufbx_load_file(filename.c_str(),&options,&error);
        assert(scene);std::vector<std::string> bones;double miny=1e9,maxy=-1e9;size_t triangles=0;
        for(auto node:scene->nodes){if(node->bone)bones.push_back(path(node,scene->root_node));if(!node->mesh)continue;
            auto mesh=node->mesh;triangles+=mesh->num_triangles;assert(mesh->vertex_uv.exists&&mesh->skin_deformers.count>0);
            for(auto pos:mesh->vertices){auto p=ufbx_transform_position(&node->geometry_to_world,pos);miny=std::min(miny,p.y);maxy=std::max(maxy,p.y);}
            auto skin=mesh->skin_deformers[0];for(auto v:skin->vertices){assert(v.num_weights>=1&&v.num_weights<=4);double total=0;
                for(size_t i=0;i<v.num_weights;++i)total+=skin->weights[v.weight_begin+i].weight;assert(std::abs(total-1)<.001);}
        }
        std::sort(bones.begin(),bones.end());assert(bones.size()>0&&bones.size()<=32);
        if(reference.empty())reference=bones;else assert(reference==bones);
        assert(triangles>0&&triangles<=500);assert(std::abs((maxy-miny)-1.9)<.01&&std::abs(miny)<.03);
        if(std::string(name)!="island_resident.fbx"){
            assert(scene->anim_stacks.count==1);auto stack=scene->anim_stacks[0];
            double expected=std::string(name).find("idle")!=std::string::npos?3:.8;
            assert(std::abs(stack->time_end-stack->time_begin-expected)<.04);
            std::cout<<"Clip "<<stack->name.data<<" duration="<<stack->time_end-stack->time_begin<<std::endl;
            options.space_conversion=UFBX_SPACE_CONVERSION_ADJUST_TRANSFORMS;
            auto animated=ufbx_load_file(filename.c_str(),&options,&error);assert(animated);
            auto pose=ufbx_evaluate_scene(animated,animated->anim_stacks[0]->anim,.5,nullptr,&error);assert(pose);
            for(auto node:pose->nodes)if(node->bone){
                const ufbx_node* original=nullptr;for(auto candidate:bind_scene->nodes)if(path(candidate,bind_scene->root_node)==path(node,pose->root_node)){original=candidate;break;}assert(original);
                auto a=ufbx_matrix_to_transform(&node->node_to_world);auto b=ufbx_matrix_to_transform(&original->node_to_world);
                const double ratio=std::abs(a.scale.x/b.scale.x);std::cout<<"Bone scale ratio "<<node->name.data<<"="<<ratio<<std::endl;
                assert(ratio>.9&&ratio<1.1);
            }
            ufbx_free_scene(pose);ufbx_free_scene(animated);
        }
        std::cout<<name<<" triangles="<<triangles<<" bones="<<bones.size()<<" height="<<maxy-miny<<std::endl;
        if(!bind_scene)bind_scene=scene;else ufbx_free_scene(scene);
    }
    std::cout<<"Resident FBX import checks passed"<<std::endl;
    ufbx_free_scene(bind_scene);
    const std::string root=argv[1];
    game::validate_resident_fbx_files({root+"/island_resident.fbx",root+"/island_resident_idle.fbx",root+"/island_resident_walk.fbx"});
    bool rejected=false;try{game::validate_resident_fbx_files({root+"/missing-resident.fbx","",""});}catch(const std::runtime_error&){rejected=true;}
    assert(rejected);
}
