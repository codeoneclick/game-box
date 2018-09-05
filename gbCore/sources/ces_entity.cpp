//
//  ces_entity.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/11/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "ces_entity.h"
#include "ces_systems_feeder.h"

static i64 g_tag = 0;

namespace gb
{
    ces_entity::ces_entity() :
    m_tag("ces_entity_" + std::to_string(g_tag++)),
    m_visible(true),
    m_visible_in_next_frame(true),
    m_is_root(false)
    {
        m_components.fill(nullptr);
        
        components.getter([=]() -> const std::array<ces_base_component_shared_ptr, std::numeric_limits<uint8_t>::max()>& {
            return m_components;
        });
        
        parent.getter([=]() {
            return m_parent.lock();
        });
        
        children.getter([=]() -> const std::vector<ces_entity_shared_ptr>& {
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
        
        m_mask.reset();
    }
    
    ces_entity::~ces_entity()
    {
        m_components.fill(nullptr);
        m_unique_children.clear();
        m_ordered_children.clear();
    }
    
    void ces_entity::construct_components()
    {
        for(auto& deferred_constructor : m_deferred_components_constructors)
        {
            auto result = deferred_constructor.get_future();
            deferred_constructor();
            ces_entity::add_component(std::move(result.get()));
        }
        m_deferred_components_constructors.clear();
    }
    
    void ces_entity::setup_components()
    {
        
    }
    
    void ces_entity::add_component(const std::shared_ptr<ces_base_component>& component)
    {
        assert(component);
		guid_t guid = component->instance_guid();
        m_components[guid] = component;
        m_mask.set(guid);
        
        if(!m_systems_feeder.expired())
        {
            m_systems_feeder.lock()->on_entity_component_added(shared_from_this());
        }
    }
    
    void ces_entity::remove_component(const std::shared_ptr<ces_base_component>& component)
    {
        assert(component);
		guid_t guid = component->instance_guid();
        ces_entity::remove_component(guid);
    }
    
    void ces_entity::remove_component(uint8_t guid)
    {
        m_components[guid] = nullptr;
        m_mask.reset(guid);
        
        if(!m_systems_feeder.expired())
        {
            m_systems_feeder.lock()->on_entity_component_removed(shared_from_this());
        }
    }
    
    void ces_entity::remove_components()
    {
        for(const auto& component : m_components)
        {
            if(component)
            {
                ces_entity::remove_component(component->instance_guid());
            }
        }
    }
    
    bool ces_entity::is_components_exist(const std::bitset<std::numeric_limits<uint8_t>::max()>& mask)
    {
        return (m_mask & mask) == mask;
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
        
        if(!m_systems_feeder.expired())
        {
            ces_entity::update_systems_feeder_recursively(child, m_systems_feeder.lock());
        }
    }
    
    void ces_entity::remove_child(const ces_entity_shared_ptr& child)
    {
        if(m_unique_children.count(child) != 0)
        {
            const auto& it = std::find(m_ordered_children.begin(), m_ordered_children.end(), child);
            m_unique_children.erase(child);
            m_ordered_children.erase(it);
            ces_entity::update_systems_feeder_recursively(child, nullptr);
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
    
    void ces_entity::update_systems_feeder_recursively(const ces_entity_shared_ptr& entity,
                                                       const ces_systems_feeder_shared_ptr& systems_feeder)
    {
        if(systems_feeder)
        {
            systems_feeder->on_entity_added(entity);
        }
        else if(!entity->m_systems_feeder.expired())
        {
            entity->m_systems_feeder.lock()->on_entity_removed(entity);
        }
        entity->m_systems_feeder = systems_feeder;
        
        for(const auto& child : entity->m_unique_children)
        {
            ces_entity::update_systems_feeder_recursively(child, systems_feeder);
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
    
    void ces_entity::enumerate_children(const std::function<void(const ces_entity_shared_ptr& child)> enumerator)
    {
        for(const auto& child : m_ordered_children)
        {
            enumerator(child);
        }
    }
    
    bool ces_entity::is_on_scene() const
    {
        return !m_systems_feeder.expired();
    }
};
