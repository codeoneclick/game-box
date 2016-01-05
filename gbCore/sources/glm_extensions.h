//
//  glm_extensions.h
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef glm_extensions_h
#define glm_extensions_h

#include "main_headers.h"
#include "std_extensions.h"

namespace glm
{
    inline glm::quat slerp(const glm::quat& from, const glm::quat& to, f32 interpolation)
    {
        glm::quat value;
        f32 omega, cosom, sinom, scale0, scale1;
        
        if (interpolation <= 0.0f)
        {
            return from;
        }
        else if (interpolation >= 1.0f)
        {
            return to;
        }
        
        cosom = from.x * to.x + from.y * to.y + from.z * to.z + from.w * to.w;
        if ( cosom < 0.0f )
        {
            value  = -to;
            cosom = -cosom;
        }
        else
        {
            value = to;
        }
        
        if ((1.0f - cosom) > 1e-6f)
        {
            omega  = acosf(cosom);
            sinom  = 1.0f / sinf(omega);
            scale0 = sinf((1.0f - interpolation) * omega) * sinom;
            scale1 = sinf(interpolation * omega) * sinom;
        }
        else
        {
            scale0 = 1.0f - interpolation;
            scale1 = interpolation;
        }
        
        return ((from * scale0) + (value * scale1));
    };
    
    inline glm::vec3 transform(const glm::vec3 &vertex, const glm::mat4x4 &matrix)
    {
        glm::vec4 result = matrix * glm::vec4(vertex.x, vertex.y, vertex.z, 1.0f);
        return glm::vec3(result.x, result.y, result.z);
    };
    
    inline glm::vec4 transform(const glm::vec4 &vertex, const glm::mat4x4 &matrix)
    {
        glm::vec4 result = matrix * vertex;
        return glm::vec4(result.x, result.y, result.z, vertex.w);
    };
    
    struct ray
    {
    protected:
        
        glm::vec3 m_origin;
        glm::vec3 m_direction;
        glm::vec3 m_inverted_direction;
        i32 m_signs[3];
        
    public:
        ray(void) : m_origin(0.0),
        m_direction(0.0)
        {
            
        };
        ray(const glm::vec3& origin, const glm::vec3& direction)
        {
            m_origin = origin;
            ray::set_direction(direction);
        };
        
        ray(const ray &copy)
        {
            m_origin = copy.m_origin;
            m_direction = copy.m_direction;
            m_inverted_direction = copy.m_inverted_direction;
            m_signs[0] = copy.m_signs[0]; m_signs[1] = copy.m_signs[1]; m_signs[2] = copy.m_signs[2];
        };
        
        void set_origin(const glm::vec3& origin)
        {
            m_origin = origin;
        };
        
        glm::vec3 get_origin(void) const
        {
            return m_origin;
        };
        
        void set_direction(const glm::vec3& direction)
        {
            m_direction = direction;
            m_inverted_direction = glm::vec3(1.0 / m_direction.x,
                                             1.0 / m_direction.y,
                                             1.0 / m_direction.z);
            m_signs[0] = (m_inverted_direction.x < 0);
            m_signs[1] = (m_inverted_direction.y < 0);
            m_signs[2] = (m_inverted_direction.z < 0);
        };
        
        glm::vec3 get_direction(void) const
        {
            return m_direction;
        };
        
        glm::vec3 get_inverted_direction(void) const
        {
            return m_inverted_direction;
        };
        
        i32 get_sign(ui32 index) const
        {
            assert(index < 3);
            return m_signs[index];
        }
    };
    
    inline bool intersect(const glm::ray &ray,
                          const glm::vec3& min_bound,
                          const glm::vec3& max_bound)
    {
        glm::vec3 parameters[2];
        parameters[0] = min_bound;
        parameters[1] = max_bound;
        f32 tmin, tmax, tymin, tymax, tzmin, tzmax;
        
        tmin = (parameters[ray.get_sign(0)].x - ray.get_origin().x) * ray.get_inverted_direction().x;
        tmax = (parameters[1 - ray.get_sign(0)].x - ray.get_origin().x) * ray.get_inverted_direction().x;
        tymin = (parameters[ray.get_sign(1)].y - ray.get_origin().y) * ray.get_inverted_direction().y;
        tymax = (parameters[1 - ray.get_sign(1)].y - ray.get_origin().y) * ray.get_inverted_direction().y;
        if ( (tmin > tymax) || (tymin > tmax) )
        {
            return false;
        }
        
        if (tymin > tmin)
        {
            tmin = tymin;
        }
        
        if (tymax < tmax)
        {
            tmax = tymax;
        }
        
        tzmin = (parameters[ray.get_sign(2)].z - ray.get_origin().z) * ray.get_inverted_direction().z;
        tzmax = (parameters[1 - ray.get_sign(2)].z - ray.get_origin().z) * ray.get_inverted_direction().z;
        if ( (tmin > tzmax) || (tzmin > tmax) )
        {
            return false;
        }
        return true;
    };
    
    inline f32 wrap_radians(f32 radians)
    {
        while (radians < .0f)
        {
            radians += M_PI * 2.f;
        }
        while (radians > M_PI * 2.f)
        {
            radians -= M_PI * 2.f;
        }
        return radians;
    };
    
    inline f32 wrap_degrees(f32 degrees)
    {
        while (degrees < .0f)
        {
            degrees += 360.f;
        }
        while (degrees > 360.f)
        {
            degrees -= 360.f;
        }
        return degrees;
    };
    
    inline f32 mix_angles_degrees(f32 start, f32 end, f32 amount)
    {
        f32 difference = fabsf(end - start);
        if (difference > 180.f)
        {
            if (end > start)
            {
                start += 360.f;
            }
            else
            {
                end += 360.f;
            }
        }
        
        f32 value = (start + ((end - start) * amount));
        return wrap_degrees(value);
    };
    
    inline glm::vec3 random_inside_sphere(f32 radius)
    {
        return glm::vec3(std::get_random_f(-radius, radius),
                         std::get_random_f(-radius, radius),
                         std::get_random_f(-radius, radius));
    };
};

#endif
