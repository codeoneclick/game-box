#include "ufbx.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>
using point=std::array<long long,3>;
using uv_corner=std::array<long long,5>;
point key(double x,double y,double z){return {{llround(x*100000),llround(y*100000),llround(z*100000)}};}
int main(int argc,char** argv){
    assert(argc==2);const std::string root=argv[1];
    ufbx_load_opts opts={};opts.target_axes={UFBX_COORDINATE_AXIS_POSITIVE_X,UFBX_COORDINATE_AXIS_POSITIVE_Y,UFBX_COORDINATE_AXIS_POSITIVE_Z};
    opts.target_unit_meters=1;opts.space_conversion=UFBX_SPACE_CONVERSION_MODIFY_GEOMETRY;
    opts.geometry_transform_handling=UFBX_GEOMETRY_TRANSFORM_HANDLING_MODIFY_GEOMETRY;
    ufbx_error error={};auto s=ufbx_load_file((root+"/gbBundle/Meshes/woodsman.fbx").c_str(),&opts,&error);assert(s);
    assert(s->anim_stacks.count==0 && s->meshes.count==1);
    std::map<std::string,std::string> parents;
    for(auto node:s->nodes)if(node->bone)parents[node->name.data]=node->parent->name.data;
    assert(parents.size()==18);
    const std::map<std::string,std::string> required={{"hips","root"},{"spine","hips"},{"chest","spine"},{"neck","chest"},{"head","neck"},
        {"upper_arm_l","chest"},{"lower_arm_l","upper_arm_l"},{"hand_l","lower_arm_l"},
        {"upper_arm_r","chest"},{"lower_arm_r","upper_arm_r"},{"hand_r","lower_arm_r"},
        {"upper_leg_l","hips"},{"lower_leg_l","upper_leg_l"},{"foot_l","lower_leg_l"},
        {"upper_leg_r","hips"},{"lower_leg_r","upper_leg_r"},{"foot_r","lower_leg_r"}};
    for(auto& p:required)assert(parents.at(p.first)==p.second);
    // In this +Z-facing model, anatomical left must lie on +X.
    std::map<std::string,double> bone_x;
    for(auto node:s->nodes)if(node->bone)bone_x[node->name.data]=node->node_to_world.m03;
    assert(bone_x.at("upper_leg_l")>bone_x.at("upper_leg_r"));
    assert(bone_x.at("upper_arm_l")>bone_x.at("upper_arm_r"));
    std::multiset<point> expected,actual;std::ifstream f(root+"/gbResources/woodsman/expected_vertices.txt");assert(f);
    double x,y,z;while(f>>x>>y>>z)expected.insert(key(x,y,z));
    auto mesh=s->meshes[0];assert(mesh->num_triangles==1560 && mesh->num_faces==780 && mesh->num_vertices==1040);
    assert(mesh->vertex_uv.exists && mesh->skin_deformers.count==1);
    double miny=1e9,maxy=-1e9,minz=1e9,maxz=-1e9;
    for(auto v:mesh->vertices){auto p=ufbx_transform_position(&mesh->instances[0]->geometry_to_world,v);
        actual.insert(key(p.x,p.y,p.z));miny=std::min(miny,p.y);maxy=std::max(maxy,p.y);minz=std::min(minz,p.z);maxz=std::max(maxz,p.z);}
    assert(actual==expected); // Every cube corner survives; no simplification/welding.
    std::multiset<uv_corner> expected_uv,actual_uv;
    std::ifstream uv_file(root+"/gbResources/woodsman/expected_uv_corners.txt");assert(uv_file);
    double u,v;
    while(uv_file>>x>>y>>z>>u>>v)expected_uv.insert({{llround(x*100000),llround(y*100000),llround(z*100000),llround(u*100000),llround(v*100000)}});
    for(size_t i=0;i<mesh->num_indices;++i) {
        auto p=ufbx_transform_position(&mesh->instances[0]->geometry_to_world,mesh->vertices[mesh->vertex_indices[i]]);
        auto uv=mesh->vertex_uv.values[mesh->vertex_uv.indices[i]];
        actual_uv.insert({{llround(p.x*100000),llround(p.y*100000),llround(p.z*100000),llround(uv.x*100000),llround(uv.y*100000)}});
    }
    assert(actual_uv==expected_uv && actual_uv.size()==3120);
    assert(std::abs(miny)<.00001 && std::abs(maxy-1.9)<.00001);
    assert(std::abs(minz+6.*1.9/47.)<.00001 && std::abs(maxz-7.*1.9/47.)<.00001);
    for(auto v:mesh->skin_deformers[0]->vertices){
        assert(v.num_weights>=1 && v.num_weights<=4);double sum=0;
        for(size_t i=0;i<v.num_weights;++i)sum+=mesh->skin_deformers[0]->weights[v.weight_begin+i].weight;
        assert(std::abs(sum-1)<.00001);
    }
    ufbx_free_scene(s);
    std::cout<<"Woodsman FBX: exact 1040 source corners and 3120 UV corners, 780 faces/1560 triangles, 18 named bones, normalized weights, no animations, Y-up/+Z front, 1.9m passed\n";
}
