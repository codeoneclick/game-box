//
//  instanced_models3d_static.h
//  gbCore
//
//  Created by sergey.sergeev on 10/6/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef instanced_models3d_static_h
#define instanced_models3d_static_h

#include "renderable_game_object.h"

namespace gb
{
    class instanced_models3d_static : public renderable_game_object
    {
    private:
        
    protected:
        
        std::vector<glm::mat4> m_transform_parameters;
        
        std::vector<glm::mat4> m_matrices_t;
        std::vector<glm::mat4> m_matrices_r;
        std::vector<glm::mat4> m_matrices_s;
        
        std::vector<glm::vec3> m_positions;
        std::vector<glm::vec3> m_rotations;
        std::vector<glm::vec3> m_scales;
        
        i32 m_num_instances;
        
    public:
        
        instanced_models3d_static(i32 num_instances);
        ~instanced_models3d_static();
        
        void add_material(const std::string& technique_name, i32 technique_pass, const material_shared_ptr& material);
        
        void set_mesh(const mesh_shared_ptr& mesh);
        
        void set_position(const glm::vec3& position, i32 instance_id);
        glm::vec3 get_position(i32 instance_id) const;
        
        void set_rotation(const glm::vec3& rotation, i32 instance_id);
        glm::vec3 get_rotation(i32 instance_id) const;
        
        void set_scale(const glm::vec3& scale, i32 instance_id);
        glm::vec3 get_scale(i32 instance_id) const;
    };
};


#endif 
