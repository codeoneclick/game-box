//
//  direction_light.h
//  gbCore
//
//  Created by sergey.sergeev on 9/22/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef direction_light_h
#define direction_light_h

#include "renderable_game_object.h"

namespace gb
{
    class direction_light : public renderable_game_object
    {
    private:
        
        glm::vec3 m_direction;
        glm::vec4 m_color;
        f32 m_intensity;
        
    protected:
        
    public:
        
        direction_light();
        ~direction_light();
        
        void set_direction(const glm::vec3& direction);
        glm::vec3 get_direction() const;
        
        void set_color(const glm::vec4& color);
        glm::vec4 get_color() const;
        
        void set_intensity(f32 intensity);
        f32 get_intensity() const;
        
        void add_material(const std::string& technique_name, i32 technique_pass, const material_shared_ptr& material);
        
        void set_mesh(const mesh_shared_ptr& mesh);
    };
};

#endif
