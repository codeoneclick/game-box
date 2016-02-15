//
//  canvas_chunk.cpp
//  gbEditor
//
//  Created by Serhii Serhiiv on 2/12/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "canvas_chunk.h"
#include "ces_material_component.h"
#include "ces_geometry_quad_component.h"

namespace gb
{
    namespace ed
    {
        const f32 canvas_chunk::k_size = 256.f;
        
        canvas_chunk::canvas_chunk()
        {
            ces_material_component_shared_ptr material_component = std::make_shared<ces_material_component>();
            ces_entity::add_component(material_component);
            
            ces_geometry_component_shared_ptr geometry_component = std::make_shared<ces_geometry_quad_component>();
            ces_entity::add_component(geometry_component);
            
            unsafe_get_geometry_quad_component_from_this->set_size(glm::vec2(k_size));
        }
        
        canvas_chunk::~canvas_chunk()
        {
            
        }
    }
}