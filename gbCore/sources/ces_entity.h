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
        
        std::vector<ces_base_component_shared_ptr> m_components;
        ces_entity_shared_ptr m_parent;
        
        std::set<ces_entity_shared_ptr> m_children;
        std::list<ces_entity_shared_ptr> m_ordered_children;
        
        std::string m_tag;
        
        void add_scene_component();
        void remove_scene_component();
        
    public:
        
        ces_entity();
        virtual ~ces_entity();
        
        void add_component(const std::shared_ptr<ces_base_component>& component);
        
        void remove_component(const std::shared_ptr<ces_base_component>& component);
        void remove_component(i32 type);
        void remove_components();
        
        bool is_component_exist(i32 type) const;
        
        ces_base_component_shared_ptr get_component(i32 type) const;
        std::vector<ces_base_component_shared_ptr> get_components() const;
        
        void add_child(const ces_entity_shared_ptr& child);
        void remove_child(const ces_entity_shared_ptr& child);
        
        ces_entity_shared_ptr get_parent() const;
        const std::list<ces_entity_shared_ptr>& get_children() const;
        
        void set_tag(const std::string& tag);
        std::string get_tag() const;
    };
};

#endif
