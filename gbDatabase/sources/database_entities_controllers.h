//
//  database_entities_controllers.h
//  gb_database
//
//  Created by serhii serhiiv on 8/1/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "db_declarations.h"

namespace gb
{
    namespace db
    {
        class database_entity_controller
        {
        private:
            
        protected:
            
            database_connection_shared_ptr m_database;
            static std::set<uintptr_t> g_guids_container;
            
        public:
            
            CTTI_CLASS_GUID(database_entity_controller, database_entity_controller::g_guids_container)
            database_entity_controller(const database_connection_shared_ptr& database);
            virtual void construct() = 0;
        };
        
        class database_entities_controllers
        {
        protected:
            
            database_connection_shared_ptr m_database;
            std::unordered_map<uint8_t, database_entity_controller_shared_ptr> m_entities_controllers;
            void register_entity_controller(uint8_t guid, const database_entity_controller_shared_ptr& controller);
            
        public:
            
            database_entities_controllers();
            ~database_entities_controllers();
            
            bool open(const std::string& filename);
            
            template<class T>
            void register_entity_controller()
            {
                std::shared_ptr<T> entity_controller = std::make_shared<T>(m_database);
                register_entity_controller(T::class_guid(), entity_controller);
            }
            
            template<class T>
            std::shared_ptr<T> get_entity_controller() const
            {
                auto it = m_entities_controllers.find(T::class_guid());
                assert(it != m_entities_controllers.end());
                return std::static_pointer_cast<T>(it->second);
            };
        };
    };
};
