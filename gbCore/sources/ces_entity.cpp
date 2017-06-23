//
//  ces_entity.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/11/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "ces_entity.h"

static i64 g_tag = 0;

namespace gb
{
    ces_entity::ces_entity() :
    m_tag("ces_entity_" + std::to_string(g_tag++)),
    m_visible(true),
    m_visible_in_next_frame(true)
    {
        m_components.fill(nullptr);
        
        components.getter([=]() -> const std::array<ces_base_component_shared_ptr, std::numeric_limits<uint8_t>::max()>& {
            return m_components;
        });
        
        parent.getter([=]() {
            return m_parent.lock();
        });
        
        children.getter([=]() -> const std::list<ces_entity_shared_ptr>& {
            return m_ordered_children;
        });
        
        children_count.getter([=]() {
            return m_unique_children.size();
        });
        
        tag.setter([=](const std::string& value) {
            m_tag = value;
        });
        tag.getter([=]() {
            return m_tag;
        });
        
        visible.setter([=](bool value) {
            m_visible = value;
            
            for(const auto& child : m_unique_children)
            {
                child->visible = value;
            }
        });
        visible.getter([=]() {
            return m_visible;
        });
        
        visible_in_next_frame.setter([=](bool value) {
            m_visible_in_next_frame = value;
            
            for(const auto& child : m_unique_children)
            {
                child->visible_in_next_frame = value;
            }
        });
        visible_in_next_frame.getter([=]() {
            return m_visible_in_next_frame;
        });
    }
    
    ces_entity::~ces_entity()
    {
        ces_entity::remove_components();
        m_unique_children.clear();
        m_ordered_children.clear();
    }
    
    void ces_entity::add_component(const std::shared_ptr<ces_base_component>& component)
    {
        assert(component);
        uintptr_t guid = component->instance_guid();
        m_components[guid] = component;
    }
    
    void ces_entity::remove_component(const std::shared_ptr<ces_base_component>& component)
    {
        assert(component);
        m_components[component->instance_guid()] = nullptr;
    }
    
    void ces_entity::remove_component(uint8_t guid)
    {
        m_components[guid] = nullptr;
    }
    
    void ces_entity::remove_components()
    {
        m_components.fill(nullptr);
    }
    
    void ces_entity::add_child(const ces_entity_shared_ptr& child)
    {
        if(m_unique_children.count(child) != 0)
        {
            assert(false);
            std::cout<<"error. can't add same child"<<std::endl;
        }
        else
        {
            ces_entity_shared_ptr parent = ces_entity::parent;
            if(parent)
            {
                parent->remove_child(child);
            }
            child->m_parent = shared_from_this();
            
            m_unique_children.insert(child);
            m_ordered_children.push_back(child);
        }
    }
    
    void ces_entity::remove_child(const ces_entity_shared_ptr& child)
    {
        if(m_unique_children.count(child) != 0)
        {
            m_unique_children.erase(child);
            m_ordered_children.remove(child);
        }
    }
    
    void ces_entity::remove_from_parent()
    {
        ces_entity_shared_ptr parent = ces_entity::parent;
        if(parent)
        {
            parent->remove_child(shared_from_this());
        }
    }

    ces_entity_shared_ptr ces_entity::get_child(const std::string& name, bool recursive)
    {
        ces_entity_shared_ptr entity = nullptr;
        for(const auto& child : m_unique_children)
        {
            std::string tag = child->tag;
            if(tag == name)
            {
                entity = child;
                break;
            }
        }
        if(recursive && !entity)
        {
            for(const auto& child : m_unique_children)
            {
                entity = child->get_child(name, recursive);
                if(entity)
                {
                    break;
                }
            }
        }
        return entity;
    }
    
    void ces_entity::rearrange_children_according_to_z_order()
    {
        assert(false);
    }
    
    bool ces_entity::get_is_visible() const
    {
        return m_visible;
    }
    
    bool ces_entity::get_is_visible_in_next_frame() const
    {
        return m_visible_in_next_frame;
    }
};
