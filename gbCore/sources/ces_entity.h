//
//  ces_entity.h
//  gbCore
//
//  Created by sergey.sergeev on 8/11/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef ces_entity_h
#define ces_entity_h

#include "ces_base_component.h"

namespace gb
{
    class ces_entity : public std::enable_shared_from_this<ces_entity>
    {
    private:
        
    protected:
        
        std::array<ces_base_component_shared_ptr, std::numeric_limits<uint8_t>::max()> m_components;

        std::weak_ptr<ces_entity> m_parent;
        
        std::set<ces_entity_shared_ptr> m_unique_children;
        std::list<ces_entity_shared_ptr> m_ordered_children;
        
        std::string m_tag;
        bool m_visible;
        
        void add_scene_component();
        void remove_scene_component();
        
    public:
        
        ces_entity();
        virtual ~ces_entity();
        
        void add_component(const ces_base_component_shared_ptr& component);
        
        void remove_component(const ces_base_component_shared_ptr& component);
        void remove_component(uint8_t guid);
        void remove_components();
        
        bool is_component_exist(uint8_t guid) const;
        
        ces_base_component_shared_ptr get_component(uint8_t guid) const;
        
        std::property_ro<std::array<ces_base_component_shared_ptr, std::numeric_limits<uint8_t>::max()>> components;
        
        void add_child(const ces_entity_shared_ptr& child);
        void remove_child(const ces_entity_shared_ptr& child);
        
        std::property_ro<ces_entity_shared_ptr> parent;
        std::property_ro<std::list<ces_entity_shared_ptr>> children;
        
        std::property_rw<std::string> tag;
        
        std::property_rw<bool> visible;
    };
};

#endif
