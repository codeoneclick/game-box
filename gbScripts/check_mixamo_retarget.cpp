#include "animation_3d_retargeter.h"
#include "bone_3d.h"
#include <ufbx.h>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <functional>
#include <memory>
#include <map>
#include <glm/gtc/matrix_transform.hpp>

using scene_ptr = std::unique_ptr<ufbx_scene, decltype(&ufbx_free_scene)>;
static glm::mat4 matrix(const ufbx_matrix& m) {
    glm::mat4 r(1.f);
    for(int c=0;c<4;++c)for(int row=0;row<3;++row)r[c][row]=float(m.cols[c].v[row]);
    return r;
}
static scene_ptr load(const std::string& file) {
    ufbx_load_opts o={};o.target_axes={UFBX_COORDINATE_AXIS_POSITIVE_X,UFBX_COORDINATE_AXIS_POSITIVE_Y,UFBX_COORDINATE_AXIS_POSITIVE_Z};
    o.target_unit_meters=1;o.space_conversion=UFBX_SPACE_CONVERSION_ADJUST_TRANSFORMS;
    o.geometry_transform_handling=UFBX_GEOMETRY_TRANSFORM_HANDLING_MODIFY_GEOMETRY;
    ufbx_error e={};scene_ptr s(ufbx_load_file(file.c_str(),&o,&e),ufbx_free_scene);
    if(!s)throw std::runtime_error(file+": "+e.description.data);return s;
}
static gb::animation_3d_rig rig(const ufbx_scene* s,std::vector<const ufbx_node*>& nodes) {
    std::function<void(const ufbx_node*)> add=[&](const ufbx_node* n){
        if(!n||n==s->root_node||std::find(nodes.begin(),nodes.end(),n)!=nodes.end())return;
        add(n->parent);nodes.push_back(n);
    };
    for(auto n:s->nodes)if(n->bone)add(n);
    // Match the serializer: skinned clips use cluster bind transforms rather
    // than potentially animated default node transforms as the reference.
    std::map<const ufbx_node*,ufbx_matrix> reference;
    for(auto mesh:s->meshes)for(auto skin:mesh->skin_deformers)for(auto cluster:skin->clusters)
        reference.emplace(cluster->bone_node,cluster->bind_to_world);
    gb::animation_3d_rig r;
    for(auto n:nodes){auto p=std::find(nodes.begin(),nodes.end(),n->parent);
        auto bind=reference.find(n);
        r.bones.push_back({std::string(n->name.data,n->name.length),p==nodes.end()?-1:int(p-nodes.begin()),matrix(bind==reference.end()?n->node_to_world:bind->second)});
    }return r;
}
static float distance(const glm::mat4& a,const glm::mat4& b){return glm::length(glm::vec3(a[3]-b[3]));}
int main(int argc,char**argv) {
    assert(argc==2||argc==3);const std::string root=argv[1];
    const std::string target_file=argc==3?argv[2]:"woodsman.fbx";
    gb::bone_3d bone(0,-1,"probe");glm::mat4 value(1);bone.set_transformation(&value);
    for(int bind=0;bind<3;++bind){
        const auto rest=gb::compose_animation_transform(glm::vec3(bind,1,2),glm::normalize(glm::quat(1.f,.2f,.3f,.4f)),glm::vec3(1.1f,1.2f,1.3f));
        value=rest;bone.bind_pose_transformation();
        for(int i=0;i<100;++i){
            const auto pose=gb::compose_animation_transform(glm::vec3(i*.01f,2,3),glm::normalize(glm::quat(1.f,.3f,.2f,.1f)),glm::vec3(.9f,1.1f,1.2f));
            value=pose;bone.update();
            const auto expected=pose*glm::inverse(rest),base=pose*rest,actual_base=bone.get_base_transformation();
            for(int c=0;c<4;++c)for(int r=0;r<4;++r){assert(std::abs(value[c][r]-expected[c][r])<.0001f);assert(std::abs(actual_base[c][r]-base[c][r])<.0001f);}
        }
    }
    for(int i=0;i<1000;++i){
        const float t=i*.013f;
        const glm::vec3 p(std::sin(t)*20,std::cos(t)*3,t),s(std::sin(t)*2,.3f+std::cos(t)*.2f,1.5f);
        const auto q=glm::normalize(glm::quat(std::cos(t),std::sin(t),.3f,std::cos(t*.7f)));
        const auto expected=glm::translate(glm::mat4(1),p)*glm::mat4_cast(q)*glm::scale(glm::mat4(1),s);
        const auto actual=gb::compose_animation_transform(p,q,s);
        for(int c=0;c<4;++c)for(int r=0;r<4;++r)assert(std::abs(actual[c][r]-expected[c][r])<.00001f);
    }
    auto target_scene=load(root+"/gbBundle/Meshes/"+target_file);
    std::vector<const ufbx_node*> target_nodes;auto target=rig(target_scene.get(),target_nodes);
    auto target_profile=gb::make_humanoid_profile(target);
    std::vector<std::pair<int,glm::vec3>> soles;
    for(auto mesh:target_scene->meshes)for(auto skin:mesh->skin_deformers)for(auto cluster:skin->clusters) {
        auto bone=std::find(target_nodes.begin(),target_nodes.end(),cluster->bone_node);
        if(bone==target_nodes.end())continue;
        int index=int(bone-target_nodes.begin());
        if(index!=target_profile.indices[12]&&index!=target_profile.indices[15])continue;
        for(size_t v=0;v<cluster->vertices.count;++v)if(cluster->weights[v]>.999) {
            auto p=mesh->vertices[cluster->vertices[v]];
            auto world=matrix(mesh->instances[0]->geometry_to_world)*glm::vec4(float(p.x),float(p.y),float(p.z),1);
            soles.emplace_back(index,glm::vec3(world));
        }
    }
    assert(!soles.empty());
    for(const auto file:{"character_idle.fbx","character_walking.fbx"}) {
        auto source_scene=load(root+"/gbResources/"+file);
        std::vector<const ufbx_node*> source_nodes;auto source=rig(source_scene.get(),source_nodes);
        const auto source_profile=gb::make_humanoid_profile(source);
        glm::vec3 source_forward(0);
        for(const auto name:{"LeftToeBase","RightToeBase"})for(size_t i=0;i<source.bones.size();++i)
            if(source.bones[i].name.find(name)!=std::string::npos)
                source_forward+=glm::vec3(source.bones[i].rest[3]-source.bones[source.bones[i].parent].rest[3]);
        source_forward.y=0;source_forward=glm::normalize(source_forward);
        std::cout<<file<<" source forward="<<source_forward.x<<","<<source_forward.z
            <<" source left hip X="<<source.bones[source_profile.indices[10]].rest[3].x
            <<" target left hip X="<<target.bones[target_profile.indices[10]].rest[3].x<<std::endl;
        assert(source.bones.size()==65 && target.bones.size()==(target_file=="woodsman.fbx"?19:18));
        gb::animation_3d_retargeter transfer(source,target,gb::make_humanoid_profile(source),target_profile,soles);
        // A-pose targets must recover the same T-pose arm directions.
        auto a_pose=target;
        for(int role:{4,7}) {
            const int arm=target_profile.indices[role];
            const auto pivot=glm::vec3(target.bones[arm].rest[3]);
            const auto change=glm::translate(glm::mat4(1),pivot)*
                glm::rotate(glm::mat4(1),role==4?-60.f:60.f,glm::vec3(0,0,1))*
                glm::translate(glm::mat4(1),-pivot);
            for(size_t b=0;b<target.bones.size();++b) {
                int parent=int(b);while(parent>=0&&parent!=arm)parent=target.bones[parent].parent;
                if(parent==arm)a_pose.bones[b].rest=change*target.bones[b].rest;
            }
        }
        std::vector<glm::mat4> reference;for(const auto& b:source.bones)reference.push_back(b.rest);
        gb::animation_3d_retargeter calibrated(source,a_pose,gb::make_humanoid_profile(source),target_profile);
        auto t_rest=transfer.retarget(reference),a_rest=calibrated.retarget(reference);
        // Keep an explicit opposite-facing case even when active assets face
        // the same way: horizontal alignment must never flip the Y axis.
        auto opposite=target;glm::mat4 yaw(1.f);yaw[0][0]=-1; yaw[2][2]=-1;
        for(auto& b:opposite.bones)b.rest=yaw*b.rest;
        gb::animation_3d_retargeter opposite_transfer(source,opposite,source_profile,target_profile);
        const auto opposite_pose=opposite_transfer.retarget(reference);
        for(int role:{10,13}) {
            assert(opposite_pose[target_profile.indices[role]][3].y>opposite_pose[target_profile.indices[role+1]][3].y);
            assert(opposite_pose[target_profile.indices[role+1]][3].y>opposite_pose[target_profile.indices[role+2]][3].y);
        }
        // Ankle-to-toe proportions must not pitch a flat neutral boot.
        for(int role:{12,15}) {
            const int foot=target_profile.indices[role];
            for(int c=0;c<3;++c)assert(glm::dot(glm::normalize(glm::vec3(t_rest[foot][c])),glm::normalize(glm::vec3(target.bones[foot].rest[c])))>.9999f);
        }
        // Facing can differ by 180 degrees, but Y-up must remain Y-up.
        for(int role:{10,13}) {
            assert(t_rest[target_profile.indices[role]][3].y>t_rest[target_profile.indices[role+1]][3].y);
            assert(t_rest[target_profile.indices[role+1]][3].y>t_rest[target_profile.indices[role+2]][3].y);
        }
        for(int role:{4,5,7,8}) {
            int a=target_profile.indices[role],b=target_profile.indices[role+1];
            auto t=glm::normalize(glm::vec3(t_rest[b][3]-t_rest[a][3]));
            auto p=glm::normalize(glm::vec3(a_rest[b][3]-a_rest[a][3]));
            assert(glm::dot(t,p)>.9999f);
        }
        auto stack=source_scene->anim_stacks[0];std::vector<glm::mat4> first,last;
        std::map<int,int> bend_counts;
        for(int frame=0;frame<31;++frame) {
            double time=stack->time_begin+(stack->time_end-stack->time_begin)*frame/30.0;
            ufbx_error e={};scene_ptr pose(ufbx_evaluate_scene(source_scene.get(),stack->anim,time,nullptr,&e),ufbx_free_scene);assert(pose);
            std::vector<glm::mat4> input;for(auto n:source_nodes)input.push_back(matrix(pose->nodes[n->typed_id]->node_to_world));
            const auto input_copy=input;auto output=transfer.retarget(input);
            assert(output.size()==target.bones.size());assert(input==input_copy);
            if(target_file=="woodsman.fbx") {
                const auto sf=glm::normalize(glm::vec3(input[source_profile.indices[0]]*glm::inverse(source.bones[source_profile.indices[0]].rest)*glm::vec4(source_forward,0)));
                const auto tf=glm::normalize(glm::vec3(output[target_profile.indices[0]]*glm::inverse(target.bones[target_profile.indices[0]].rest)*glm::vec4(0,0,1,0)));
                for(int role:{4,7,10,13}) {
                    auto pole=[](const std::vector<glm::mat4>& pose,const gb::humanoid_profile& profile,int r) {
                        const auto a=glm::vec3(pose[profile.indices[r]][3]),b=glm::vec3(pose[profile.indices[r+1]][3]),c=glm::vec3(pose[profile.indices[r+2]][3]);
                        const auto axis=glm::normalize(c-a);return b-a-axis*glm::dot(b-a,axis);
                    };
                    const float sb=glm::dot(pole(input,source_profile,role),sf),tb=glm::dot(pole(output,target_profile,role),tf);
                    if(std::abs(sb)>.015f) {
                        ++bend_counts[role];
                        if(sb*tb<=0)std::cerr<<"Inverted joint: "<<file<<" frame="<<frame<<" role="<<role<<" source bend="<<sb<<" target bend="<<tb<<std::endl;
                        assert(sb*tb>0);
                    }
                }
            }
            float sole_y=1.e9f;
            for(const auto& marker:soles) {
                auto p=output[marker.first]*glm::inverse(target.bones[marker.first].rest)*glm::vec4(marker.second,1);
                sole_y=std::min(sole_y,p.y);assert(p.y>=-.00001f);
            }
            assert(std::abs(sole_y)<.00001f);
            // Grounding must protect the complete skinned boot, not just sole markers.
            if(target_file=="woodsman.fbx")for(auto mesh:target_scene->meshes)for(auto skin:mesh->skin_deformers) {
                for(size_t v=0;v<mesh->vertices.count;++v) {
                    auto p=mesh->vertices[v];auto world=matrix(mesh->instances[0]->geometry_to_world)*glm::vec4(float(p.x),float(p.y),float(p.z),1);
                    glm::vec4 skinned(0);auto weights=skin->vertices[v];
                    for(size_t w=0;w<weights.num_weights;++w) {
                        auto weight=skin->weights[weights.weight_begin+w];auto cluster=skin->clusters[weight.cluster_index];
                        auto node=std::find(target_nodes.begin(),target_nodes.end(),cluster->bone_node);assert(node!=target_nodes.end());
                        const int b=int(node-target_nodes.begin());
                        skinned+=float(weight.weight)*(output[b]*glm::inverse(target.bones[b].rest)*world);
                    }
                    if(skinned.y<-.003f)std::cerr<<file<<" frame="<<frame<<" vertex="<<v<<" bind="<<world.x<<","<<world.y<<","<<world.z<<" skinned_y="<<skinned.y<<std::endl;
                    assert(skinned.y>=-.003f);
                }
            }
            for(size_t i=0;i<output.size();++i){
                for(int c=0;c<4;++c)for(int row=0;row<4;++row)assert(std::isfinite(output[i][c][row]));
                for(int c=0;c<3;++c)assert(std::abs(glm::length(glm::vec3(output[i][c]))-glm::length(glm::vec3(target.bones[i].rest[c])))<.001f);
                int parent=target.bones[i].parent;
                if(parent>=0&&int(i)!=target_profile.indices[0])assert(std::abs(distance(output[i],output[parent])-distance(target.bones[i].rest,target.bones[parent].rest))<.001f);
            }
            const int hips=target_profile.indices[0];assert(std::abs(output[hips][3].x-target.bones[hips].rest[3].x)<.0001f);
            assert(std::abs(output[hips][3].z-target.bones[hips].rest[3].z)<.0001f);
            if(first.empty())first=output;last=output;
        }
        if(target_file=="woodsman.fbx") {
            std::cout<<"Matching elbow/knee bend signs (L arm/R arm/L leg/R leg): ";
            for(int role:{4,7,10,13}) {
                std::cout<<bend_counts[role]<<" ";
                if(std::string(file)=="character_walking.fbx")assert(bend_counts[role]>0);
            }
            std::cout<<std::endl;
        }
        // Check a different phase, rather than the loop's repeated endpoint.
        ufbx_error e={};scene_ptr mid(ufbx_evaluate_scene(source_scene.get(),stack->anim,stack->time_begin+.25,nullptr,&e),ufbx_free_scene);
        std::vector<glm::mat4> input;for(auto n:source_nodes)input.push_back(matrix(mid->nodes[n->typed_id]->node_to_world));
        auto output=transfer.retarget(input);float change=0;
        for(size_t i=0;i<output.size();++i)for(int c=0;c<3;++c)change+=glm::length(output[i][c]-first[i][c]);
        assert(change>.01f);
        bool rejected=false;try{transfer.retarget({});}catch(const std::runtime_error&){rejected=true;}assert(rejected);
        auto bad=source;bad.bones[source.bones.size()-1].parent=int(source.bones.size()-1);
        rejected=false;try{gb::animation_3d_retargeter invalid(bad,target,gb::make_humanoid_profile(source),target_profile);}catch(const std::runtime_error&){rejected=true;}assert(rejected);
        std::cout<<file<<": "<<source.bones.size()<<" -> "<<target.bones.size()<<" bones; finite poses, target lengths/scales, in-place motion, moving clips and rejection checks passed\n";
    }
}
