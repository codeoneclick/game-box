//
//  collision_manager.h
//  gbCore
//
//  Created by sergey.sergeev on 8/31/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef collision_manager_h
#define collision_manager_h

#include "main_headers.h"
#include "declarations.h"
#include "glm_extensions.h"

namespace gb
{
    class collision_manager : public b2ContactListener
    {
    private:
        
        static bool triangle_intersection(const glm::vec3& point_01,
                                          const glm::vec3& point_02,
                                          const glm::vec3& point_03,
                                          const glm::ray& ray,
                                          glm::vec3* intersected_point);
        
    protected:
        
        std::shared_ptr<b2World> m_box2d_world;
        
        void BeginContact(b2Contact* contact);
        void EndContact(b2Contact* contact);
        
    public:
        
        collision_manager();
        ~collision_manager();
        
        static void unproject(const glm::ivec2& point,
                              const glm::mat4x4& mat_v, const glm::mat4x4& mat_p,
                              const glm::ivec4& viewport,
                              glm::ray *ray);
        
        static bool is_triangles_intersected(const glm::ray& ray,
                                             const std::vector<std::tuple<glm::vec3, glm::vec3, glm::vec3>>& triangles,
                                             const glm::ivec2& point, glm::vec3* intersected_point);
        
        static bool is_bounding_box_intersected(const glm::ray& ray,
                                                const glm::vec3& min_bound, const glm::vec3& max_bound,
                                                const glm::ivec2& point, glm::vec3* intersected_point);
        
        void create_box2d_world(const glm::vec2 &min_bound, const glm::vec2 &max_bound);
        
        b2Body* create_box2d_body(const std::shared_ptr<b2BodyDef> box2d_body_definition);
        void destroy_box2d_body(b2Body* box2d_body);
        
        void update(f32 deltatime);
    };
};

#endif
