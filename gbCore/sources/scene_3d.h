//
//  scene_3d.hpp
//  gbCore
//
//  Created by serhii.s on 12/17/18.
//  Copyright © 2018 sergey.sergeev. All rights reserved.
//

#pragma once

#include "resource.h"

namespace gb
{
    class scene_object_3d_data final
    {
    private:
        
    protected:
        
        std::string m_name;
        std::string m_parent_name;
        
        glm::vec3 m_position;
        glm::vec3 m_rotation;
        glm::vec3 m_scale;
        
    public:
        
        scene_object_3d_data(const std::string& name,
                             const std::string& parent_name,
                             const glm::vec3& position,
                             const glm::vec3& rotation,
                             const glm::vec3& scale);
        
        ~scene_object_3d_data();
        
        const std::string& get_name() const;
        const std::string& get_parent_name() const;
        
        const glm::vec3& get_rotation() const;
        const glm::vec3& get_position() const;
        const glm::vec3& get_scale() const;
    };
    
    class scene_3d_transfering_data final : public resource_transfering_data
    {
    private:
        
    protected:
        
        std::vector<scene_object_3d_data_shared_ptr> m_scene_objects;
        
    public:
        
        scene_3d_transfering_data(const std::vector<scene_object_3d_data_shared_ptr>& scene_objects);
        ~scene_3d_transfering_data();
        
        i32 get_num_objects() const;
        scene_object_3d_data_shared_ptr get_scene_object(i32 index) const;
    };
    
    class scene_3d : public resource
    {
    private:
        
    protected:
        
        scene_3d_transfering_data_shared_ptr m_scene_3d_data;
        
        void on_transfering_data_serialized(const std::shared_ptr<resource_transfering_data>& data);
        void on_transfering_data_commited(const std::shared_ptr<resource_transfering_data>& data);
        
    public:
        
        scene_3d(const std::string& guid);
        
        static scene_3d_shared_ptr construct(const std::string& guid,
                                             const scene_3d_transfering_data_shared_ptr& data);
        ~scene_3d();
        
        i32 get_num_objects() const;
        scene_object_3d_data_shared_ptr get_scene_object(i32 index) const;
    };
};

