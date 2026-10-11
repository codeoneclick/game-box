#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <array>
#include <string>
#include <vector>
#include <utility>

namespace gb
{
    // Affine T*R*S without two general 4x4 matrix products. Used both when
    // baking retargeted frames and when sampling the runtime mixer.
    inline glm::mat4 compose_animation_transform(const glm::vec3& position,const glm::quat& rotation,const glm::vec3& scale)
    {
        auto result=glm::mat4_cast(rotation);
        result[0]*=scale.x;result[1]*=scale.y;result[2]*=scale.z;
        result[3]=glm::vec4(position,1.f);
        return result;
    }
    // Transforms use the same model space as FBX animation frames, in metres.
    struct animation_3d_rig_bone
    {
        std::string name;
        int parent = -1;
        glm::mat4 rest = glm::mat4(1.f);
        std::string path;
        animation_3d_rig_bone() = default;
        animation_3d_rig_bone(const std::string& bone_name,int parent_index,const glm::mat4& reference,
                              const std::string& bone_path=""):
            name(bone_name),parent(parent_index),rest(reference),path(bone_path) {}
    };
    struct animation_3d_rig { std::vector<animation_3d_rig_bone> bones; };
    enum class humanoid_bone
    {
        hips, spine, neck, head,
        left_arm, left_forearm, left_hand, right_arm, right_forearm, right_hand,
        left_thigh, left_shin, left_foot, right_thigh, right_shin, right_foot, count
    };
    struct humanoid_profile
    {
        std::array<int, static_cast<size_t>(humanoid_bone::count)> indices;
        humanoid_profile() { indices.fill(-1); }
    };
    // Known Mixamo names and common Blender limb names; callers may supply
    // explicit profiles for other naming conventions.
    humanoid_profile make_humanoid_profile(const animation_3d_rig& rig);
    class animation_3d_retargeter
    {
        animation_3d_rig m_source, m_target;
        humanoid_profile m_source_profile, m_target_profile;
        std::vector<int> m_roles, m_order;
        std::vector<glm::quat> m_source_rest, m_target_rest, m_calibrated_rest;
        std::vector<glm::vec3> m_target_scale;
        std::vector<std::pair<int,glm::vec3>> m_sole_markers;
        glm::quat m_body_alignment;
        float m_hip_scale = 1.f;
    public:
        animation_3d_retargeter(const animation_3d_rig& source, const animation_3d_rig& target,
                               const humanoid_profile& source_profile, const humanoid_profile& target_profile,
                               const std::vector<std::pair<int,glm::vec3>>& sole_markers = {});
        // Bakes an in-place pose into the target bone order. Never mutates input.
        std::vector<glm::mat4> retarget(const std::vector<glm::mat4>& source_pose) const;
    };
}
