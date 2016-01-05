//
//  omni_light.h
//  gbCore
//
//  Created by sergey.sergeev on 9/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef omni_light_h
#define omni_light_h

#include "renderable_game_object.h"

namespace gb
{
    class omni_light : public renderable_game_object
    {
    private:
        
        std::weak_ptr<std::vector<glm::vec4>> m_transformations;
        std::weak_ptr<std::vector<glm::vec4>> m_colors;
        i32 m_instance_id;
        
    protected:
        
    public:
        
        omni_light();
        ~omni_light();
        
        void set_instance_id(i32 instance_id);
        void set_external_uniforms_data(const std::shared_ptr<std::vector<glm::vec4>>& transformations,
                                        const std::shared_ptr<std::vector<glm::vec4>>& colors);
        
        void set_position(const glm::vec3& position);
        glm::vec3 get_position() const;
        
        void set_radius(f32 radius);
        f32 get_radius() const;
        
        void set_color(const glm::vec4& color);
        glm::vec4 get_color() const;
        
        void add_material(const std::string& technique_name, i32 technique_pass, const material_shared_ptr& material);
        
        void set_mesh(const instanced_mesh_shared_ptr& mesh);
    };
};


#endif
