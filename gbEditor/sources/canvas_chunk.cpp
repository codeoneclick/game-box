//
//  canvas_chunk.cpp
//  gbEditor
//
//  Created by Serhii Serhiiv on 2/12/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "canvas_chunk.h"
#include "ces_material_component.h"

namespace gb
{
    namespace ed
    {
        
        canvas_chunk::canvas_chunk()
        {
            ces_material_component_shared_ptr material_component = std::make_shared<ces_material_component>();
            ces_entity::add_component(material_component);
        }
        
        canvas_chunk::~canvas_chunk()
        {
            
        }
    }
}