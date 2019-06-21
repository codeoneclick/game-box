//
//  ces_levels_database_component.h
//  gbDemo
//
//  Created by serhii.s on 6/10/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"
#include "db_declarations.h"
#include "level_configuration.h"

namespace game
{
    class ces_levels_database_component : public gb::ces_base_component
    {
    public:
        
        class level_dto
        {
        private:
            
            friend class ces_levels_database_component;
            
            gb::db::database_coordinator_weak_ptr m_database_coordinator;
            
            i32 m_id;
            i32 m_level_index = -1;
            bool m_is_openned = false;
            bool m_is_passed = false;
            i32 m_drift_time = 0;
            bool m_star_1_received = false;
            bool m_star_2_received = false;
            bool m_star_3_received = false;
            std::string m_scene_filename;
            i32 m_required_drift_time = 0;
            i32 m_session_time_in_seconds = 0;
            i32 m_ai_cars_count = 0;
            f32 m_complexity = 0.f;
            
        protected:
            
        public:
            
            level_dto(const gb::db::database_coordinator_shared_ptr& database_coordinator);
            ~level_dto() = default;
            
            i32 get_id() const;
            i32 get_level_index() const;
            bool get_is_openned() const;
            bool get_is_passed() const;
            i32 get_drift_time() const;
            
            bool get_is_star_1_received() const;
            bool get_is_star_2_received() const;
            bool get_is_star_3_received() const;
           
            std::string get_scene_filename() const;
            i32 get_required_drift_time() const;
            i32 get_session_time_in_seconds() const;
            i32 get_ai_cars_count() const;
            f32 get_complexity() const;
        };
        
        
    private:
        
        gb::db::database_coordinator_weak_ptr m_database_coordinator;
        std::unordered_map<ui32,  std::shared_ptr<gb::level_configuration>> m_levels_configurations;
        std::unordered_map<i32, std::shared_ptr<level_dto>> m_levels;
        i32 m_playing_level_id = 0;
        
    protected:
        
    public:
        
        STTI_CLASS_GUID(ces_levels_database_component, gb::ces_base_component::g_guids_container)
        ces_levels_database_component();
        ~ces_levels_database_component() = default;
        
        void set_database_coordinator(const gb::db::database_coordinator_shared_ptr& database_coordinator);
        
        bool is_level_exist(i32 level_id) const;
        void add_level(i32 level_id, const std::shared_ptr<gb::level_configuration>& level_configuration);
        
        std::unordered_map<i32, std::shared_ptr<level_dto>> get_all_levels();
        std::shared_ptr<level_dto> get_level(i32 level_id);
        
        void open_level(i32 level_id);
        void pass_level(i32 level_id);
        
        bool is_level_oppened(i32 level_id);
        bool is_level_passed(i32 level_id);
        
        bool get_is_star_received(i32 level_id, i32 star_index);
        void set_star_received(i32 level_id, i32 star_index);
        
        f32 get_drift_time(i32 level_id);
        void set_drift_time(i32 level_id, f32 drift_time);
        
        void set_playing_level_id(i32 level_id);
        i32 get_playing_level_id();
        
        i32 get_next_level_id();
    };
};

