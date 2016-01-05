//
//  mesh_constructor.h
//  gbCore
//
//  Created by sergey.sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef mesh_constructor_h
#define mesh_constructor_h

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
    class mesh_constructor
    {
    private:
        
    protected:
        
        static std::tuple<vbo_shared_ptr, ibo_shared_ptr, glm::vec3, glm::vec3> create_sphere_data(f32 radius, i32 rings, i32 sectors);
        static std::tuple<vbo_shared_ptr, ibo_shared_ptr> create_box_data(const glm::vec3& min_bound,
                                                                          const glm::vec3& max_bound);
        
        static glm::vec3 generate_tangent(const glm::vec3& point_01, const glm::vec3& point_02, const glm::vec3& point_03,
                                          const glm::vec2& texcoord_01, const glm::vec2& texcoord_02, const glm::vec2& texcoord_03);
        static glm::vec3 get_closest_point_on_line(const glm::vec3& a, const glm::vec3& b, const glm::vec3& p);
        static glm::vec3 ortogonalize(const glm::vec3& v1, const glm::vec3& v2);
        
    public:
        
        mesh_constructor() = default;
        ~mesh_constructor() = default;
        
        static mesh_shared_ptr create_wireframe_box(const glm::vec3& min_bound,
                                                    const glm::vec3& max_bound);
        
        static mesh_shared_ptr create_plane(const glm::vec2& size);
        
        static mesh_shared_ptr create_ocean_plane(i32 size, f32 altitude);
        
        static mesh_shared_ptr create_sphere(f32 radius, i32 rings, i32 sectors);
        
        static mesh_shared_ptr create_screen_quad();
        
        static instanced_mesh_shared_ptr create_instanced_sphere(f32 radius, i32 rings, i32 sectors);
        
        static mesh_shared_ptr create_box(const glm::vec3& min_bound,
                                          const glm::vec3& max_bound);
        
        static instanced_mesh_shared_ptr create_boxes(const glm::vec3& min_bound,
                                                      const glm::vec3& max_bound,
                                                      i32 num_instances);
        
        static mesh_shared_ptr create_grid(i32 rows, i32 columns);
        
        static mesh_shared_ptr create_3d_tile_cursor();
        
        static mesh_shared_ptr create_model3d_cursor();
    };
};

#endif
