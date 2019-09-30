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
        
        friend class ces_base_component;
        
    protected:
        
        std::array<ces_base_component_shared_ptr, std::numeric_limits<uint8_t>::max()> m_components;
        std::bitset<std::numeric_limits<uint8_t>::max()> m_mask;
        
        std::list<std::packaged_task<ces_base_component::component_constructor>> m_deferred_components_constructors;

        ces_entity_weak_ptr m_self_weak;
        ces_entity_weak_ptr m_parent;
        ces_systems_feeder_weak_ptr m_systems_feeder;
        
        std::unordered_set<ces_entity_shared_ptr> m_unique_children;
        std::vector<ces_entity_shared_ptr> m_ordered_children;
        
        std::string m_tag;
        bool m_visible;
        bool m_visible_in_next_frame;
        bool m_is_root;
        
        void update_systems_feeder_recursively(const ces_entity_shared_ptr& entity,
                                               const ces_systems_feeder_shared_ptr& systems_feeder);
        
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
        
        template<typename T, typename... ARGS> static std::shared_ptr<T> construct(ARGS... args)
        {
            auto entity = std::make_shared<T>(std::forward<ARGS>(args)...);
            entity->m_self_weak = entity;
            entity->construct_components();
            entity->setup_components();
            return entity;
        };
        
        void construct_components();
        virtual void setup_components();
        
        template<typename T, typename... ARGS> void add_deferred_component_constructor(ARGS... args)
        {
            std::packaged_task<ces_base_component::component_constructor> deferred_constructor(std::bind(ces_base_component::construct<T, ARGS...>, std::forward<ARGS>(args)...));
            m_deferred_components_constructors.push_back(std::move(deferred_constructor));
        };
        
        template<typename T> std::shared_ptr<T> as()
        {
            return std::static_pointer_cast<T>(shared_from_this());
        };
        
        void add_component(const ces_base_component_shared_ptr& component);
        
        template<typename T, typename... ARGS> std::shared_ptr<T> add_component(ARGS... args)
        {
            auto component = std::make_shared<T>(std::forward<ARGS>(args)...);
            ces_entity::add_component(component);
            return component;
        };
        
        void remove_component(const ces_base_component_shared_ptr& component);
        void remove_component(uint8_t guid);
        void remove_components();
        
        template<typename TComponent> bool is_component_exist() const
        {
            return m_components[TComponent::class_guid()] != nullptr;
        };
        
        bool is_components_exist(const std::bitset<std::numeric_limits<uint8_t>::max()>& mask);
        
        template<typename TComponent> std::shared_ptr<TComponent> get_component() const
        {
            std::shared_ptr<TComponent> component = std::static_pointer_cast<TComponent>(m_components[TComponent::class_guid()]);
            return component;
        };
        
        template<typename TComponent> TComponent* get_component_raw() const
        {
            TComponent* component = static_cast<TComponent*>(m_components[TComponent::class_guid()].get());
            return component;
        };
        
        std::property_ro<const std::array<ces_base_component_shared_ptr, std::numeric_limits<uint8_t>::max()>&> components;
        
        virtual void add_child(const ces_entity_shared_ptr& child);
        virtual void remove_child(const ces_entity_shared_ptr& child);
        virtual void remove_from_parent();
        virtual void rearrange_children_according_to_z_order();
        virtual ces_entity_shared_ptr get_child(const std::string& name, bool recursive = false);
        
        std::property_ro<ces_entity_shared_ptr> parent;
        std::property_ro<const std::vector<ces_entity_shared_ptr>&> children;
        std::property_ro<ui32> children_count;
        
        std::property_rw<std::string> tag;
        
        std::property_rw<bool> visible;
        std::property_rw<bool> visible_in_next_frame;
        
        bool get_is_visible() const;
        bool get_is_visible_in_next_frame() const;
        
        bool is_on_scene() const;
        
        void enumerate_children(const std::function<void(const ces_entity_shared_ptr& child)> enumerator);
    };
};
