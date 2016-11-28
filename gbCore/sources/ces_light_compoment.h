//
//  ces_light_compoment.h
//  gbCore
//
//  Created by sergey.sergeev on 1/11/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef ces_light_compoment_h
#define ces_light_compoment_h

#include "ces_base_component.h"

namespace gb
{
    class ces_light_compoment : public ces_base_component
    {
    private:
        
    protected:
        
        std::list<ces_entity_weak_ptr> m_luminous_entities;
        
    public:
        
        CTTI_CLASS_GUID(ces_light_compoment, ces_base_component::g_guids_container)
        ces_light_compoment();
        ~ces_light_compoment();
        
        void add_luminous_entity(const ces_entity_shared_ptr& entity);
        const std::list<ces_entity_weak_ptr>& get_luminous_entities() const;
        
        void cleanup();
    };
    
#define unsafe_get_light_component(entity) static_cast<ces_light_compoment*>(entity->get_component(ces_light_compoment::class_guid()).get())
#define unsafe_get_light_component_from_this static_cast<ces_light_component*>(ces_entity::get_component(ces_light_compoment::class_guid()).get())
    
};

#endif
