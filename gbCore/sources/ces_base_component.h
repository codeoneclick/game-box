//
//  ces_base_component.h
//  gbCore
//
//  Created by sergey.sergeev on 8/11/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef ces_base_component_h
#define ces_base_component_h

#include "main_headers.h"
#include "declarations.h"
#include "ces_component_types.h"

namespace gb
{
    class ces_base_component : public std::enable_shared_from_this<ces_base_component>
    {
    private:
        
    protected:
        
        i32 m_type;
        ces_base_component();
        
    public:
        
        virtual ~ces_base_component() = default;
        
        i32 get_type() const;
    };


#define unsafe_get_material_component(entity) static_cast<ces_material_component*>(entity->get_component(e_ces_component_type_material).get())
#define unsafe_get_material_component_from_this static_cast<ces_material_component*>(ces_entity::get_component(e_ces_component_type_material).get())
    
#define unsafe_get_transformation_component(entity) static_cast<ces_transformation_component*>(entity->get_component(e_ces_component_type_transformation).get())
#define unsafe_get_transformation_component_from_this static_cast<ces_transformation_component*>(ces_entity::get_component(e_ces_component_type_transformation).get())
    
#define unsafe_get_geometry_component(entity) static_cast<ces_geometry_component*>(entity->get_component(e_ces_component_type_geometry).get())
#define unsafe_get_geometry_component_from_this static_cast<ces_geometry_component*>(ces_entity::get_component(e_ces_component_type_geometry).get())
    
#define unsafe_get_geometry_quad_component(entity) static_cast<ces_geometry_quad_component*>(entity->get_component(e_ces_component_type_geometry).get())
#define unsafe_get_geometry_quad_component_from_this static_cast<ces_geometry_quad_component*>(ces_entity::get_component(e_ces_component_type_geometry).get())
    
#define unsafe_get_geometry_freeform_component(entity) static_cast<ces_geometry_freeform_component*>(entity->get_component(e_ces_component_type_geometry).get())
#define unsafe_get_geometry_freeform_component_from_this static_cast<ces_geometry_freeform_component*>(ces_entity::get_component(e_ces_component_type_geometry).get())
    
#define unsafe_get_text_component(entity) static_cast<ces_text_component*>(entity->get_component(e_ces_component_type_text).get())
#define unsafe_get_text_component_from_this static_cast<ces_text_component*>(ces_entity::get_component(e_ces_component_type_text).get())
    
};

#endif
