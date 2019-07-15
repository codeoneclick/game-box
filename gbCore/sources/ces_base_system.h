//
//  ces_base_system.h
//  gbCore
//
//  Created by Sergey Sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "ces_entity.h"

namespace gb
{
    class ces_base_system : public std::enable_shared_from_this<ces_base_system>
    {
    private:
        
        friend class ces_systems_feeder;
        bool m_is_paused = false;
        camera_2d_weak_ptr m_camera_2d;
        camera_3d_weak_ptr m_camera_3d;
        std::unordered_map<std::bitset<std::numeric_limits<uint8_t>::max()>, std::vector<ces_entity_weak_ptr>> m_references_to_required_entities;
        
    protected:
        
        static std::set<stti_guid_t> g_guids_container;
        
        virtual void on_feed_start(f32 dt) = 0;
        virtual void on_feed(const ces_entity_shared_ptr& root, f32 dt) = 0;
        virtual void on_feed_end(f32 dt) = 0;
        
        virtual bool can_be_feeded(const ces_entity_shared_ptr& root);
        virtual void cleanup(const ces_entity_shared_ptr& root);
        virtual void cleanup_done(const ces_entity_shared_ptr& root);
        
        void set_current_camera_2d(camera_2d_const_shared_ptr camera);
        camera_2d_shared_ptr get_current_camera_2d() const;
        
        void set_current_camera_3d(camera_3d_const_shared_ptr camera);
        camera_3d_shared_ptr get_current_camera_3d() const;
        
        ces_base_system();

		ui8 m_order;
        
        void add_required_component_guid(std::bitset<std::numeric_limits<uint8_t>::max()>&  mask, uint8_t guid);
        void add_required_components_mask(std::bitset<std::numeric_limits<uint8_t>::max()> mask);
        void enumerate_entities_with_components(const std::bitset<std::numeric_limits<uint8_t>::max()>& mask, const std::function<void(const ces_entity_shared_ptr& child)>& enumerator);
        
    public:
        
        STTI_CLASS_GUID(ces_base_system, ces_base_system::g_guids_container)
        virtual ~ces_base_system() = default;
        
        virtual void init();

		ui8 get_order() const;
		void set_order(ui8 value);
        
        void set_is_paused(bool value);
        bool get_is_paused() const;
    };
};
