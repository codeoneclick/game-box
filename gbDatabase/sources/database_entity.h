//
//  database_entity.h
//  gb_database
//
//  Created by serhii serhiiv on 8/2/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "declarations.h"
#include "db_declarations.h"
#include "database_table.h"
#include "database_coordinator.h"

namespace gb
{
    namespace db
    {
        template<typename table_t, typename data_t>
        class database_entity : public std::enable_shared_from_this<database_entity<table_t, data_t>>
        {
        public:
            
        private:
            
            friend class database_table;
            
        protected:
            
            i32 m_id = -1;
            data_t m_data;
            database_coordinator_shared_ptr m_database_coordinator;
            
        public:
            
            database_entity(const database_coordinator_shared_ptr& database_coordinator) :
            m_database_coordinator(database_coordinator)
            {
                
            }
            ~database_entity() = default;
            
            data_t& get_data()
            {
                return m_data;
            }
            
            void save_to_db()
            {
                auto table = m_database_coordinator->get_table<table_t>();
                i32 size = sizeof(data_t);
                char raw_data[size];
                memcpy(raw_data, &m_data, size);
                table->save_to_db(m_id, raw_data, size);
            }
            
            bool load_from_db(i32 id)
            {
                m_id = id;
                auto table = m_database_coordinator->get_table<table_t>();
                i32 size = sizeof(data_t);
                char raw_data[size];
                bool result = table->load_from_db(m_id, raw_data, size);
                if(result)
                {
                    data_t* data_ptr = (data_t*)raw_data;
                    memcpy(&m_data, data_ptr, size);
                }
                return result;
            }
            
            static std::vector<std::shared_ptr<database_entity<table_t, data_t>>> load_all_from_db(const database_coordinator_shared_ptr& database_coordinator)
            {
                std::vector<std::shared_ptr<database_entity<table_t, data_t>>> entities;
                auto table = database_coordinator->get_table<table_t>();
                i32 size = sizeof(data_t);
                char raw_data[size];
                table->load_from_db(-1, raw_data, size, true, [&entities, database_coordinator](i32 id, char* raw_data, i32 size) {
                    std::shared_ptr<database_entity<table_t, data_t>> entity = std::make_shared<database_entity<table_t, data_t>>(database_coordinator);
                    entity->m_id = id;
                    data_t* data_ptr = (data_t*)raw_data;
                    memcpy(&entity->m_data, data_ptr, size);
                    entities.push_back(entity);
                });
                return entities;
            }
        };
    };
};