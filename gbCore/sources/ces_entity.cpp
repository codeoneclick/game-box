//
//  ces_entity.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/11/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "ces_entity.h"

#define k_max_components 128

namespace gb
{
    ces_entity::ces_entity() :
    m_parent(nullptr)
    {
        m_components.resize(k_max_components, nullptr);
        ces_entity::remove_components();
    }
    
    ces_entity::~ces_entity()
    {
        ces_entity::remove_components();
        m_children.clear();
    }
    
    void ces_entity::add_component(const std::shared_ptr<ces_base_component>& component)
    {
        assert(component != nullptr && component->get_type() != e_ces_component_type_undefined);
        m_components[component->get_type()] = component;
    }
    
    void ces_entity::remove_component(const std::shared_ptr<ces_base_component>& component)
    {
        assert(component != nullptr && component->get_type() != e_ces_component_type_undefined);
        m_components[component->get_type()] = nullptr;
    }
    
    void ces_entity::remove_component(i32 type)
    {
        m_components[type] = nullptr;
    }
    
    void ces_entity::remove_components()
    {
        for(auto& component : m_components)
        {
            component = nullptr;
        }
    }
    
    bool ces_entity::is_component_exist(i32 type) const
    {
        assert(type != e_ces_component_type_undefined);
        return m_components[type] != nullptr;
    }
    
    std::shared_ptr<ces_base_component> ces_entity::get_component(i32 type) const
    {
        assert(type != e_ces_component_type_undefined);
        return m_components[type];
    }
    
    std::vector<ces_base_component_shared_ptr> ces_entity::get_components() const
    {
        return m_components;
    }
    
    void ces_entity::add_child(const ces_entity_shared_ptr& child)
    {
        if(m_children.count(child) != 0)
        {
            assert(false);
            std::cout<<"error. can't add same child"<<std::endl;
        }
        else
        {
            if(child->m_parent)
            {
                child->m_parent->remove_child(child);
            }
            child->m_parent = shared_from_this();
            m_children.insert(child);
        }
    }
    
    void ces_entity::remove_child(const ces_entity_shared_ptr& child)
    {
        child->m_parent = nullptr;
        m_children.erase(child);
    }
    
    ces_entity_shared_ptr ces_entity::get_parent() const
    {
        return m_parent;
    }
};