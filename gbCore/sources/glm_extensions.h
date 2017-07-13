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
    
    inline glm::vec2 transform_to_vec2(const glm::vec3 &vector, const glm::mat4 &matrix)
    {
        glm::vec4 result;
        result.x = vector.x;
        result.y = vector.y;
        result.z = vector.z;
        result.w = 1.f;
        result = matrix * result;
        
        return glm::vec2(result.x, result.y);
    };
    
    inline void transform_to_vec2_out(const glm::vec3 &vector, const glm::mat4 &matrix, glm::vec2* out)
    {
        glm::vec4 result;
        result.x = vector.x;
        result.y = vector.y;
        result.z = vector.z;
        result.w = 1.f;
        result = matrix * result;
        
        (*out).x = result.x;
        (*out).y = result.y;
    };
    
    inline glm::vec2 transform(const glm::vec2 &vector, const glm::mat4 &matrix)
    {
        glm::vec4 result;
        result.x = vector.x;
        result.y = vector.y;
        result.z = 0.f;
        result.w = 1.f;
        result = matrix * result;
        return glm::vec2(result.x, result.y);
    };
    
    inline glm::vec3 transform(const glm::vec3 &vector, const glm::mat4 &matrix)
    {
        glm::vec4 result;
        result.x = vector.x;
        result.y = vector.y;
        result.z = vector.z;
        result.w = 1.f;
        result = matrix * result;
        return glm::vec3(result.x, result.y, result.z);
    };
    
    inline glm::vec4 transform(const glm::vec4 &vertex, const glm::mat4 &matrix)
    {
        return matrix * vertex;
    };
    
    inline f32 perp_product(const glm::vec2& vec_01, const glm::vec2& vec_02)
    {
        return vec_01.x * vec_02.y - vec_01.y * vec_02.x;
    };
    
    struct segment
    {
    private:
        
    protected:
        
    public:
        
        glm::vec2 point_01;
        glm::vec2 point_02;
        
        segment() :
        point_01(0.f),
        point_02(0.f)
        {
            
        };
        
        segment(const glm::vec2& _point_01, const glm::vec2& _point_02) :
        point_01(_point_01),
        point_02(_point_02)
        {
            
        };
    };
    
    inline bool point_in_segment(const glm::vec2& point, const segment& _segment)
    {
        if (!std::is_f32_equal(_segment.point_01.x, _segment.point_02.x))
        {
            if (_segment.point_01.x < point.x && point.x < _segment.point_02.x)
            {
                return true;
            }
            if (_segment.point_01.x > point.x && point.x > _segment.point_02.x)
            {
                return true;
            }
        }
        else
        {
            if (_segment.point_01.y < point.y && point.y < _segment.point_02.y)
            {
                return true;
            }
            if (_segment.point_01.y > point.y && point.y > _segment.point_02.y)
            {
                return true;
            }
        }
        return false;
    };
    
    inline bool segments_intersect(const segment& segment_01, const segment& segment_02, glm::vec2* intersection_point = nullptr, glm::vec2* overlap_point = nullptr)
    {
        glm::vec2 u = segment_01.point_02 - segment_01.point_01;
        glm::vec2 v = segment_02.point_02 - segment_02.point_01;
        glm::vec2 w = segment_01.point_01 - segment_02.point_01;
        f32 d = perp_product(u, v);
        
        if (fabsf(d) <= std::numeric_limits<f32>::epsilon())
        {
            if (!std::is_f32_equal(perp_product(u,w), 0.f) || !std::is_f32_equal(perp_product(v,w), 0.f))
            {
                return false;
            }

            f32 du = dot(u, u);
            f32 dv = dot(v, v);
            if (std::is_f32_equal(du, 0.f) && std::is_f32_equal(dv, 0.f))
            {
                if (segment_01.point_01 !=  segment_02.point_01)
                {
                    return false;
                }
                if(intersection_point)
                {
                    *intersection_point = segment_01.point_01;
                }
                return true;
            }
            if (std::is_f32_equal(du, 0.f))
            {
                if (!point_in_segment(segment_01.point_01, segment_02))
                {
                    return false;
                }
                if(intersection_point)
                {
                    *intersection_point = segment_01.point_01;
                }
                return true;
            }
            if (std::is_f32_equal(dv, 0.f))
            {
                if  (!point_in_segment(segment_02.point_01, segment_01))
                {
                    return false;
                }
                if(intersection_point)
                {
                    *intersection_point = segment_02.point_01;
                }
                return true;
            }
            
            f32 t0, t1;
            glm::vec2 w2 = segment_01.point_02 - segment_02.point_01;
            if (!std::is_f32_equal(v.x, 0.f))
            {
                t0 = w.x / v.x;
                t1 = w2.x / v.x;
            }
            else
            {
                t0 = w.y / v.y;
                t1 = w2.y / v.y;
            }
            if (t0 > t1)
            {
                f32 t = t0; t0 = t1; t1 = t;
            }
            if (t0 > 1.f || t1 < 0.f)
            {
                return false;
            }
            
            t0 = t0 < 0.f ? 0.f : t0;
            t1 = t1 > 1.f ? 1.f : t1;
            if (std::is_f32_equal(t0, t1))
            {
                if(intersection_point)
                {
                    *intersection_point = segment_02.point_01 +  t0 * v;
                }
                return true;
            }
            if(intersection_point)
            {
                *intersection_point = segment_02.point_01 + t0 * v;
            }
            if(overlap_point)
            {
                *overlap_point = segment_02.point_01 + t1 * v;
            }
            return true;
        }
        
        f32 sI = perp_product(v, w) / d;
        if (sI < 0.f || sI > 1.f)
        {
            return false;
        }
        
        f32 tI = perp_product(u, w) / d;
        if (tI < 0.f || tI > 1.f)
        {
            return false;
        }
        
        if(intersection_point)
        {
            *intersection_point = segment_01.point_01 + sI * u;
        }
        return true;
    };
    
    inline f32 sign(const glm::vec2& point_01, const glm::vec2& point_02, const glm::vec2& point_03)
    {
        return (point_01.x - point_03.x) * (point_02.y - point_03.y) - (point_02.x - point_03.x) * (point_01.y - point_03.y);
    }
    
    struct triangle
    {
        triangle()
        {
            
        };
        
        triangle(const glm::vec2& point_01, const glm::vec2& point_02, const glm::vec2& point_03)
        {
            points[0] = point_01;
            points[1] = point_02;
            points[2] = point_03;
            
            segments[0] = segment(point_01, point_02);
            segments[1] = segment(point_02, point_03);
            segments[2] = segment(point_03, point_01);
        };
        
        std::array<segment, 3> segments;
        std::array<glm::vec2, 3> points;
    };
    
    inline bool point_in_triangle(const glm::vec2& point, const glm::triangle& triangle)
    {
        bool b1, b2, b3;
        b1 = sign(point, triangle.points[0], triangle.points[1]) < 0.0f;
        b2 = sign(point, triangle.points[1], triangle.points[2]) < 0.0f;
        b3 = sign(point, triangle.points[2], triangle.points[0]) < 0.0f;
        return ((b1 == b2) && (b2 == b3));
    }
    
    struct ray
    {
    protected:
        
        glm::vec3 m_origin;
        glm::vec3 m_direction;
        glm::vec3 m_inverted_direction;
        i32 m_signs[3];
        
    public:
        ray() : m_origin(0.0),
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
            m_signs[0] = (m_inverted_direction.x < 0.f);
            m_signs[1] = (m_inverted_direction.y < 0.f   );
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
    
    inline bool intersect(const glm::vec4 &frame, const glm::vec2& point)
    {
        if(point.x >= frame.x && point.x <= frame.z &&
           point.y >= frame.y && point.y <= frame.w)
        {
            return true;
        }
        return false;
    }
    
    // https://github.com/ncase/sight-and-light/blob/gh-pages/draft6.html
    inline bool intersect(const glm::vec4& ray, const glm::vec4& edge, glm::vec2* intersected_point, f32* distance)
    {
        f32 r_dx = ray.z - ray.x;
        f32 r_dy = ray.w - ray.y;
        
        f32 s_dx = edge.z - edge.x;
        f32 s_dy = edge.w - edge.y;
        
        f32 r_mag = sqrtf(r_dx * r_dx + r_dy * r_dy);
        f32 s_mag = sqrtf(s_dx * s_dx + s_dy * s_dy);
        
        if(std::is_f32_equal(r_dx / r_mag, s_dx / s_mag) &&
           std::is_f32_equal(r_dy / r_mag, s_dy / s_mag))
        {
            return false;
        }
        
        f32 T2 = (r_dx * (edge.y - ray.y) + r_dy * (ray.x - edge.x)) / (s_dx * r_dy - s_dy * r_dx);
        f32 T1 = (edge.x + s_dx * T2 - ray.x) / r_dx;
        
        if(T1 < 0.f)
        {
            return false;
        }
        
        if(T2 < 0.f || T2 > 1.f)
        {
            return false;
        }
        
        (*intersected_point).x = ray.x + r_dx * T1;
        (*intersected_point).y = ray.y + r_dy * T1;
        *distance = T1;
        return true;
    }
    
    inline bool intersect(const glm::vec4 &bound_01, const glm::vec4 &bound_02)
    {
        bool is_intersect = bound_01.x > bound_02.z ||
        bound_02.x > bound_01.z ||
        bound_01.y > bound_02.w ||
        bound_02.y > bound_01.w;
        return !is_intersect;
    };

	inline void project_verices(const glm::vec2& axis, const std::vector<glm::vec2>& vertices, f32* min, f32* max)
	{
		f32 dot = glm::dot(axis, vertices.at(0));
		*min = dot;
		*max = dot;
		for (i32 i = 0; i <vertices.size(); ++i) 
		{
			dot = glm::dot(axis, vertices.at(i));
			if (dot < *min) 
			{
				*min = dot;
			}
			else 
			{
				if (dot > *max) {
					*max = dot;
				}
			}
		}
	}

	inline bool intersect(const std::vector<glm::vec2>& vertices_01, const glm::mat4& mat_m_01,
		const std::vector<glm::vec2>& vertices_02, const glm::mat4& mat_m_02)
	{
		std::vector<glm::vec2> edges;
		std::vector<glm::vec2> transformed_vertices_01;
		std::vector<glm::vec2> transformed_vertices_02;

		for (i32 i = 0; i < vertices_01.size(); ++i)
		{
			i32 next_vertex_index = (i + 1) % vertices_01.size();
			edges.push_back(glm::transform(vertices_01[next_vertex_index], mat_m_01) - glm::transform(vertices_01[i], mat_m_01));
			transformed_vertices_01.push_back(glm::transform(vertices_01[i], mat_m_01));
		}

		for (i32 i = 0; i < vertices_02.size(); ++i)
		{
			i32 next_vertex_index = (i + 1) % vertices_02.size();
			edges.push_back(glm::transform(vertices_02[i], mat_m_02) - glm::transform(vertices_02[next_vertex_index], mat_m_02));
			transformed_vertices_02.push_back(glm::transform(vertices_02[i], mat_m_02));
		}

		bool is_intersect = true;
		for (const auto& edge : edges)
		{
			glm::vec2 axis = glm::vec2(-edge.x, edge.y);
			axis = glm::normalize(axis);

			f32 min_01 = 0.f; f32 min_02 = 0.f; f32 max_01 = 0.f; f32 max_02 = 0.f;

			project_verices(axis, vertices_01, &min_01, &max_01);
			project_verices(axis, vertices_02, &min_02, &max_02);

			f32 distance = 0.f;

			if (min_01 < min_02) 
			{
				distance = min_02 - max_01;
			}
			else
			{
				distance = min_01 - max_02;
			}
			is_intersect = !(distance > 0.f);
		}
		return is_intersect;
	}
    
    inline bool inside(const glm::vec4& small_bound, const glm::vec4& big_bound)
    {
        glm::vec2 points[4];
        points[0] = glm::vec2(small_bound.x, small_bound.y);
        points[1] = glm::vec2(small_bound.x, small_bound.w);
        points[2] = glm::vec2(small_bound.z, small_bound.y);
        points[3] = glm::vec2(small_bound.z, small_bound.w);
        for(i32 i = 0; i < 4; ++i)
        {
            if(intersect(big_bound, points[i]))
            {
                return true;
            }
        }
        return false;
    }
    
    inline f32 wrap_radians(f32 radians)
    {
        if(std::isnan(radians))
        {
            return 0.f;
        }
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
        if(std::isnan(degrees))
        {
            return 0.f;
        }
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
        start = wrap_degrees(start);
        end = wrap_degrees(end);
        
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
    
    inline f32 delta_angles_degrees(f32 angle_01, f32 angle_02)
    {
        f32 delta_angles = glm::wrap_degrees(angle_01) - glm::wrap_degrees(angle_02);
        delta_angles = glm::wrap_degrees((static_cast<i32>(delta_angles) + 180) % 360 - 180);
        return delta_angles;
    };
    
    inline glm::vec3 random_inside_sphere(f32 radius)
    {
        return glm::vec3(std::get_random_f(-radius, radius),
                         std::get_random_f(-radius, radius),
                         std::get_random_f(-radius, radius));
    };
    
    enum e_orientation
    {
        e_orientation_colinear = 0,
        e_orientation_clockwise,
        e_orientation_counterclockwise
    };
    
    inline e_orientation orientation(const glm::vec2& point_01, const glm::vec2& point_02, const glm::vec2& point_03)
    {
        i32 result = (point_02.y - point_01.y) * (point_03.x - point_02.x) - (point_02.x - point_01.x) * (point_03.y - point_02.y);
        if (result == 0)
        {
            return e_orientation_colinear;
        }
        return (result > 0) ? e_orientation_clockwise : e_orientation_counterclockwise;
    };
};

#endif
