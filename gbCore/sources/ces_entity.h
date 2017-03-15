//
//  ces_entity.h
//  gbCore
//
//  Created by sergey.sergeev on 8/11/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

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
        bool m_visible_in_next_frame;
        
        template<typename T_COMPONENT>
        void add_component_recursively()
        {
            std::shared_ptr<T_COMPONENT> component = std::make_shared<T_COMPONENT>();
            ces_entity::add_component(component);
            
            for(const auto& child : m_unique_children)
            {
                child->add_component_recursively<T_COMPONENT>();
            }
        };
        
        template<typename T_COMPONENT>
        void remove_component_recursively()
        {
            ces_entity::remove_component(T_COMPONENT::class_guid());
            
            for(const auto& child : m_unique_children)
            {
                child->remove_component_recursively<T_COMPONENT>();
            }
        };
        
    public:
        
        ces_entity();
        virtual ~ces_entity();
        
        void add_component(const ces_base_component_shared_ptr& component);
        
        void remove_component(const ces_base_component_shared_ptr& component);
        void remove_component(uint8_t guid);
        void remove_components();
        
        template<typename TComponent> bool is_component_exist() const
        {
            return m_components[TComponent::class_guid()] != nullptr;
        }
        
        template<typename TComponent> std::shared_ptr<TComponent> get_component() const
        {
            std::shared_ptr<TComponent> component = std::static_pointer_cast<TComponent>(m_components[TComponent::class_guid()]);
            return component;
        }
        
        template<typename TComponent> TComponent* get_unsafe_component() const
        {
            TComponent* component = static_cast<TComponent*>(m_components[TComponent::class_guid()].get());
            return component;
        }
        
        std::property_ro<const std::array<ces_base_component_shared_ptr, std::numeric_limits<uint8_t>::max()>&> components;
        
        virtual void add_child(const ces_entity_shared_ptr& child);
        virtual void remove_child(const ces_entity_shared_ptr& child);
        virtual void remove_from_parent();
        virtual void rearrange_children_according_to_z_order();
        virtual ces_entity_shared_ptr get_child(const std::string& name, bool recursive = false);
        
        std::property_ro<ces_entity_shared_ptr> parent;
        std::property_ro<std::list<ces_entity_shared_ptr>> children;
        
        std::property_rw<std::string> tag;
        
        std::property_rw<bool> visible;
        std::property_rw<bool> visible_in_next_frame;
    };
};
