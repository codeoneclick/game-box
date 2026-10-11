#include "animation_3d_retargeter.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>
#include <functional>
#include <stdexcept>

namespace gb
{
    namespace
    {
        glm::vec3 position(const glm::mat4& m) { return glm::vec3(m[3]); }
        glm::vec3 scale(const glm::mat4& m)
        {
            glm::vec3 s;
            for(int c=0;c<4;++c)for(int r=0;r<4;++r)
                if(!std::isfinite(m[c][r]))throw std::runtime_error("Retarget: nonfinite bone transform");
            for(int c=0;c<3;++c) { s[c]=glm::length(glm::vec3(m[c]));
                if(s[c]<1.e-6f)throw std::runtime_error("Retarget: degenerate bone transform"); }
            return s;
        }
        glm::quat rotation(const glm::mat4& m)
        {
            const auto s=scale(m);glm::mat3 basis;
            for(int c=0;c<3;++c)basis[c]=glm::vec3(m[c])/s[c];
            if(glm::determinant(basis)<.99f || std::abs(glm::dot(basis[0],basis[1]))>.001f ||
               std::abs(glm::dot(basis[0],basis[2]))>.001f || std::abs(glm::dot(basis[1],basis[2]))>.001f)
                throw std::runtime_error("Retarget: mirrored/sheared bone transform unsupported");
            return glm::normalize(glm::quat_cast(basis));
        }
        std::vector<int> hierarchy(const animation_3d_rig& rig)
        {
            if(rig.bones.empty() || rig.bones.size()>1024)throw std::runtime_error("Retarget: invalid rig size");
            std::vector<int> state(rig.bones.size()), order;
            std::function<void(int)> visit=[&](int i) {
                if(state[i]==2)return;
                if(state[i]==1)throw std::runtime_error("Retarget: cyclic skeleton");
                state[i]=1;int parent=rig.bones[i].parent;
                if(parent < -1 || parent>=int(rig.bones.size()))throw std::runtime_error("Retarget: invalid parent");
                if(parent>=0)visit(parent);
                rotation(rig.bones[i].rest);state[i]=2;order.push_back(i);
            };
            for(size_t i=0;i<rig.bones.size();++i)visit(int(i));return order;
        }
        std::string basename(const std::string& name)
        {
            const auto p=name.find_last_of(":/|");return p==std::string::npos?name:name.substr(p+1);
        }
        int find(const animation_3d_rig& rig,const std::string& name)
        {
            int found=-1;
            for(size_t i=0;i<rig.bones.size();++i)if(basename(rig.bones[i].name)==name) {
                if(found>=0)throw std::runtime_error("Retarget: ambiguous bone name "+name);found=int(i);
            }return found;
        }
        void validate_profile(const animation_3d_rig& rig,const humanoid_profile& profile)
        {
            std::vector<bool> used(rig.bones.size());
            for(auto i:profile.indices) {
                if(i<0 || i>=int(rig.bones.size()))throw std::runtime_error("Retarget: incomplete humanoid profile");
                if(used[i])throw std::runtime_error("Retarget: duplicate humanoid mapping");used[i]=true;
            }
        }
        glm::quat align(const glm::vec3& from,const glm::vec3& to)
        {
            if(glm::length(from)<1.e-5f || glm::length(to)<1.e-5f)throw std::runtime_error("Retarget: zero-length limb");
            auto a=glm::normalize(from),b=glm::normalize(to);float d=glm::clamp(glm::dot(a,b),-1.f,1.f);
            if(d<-.9999f) {
                auto axis=glm::cross(a,std::abs(a.x)<.8f?glm::vec3(1,0,0):glm::vec3(0,1,0));
                axis=glm::normalize(axis);return glm::quat(0.f,axis.x,axis.y,axis.z);
            }
            auto v=glm::cross(a,b);return glm::normalize(glm::quat(1.f+d,v.x,v.y,v.z));
        }
    }
    humanoid_profile make_humanoid_profile(const animation_3d_rig& rig)
    {
        const std::vector<std::vector<std::string>> aliases={
            {"Hips","pelvis","hips"},{"Spine2","Chest","chest","spine","Spine"},{"Neck","neck"},{"Head","head"},
            {"LeftArm","upper_arm.L","upper_arm_l"},{"LeftForeArm","forearm.L","lower_arm_l"},{"LeftHand","hand.L","hand_l"},
            {"RightArm","upper_arm.R","upper_arm_r"},{"RightForeArm","forearm.R","lower_arm_r"},{"RightHand","hand.R","hand_r"},
            {"LeftUpLeg","thigh.L","upper_leg_l"},{"LeftLeg","shin.L","lower_leg_l"},{"LeftFoot","foot.L","foot_l"},
            {"RightUpLeg","thigh.R","upper_leg_r"},{"RightLeg","shin.R","lower_leg_r"},{"RightFoot","foot.R","foot_r"}};
        humanoid_profile p;
        for(size_t role=0;role<aliases.size();++role)for(const auto& name:aliases[role]) {
            int i=find(rig,name);if(i>=0){p.indices[role]=i;break;}
        }return p;
    }
    animation_3d_retargeter::animation_3d_retargeter(const animation_3d_rig& source,const animation_3d_rig& target,
        const humanoid_profile& source_profile,const humanoid_profile& target_profile,
        const std::vector<std::pair<int,glm::vec3>>& sole_markers):
        m_source(source),m_target(target),m_source_profile(source_profile),m_target_profile(target_profile)
    {
        hierarchy(source);m_order=hierarchy(target);validate_profile(source,source_profile);validate_profile(target,target_profile);
        m_roles.assign(target.bones.size(),-1);
        for(size_t r=0;r<target_profile.indices.size();++r)m_roles[target_profile.indices[r]]=int(r);
        for(const auto& b:source.bones)m_source_rest.push_back(rotation(b.rest));
        for(const auto& b:target.bones){m_target_rest.push_back(rotation(b.rest));m_target_scale.push_back(scale(b.rest));}
        auto left_axis=[](const animation_3d_rig& rig,const humanoid_profile& p) {
            auto v=position(rig.bones[p.indices[10]].rest)-position(rig.bones[p.indices[13]].rest);v.y=0;return v;
        };
        // Both rigs are Y-up. Matching a horizontal body axis must only yaw:
        // unconstrained antiparallel alignment can turn 180 degrees around Z
        // and flip the legs upward when the rigs face opposite directions.
        auto source_axis=left_axis(source,source_profile),target_axis=left_axis(target,target_profile);
        if(glm::length(source_axis)<1.e-5f || glm::length(target_axis)<1.e-5f)
            throw std::runtime_error("Retarget: degenerate horizontal hip axis");
        source_axis=glm::normalize(source_axis);target_axis=glm::normalize(target_axis);
        const float yaw=std::atan2(glm::cross(source_axis,target_axis).y,glm::dot(source_axis,target_axis));
        m_body_alignment=glm::quat(std::cos(yaw*.5f),0.f,std::sin(yaw*.5f),0.f);
        const float source_height=position(source.bones[source_profile.indices[0]].rest).y;
        const float target_height=position(target.bones[target_profile.indices[0]].rest).y;
        if(source_height<=.01f || target_height<=.01f)throw std::runtime_error("Retarget: rig feet origin/hip height invalid");
        m_hip_scale=target_height/source_height;
        for(const auto& marker:sole_markers) {
            if(marker.first<0||marker.first>=int(target.bones.size()))throw std::runtime_error("Retarget: invalid sole marker bone");
            for(int c=0;c<3;++c)if(!std::isfinite(marker.second[c]))throw std::runtime_error("Retarget: invalid sole marker position");
            auto local=glm::inverse(target.bones[marker.first].rest)*glm::vec4(marker.second,1.f);
            m_sole_markers.emplace_back(marker.first,glm::vec3(local)*m_target_scale[marker.first]);
        }
        m_calibrated_rest=m_target_rest;
        // Calibrate limbs into the source reference pose (T-pose -> A-pose is
        // not a simple index remap). Keep the target torso's neutral posture.
        for(size_t role=4;role<target_profile.indices.size();++role) {
            // Ankle-to-toe slope depends on foot proportions, not neutral sole
            // pitch. Preserve the target boot orientation and apply only the
            // source rotation delta below, as for the neutral torso.
            if(role==12||role==15)continue;
            int ti=target_profile.indices[role],si=source_profile.indices[role];
            glm::vec3 td=m_target_rest[ti]*glm::vec3(0,1,0),sd;
            if(role==4||role==5||role==7||role==8||role==10||role==11||role==13||role==14) {
                td=position(target.bones[target_profile.indices[role+1]].rest)-position(target.bones[ti].rest);
                sd=position(source.bones[source_profile.indices[role+1]].rest)-position(source.bones[si].rest);
            } else {
                const char* names[]={"LeftHandMiddle1","RightHandMiddle1","LeftToeBase","RightToeBase"};
                int helper=find(source,names[role==6?0:role==9?1:role==12?2:3]);
                sd=helper>=0?position(source.bones[helper].rest)-position(source.bones[si].rest):m_source_rest[si]*glm::vec3(0,1,0);
            }
            m_calibrated_rest[ti]=align(td,m_body_alignment*sd)*m_target_rest[ti];
        }
    }
    std::vector<glm::mat4> animation_3d_retargeter::retarget(const std::vector<glm::mat4>& input) const
    {
        if(input.size()!=m_source.bones.size())throw std::runtime_error("Retarget: source pose bone count mismatch");
        std::vector<glm::quat> source_rotation;for(const auto& m:input)source_rotation.push_back(rotation(m));
        std::vector<glm::quat> q(m_target.bones.size());std::vector<glm::vec3> p(m_target.bones.size());
        const int hips=m_target_profile.indices[0],source_hips=m_source_profile.indices[0];
        std::vector<bool> under_hips(m_target.bones.size());
        for(int i:m_order) {
            int parent=m_target.bones[i].parent,role=m_roles[i];
            under_hips[i]=i==hips || (parent>=0&&under_hips[parent]);
            if(role>=0) {
                int source=m_source_profile.indices[role];
                q[i]=glm::normalize(m_body_alignment*source_rotation[source]*glm::inverse(m_source_rest[source])*
                    glm::inverse(m_body_alignment)*m_calibrated_rest[i]);
            } else q[i]=parent>=0?glm::normalize(q[parent]*glm::inverse(m_target_rest[parent])*m_target_rest[i]):m_target_rest[i];
            if(i==hips) {
                p[i]=position(m_target.bones[i].rest);
                p[i].y+=(position(input[source_hips]).y-position(m_source.bones[source_hips].rest).y)*m_hip_scale;
            } else p[i]=parent>=0?p[parent]+q[parent]*glm::inverse(m_target_rest[parent])*
                (position(m_target.bones[i].rest)-position(m_target.bones[parent].rest)):position(m_target.bones[i].rest);
        }
        // Approximate sole markers anchor the lowest foot. This adjusts height,
        // not joint angles; full foot locking/IK is intentionally separate.
        float ground_shift=1.e9f;
        if(!m_sole_markers.empty())for(const auto& marker:m_sole_markers)
            ground_shift=std::min(ground_shift,(p[marker.first]+q[marker.first]*marker.second).y);
        else for(int role:{12,15}) {
            int i=m_target_profile.indices[role];auto sole=position(m_target.bones[i].rest);sole.y=0.f;
            const auto local=glm::inverse(m_target_rest[i])*(sole-position(m_target.bones[i].rest));
            ground_shift=std::min(ground_shift,(p[i]+q[i]*local).y);
        }
        std::vector<glm::mat4> result(m_target.bones.size());
        for(size_t i=0;i<result.size();++i) {
            if(under_hips[i])p[i].y-=ground_shift;
            result[i]=compose_animation_transform(p[i],q[i],m_target_scale[i]);
        }return result;
    }
}
