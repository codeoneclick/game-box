//
//  ces_systems_feeder.cpp
//  gbCore
//
//  Created by Sergey Sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "ces_systems_feeder.h"
#include "ces_base_system.h"
#include "scene_graph.h"
#include "ces_system_modifiers_component.h"
#include <chrono>
#include <cstdlib>
#include <typeinfo>

namespace gb
{
    ces_systems_feeder::ces_systems_feeder()
    {
        
    }
    
    ces_systems_feeder::~ces_systems_feeder()
    {
        
    }
    
    ces_base_system_shared_ptr ces_systems_feeder::get_system(i32 guid) const
    {
        const auto& system = m_systems.find(guid);
        if(system != m_systems.end())
        {
            return system->second;
        }
        return nullptr;
    }
    
    void ces_systems_feeder::on_update(f32 dt)
    {
        // Opt-in diagnostics only: preserve system order and all feeder phases.
        static const bool profile = std::getenv("GB_PROFILE_SYSTEMS") != nullptr;
        static std::map<std::string, double> samples;
        static size_t frames = 0;
        static double wall_ms = 0, cpu_ms = 0;
        const auto begin = profile ? std::chrono::steady_clock::now() : std::chrono::steady_clock::time_point{};
        static auto previous_begin = begin;
        const double frame_ms = profile ? std::chrono::duration<double, std::milli>(begin - previous_begin).count() : 0.;
        if (profile) previous_begin = begin;
        auto measure = [&](const ces_base_system_shared_ptr& system, const char* phase, const std::chrono::steady_clock::time_point& start) {
            if (profile && frames >= 60)
                samples[std::string(typeid(*system).name()) + "/" + phase] += std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count();
        };
        if(m_root)
        {
            // Batch query erasures, retaining the original final append order.
            // Changes still become visible only at the start of the next tick.
            {
                struct change { e_entity_state state; ces_entity_shared_ptr entity; };
                std::vector<change> changes;
                std::unordered_map<const ces_entity*,size_t> last_erase;
                while (!m_changed_entities.empty())
                {
                    const auto event=m_changed_entities.front();
                    m_changed_entities.pop();
                    if (auto entity=event.second.lock())
                    {
                        if (event.first==e_entity_state_changed || event.first==e_entity_state_removed)
                            last_erase[entity.get()]=changes.size();
                        changes.push_back({event.first,std::move(entity)});
                    }
                }
                if (!changes.empty())
                {
                    for (const auto& system:m_ordered_systems)
                    {
                        for (auto& required_mask:system->m_references_to_required_entities)
                        {
                            auto& references=required_mask.second;
                            if (!last_erase.empty())
                                references.erase(std::remove_if(references.begin(),references.end(),[&](const ces_entity_weak_ptr& weak) {
                                    const auto entity=weak.lock();
                                    return !entity || last_erase.count(entity.get())!=0;
                                }),references.end());
                            for(size_t index=0;index<changes.size();++index)
                            {
                                const auto& event=changes[index];
                                const auto erased=last_erase.find(event.entity.get());
                                if (erased!=last_erase.end() && index<erased->second) continue;
                                if ((event.state==e_entity_state_changed || event.state==e_entity_state_added) &&
                                    event.entity->is_components_exist(required_mask.first))
                                    references.push_back(event.entity);
                            }
                        }
                    }
                }
            }
            
            auto scene = std::static_pointer_cast<scene_graph>(m_root);
            
            for(const auto& system : m_ordered_systems)
            {
                system->set_current_camera_2d(scene->get_camera_2d());
                system->set_current_camera_3d(scene->get_camera_3d());
                if (system->can_be_feeded(m_root))
                {
                    const auto start = profile ? std::chrono::steady_clock::now() : begin;
                    system->on_feed_start(dt);
                    measure(system, "start", start);
                }
            }
            
            for(const auto& system : m_ordered_systems)
            {
                if (system->can_be_feeded(m_root))
                {
                    const auto start = profile ? std::chrono::steady_clock::now() : begin;
                    system->on_feed(m_root, dt);
                    measure(system, "feed", start);
                }
            }
            
            for(const auto& system : m_ordered_systems)
            {
                if (system->can_be_feeded(m_root))
                {
                    const auto start = profile ? std::chrono::steady_clock::now() : begin;
                    system->on_feed_end(dt);
                    measure(system, "end", start);
                }
            }
            
            for(const auto& system : m_ordered_systems)
            {
                system->cleanup(m_root);
            }
            
            for(const auto& system : m_ordered_systems)
            {
                system->cleanup_done(m_root);
            }
            if (profile) {
                if (frames >= 60) {
                    cpu_ms += std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - begin).count();
                    wall_ms += frame_ms;
                }
                ++frames;
                if (frames > 60 && (frames - 60) % 120 == 0) {
                    std::cout << "PROFILE wall_ms=" << wall_ms / 120 << " feeder_ms=" << cpu_ms / 120 << std::endl;
                    for (const auto& sample : samples) if (sample.second / 120 > .05)
                        std::cout << "PROFILE " << sample.first << " ms=" << sample.second / 120 << std::endl;
                    samples.clear();wall_ms = cpu_ms = 0;
                }
            }
        }
    }
    
    void ces_systems_feeder::add_system(const ces_base_system_shared_ptr& system)
    {
        m_systems[system->instance_guid()] = system;
        m_ordered_systems.push_back(system);
		m_ordered_systems.sort([](const ces_base_system_shared_ptr &value_01, const ces_base_system_shared_ptr &value_02) { 
			return value_01->get_order() < value_02->get_order(); 
		});
    }
    
    void ces_systems_feeder::remove_system(i32 type)
    {
        const auto& system = m_systems.find(type);
        if(system != m_systems.end())
        {
            const auto system_to_remove = system->second;
            m_systems.erase(system);
            const auto& iterator = std::find(m_ordered_systems.begin(), m_ordered_systems.end(), system_to_remove);
            if(iterator != m_ordered_systems.end())
            {
                m_ordered_systems.erase(iterator);
            }
            else
            {
                assert(false);
            }
        }
    }
    
    void ces_systems_feeder::set_root(const ces_entity_shared_ptr &entity)
    {
        m_root = entity;
    }
    
    void ces_systems_feeder::cleanup()
    {
        m_systems.clear();
        m_ordered_systems.clear();
        ces_systems_feeder::set_root(nullptr);
    }
    
    void ces_systems_feeder::on_entity_added(const ces_entity_shared_ptr& entity)
    {
         m_changed_entities.push(std::make_pair(e_entity_state_added, entity));
    }
    
    void ces_systems_feeder::on_entity_removed(const ces_entity_shared_ptr& entity)
    {
         m_changed_entities.push(std::make_pair(e_entity_state_removed, entity));
    }
    
    void ces_systems_feeder::on_entity_component_added(const ces_entity_shared_ptr& entity)
    {
        m_changed_entities.push(std::make_pair(e_entity_state_changed, entity));
    }
    
    void ces_systems_feeder::on_entity_component_removed(const ces_entity_shared_ptr& entity)
    {
         m_changed_entities.push(std::make_pair(e_entity_state_changed, entity));
    }
}
