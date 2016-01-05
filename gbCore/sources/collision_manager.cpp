//
//  collision_manager.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/31/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "collision_manager.h"

namespace gb
{
    collision_manager::collision_manager() :
    m_box2d_world(nullptr)
    {
        
    }
    
    collision_manager::~collision_manager()
    {
        
    }
    
    void collision_manager::unproject(const glm::ivec2& point,
                                      const glm::mat4x4& mat_v, const glm::mat4x4& mat_p,
                                      const glm::ivec4& viewport,
                                      glm::ray *ray)
    {
        glm::vec3 direction, origin;
        f32 screen_x =  -((( 2.0f * point.x ) / viewport[2]) - 1.0f ) / mat_p[0][0];
        f32 screen_y =  ((( 2.0f * point.y ) / viewport[3]) - 1.0f ) / mat_p[1][1];
        glm::mat4 mat_iv = glm::inverse(mat_v);
        
        direction.x  = (screen_x * mat_iv[0][0] + screen_y * mat_iv[1][0] + mat_iv[2][0]);
        direction.y  = (screen_x * mat_iv[0][1] + screen_y * mat_iv[1][1] + mat_iv[2][1]);
        direction.z  = (screen_x * mat_iv[0][2] + screen_y * mat_iv[1][2] + mat_iv[2][2]);
        
        origin.x = mat_iv[3][0];
        origin.y = mat_iv[3][1];
        origin.z = mat_iv[3][2];
        
        ray->set_direction(direction);
        ray->set_origin(origin);
    }
    
    bool collision_manager::is_triangles_intersected(const glm::ray& ray,
                                                     const std::vector<std::tuple<glm::vec3, glm::vec3, glm::vec3>>& triangles,
                                                     const glm::ivec2& point, glm::vec3* intersected_point)
    {
        for(const auto& iterator : triangles)
        {
            if(collision_manager::triangle_intersection(std::get<0>(iterator),
                                                        std::get<1>(iterator),
                                                        std::get<2>(iterator),
                                                        ray, intersected_point))
            {
                return true;
            }
            
        }
        return false;
    }
    
    bool collision_manager::is_bounding_box_intersected(const glm::ray& ray,
                                                        const glm::vec3& min_bound, const glm::vec3& max_bound,
                                                        const glm::ivec2& point, glm::vec3* intersected_point)
    {
        return glm::intersect(ray, min_bound, max_bound);
    }
    
    bool collision_manager::triangle_intersection(const glm::vec3& point_01, const glm::vec3& point_02, const glm::vec3& point_03,
                                                  const glm::ray& ray,
                                                  glm::vec3* intersected_point)
    {
        glm::vec3 edge_01 = point_02 - point_01;
        glm::vec3 edge_02 = point_03 - point_01;
        
        glm::vec3 p_vector = glm::cross(ray.get_direction(), edge_02);
        f32 determinant = glm::dot(edge_01, p_vector);
        if(fabs(determinant) < .0001f)
        {
            return false;
        }
        
        f32 inv_determinant = 1.f / determinant;
        glm::vec3 t_vector = ray.get_origin() - point_01;
        
        f32 u = glm::dot(t_vector, p_vector) * inv_determinant;
        if ( u < -0.0001f || u > 1.0001f )
        {
            return false;
        }
        
        glm::vec3 q_vector = glm::cross(t_vector, edge_01);
        f32 v = glm::dot(ray.get_direction(), q_vector) * inv_determinant;
        if ( v < -0.0001f || (v + u) > 1.0001f )
        {
            return false;
        }
        (*intersected_point) = point_01 + (edge_01 * u) + (edge_02 * v);
        return true;
    }
    
    void collision_manager::create_box2d_world(const glm::vec2 &min_bound, const glm::vec2 &max_bound)
    {
        b2Vec2 gravity = b2Vec2(.0f, .0f);
        m_box2d_world = std::make_shared<b2World>(gravity);
        m_box2d_world->SetContinuousPhysics(true);
        m_box2d_world->SetContactListener(this);
        
        b2BodyDef bound_body_definition;
        bound_body_definition.position.Set(0, 0);
        b2Body* bound_body = m_box2d_world->CreateBody(&bound_body_definition);
        
        b2EdgeShape bounding_box;
        bounding_box.Set(b2Vec2(min_bound.x, min_bound.y), b2Vec2(max_bound.x, min_bound.y));
        bound_body->CreateFixture(&bounding_box, 0);
        bounding_box.Set(b2Vec2(min_bound.x, max_bound.y), b2Vec2(max_bound.x, max_bound.y));
        bound_body->CreateFixture(&bounding_box, 0);
        bounding_box.Set(b2Vec2(min_bound.x, max_bound.y), b2Vec2(min_bound.x, min_bound.y));
        bound_body->CreateFixture(&bounding_box, 0);
        bounding_box.Set(b2Vec2(max_bound.x, max_bound.y), b2Vec2(max_bound.x, min_bound.y));
        bound_body->CreateFixture(&bounding_box, 0);
    }
    
    b2Body* collision_manager::create_box2d_body(const std::shared_ptr<b2BodyDef> box2d_body_definition)
    {
        assert(m_box2d_world);
        return m_box2d_world->CreateBody(box2d_body_definition.get());
    }
    
    void collision_manager::destroy_box2d_body(b2Body* box2d_body)
    {
        assert(m_box2d_world);
        m_box2d_world->DestroyBody(box2d_body);
        box2d_body = nullptr;
    }
    
    void collision_manager::update(f32 deltatime)
    {
        if(m_box2d_world)
        {
            m_box2d_world->Step(1.f / 60.f, 1, 1);
        }
    }
    
    void collision_manager::BeginContact(b2Contact* contact)
    {
        
    }
    
    void collision_manager::EndContact(b2Contact* contact)
    {
        
    }
}