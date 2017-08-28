//
//  heightmap_geometry_generator.h
//  gbCore
//
//  Created by serhii serhiiv on 8/24/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
    class heightmap_geometry_generator
    {
    private:
    
        static void create_vertices_data(const ces_entity_shared_ptr& entity);
        static void create_vbos_data(const ces_entity_shared_ptr& entity);
        static void create_ibos_data(const ces_entity_shared_ptr& entity);
        static void create_tangent_space(const ces_entity_shared_ptr& entity);
        static void create_attaches_to_vbo(const ces_entity_shared_ptr& entity);
        static void create_bounding_boxes(const ces_entity_shared_ptr& entity);
        
    protected:

    public:
        
        heightmap_geometry_generator() = default;
        ~heightmap_geometry_generator() = default;
        
        static void generate_geometry(const ces_entity_shared_ptr& entity);
        static void generate_tangent_space(const ces_entity_shared_ptr& entity);
        static void generate_attaches_to_vbo(const ces_entity_shared_ptr& entity);
        static void generate_bounding_boxes(const ces_entity_shared_ptr& entity);
    };
};

