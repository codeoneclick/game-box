//
//  database_coordinator.h
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
        class database_coordinator
        {
        protected:
            
            database_connection_shared_ptr m_database;
            std::unordered_map<uint8_t, database_table_shared_ptr> m_tables;
            void register_table(uint8_t guid, const database_table_shared_ptr& table);
            
        public:
            
            database_coordinator();
            ~database_coordinator();
            
            bool open(const std::string& filename);
            
            template<typename T>
            void register_table()
            {
                std::shared_ptr<T> table = std::make_shared<T>(m_database);
                register_table(T::class_guid(), table);
            }
            
            template<typename T>
            std::shared_ptr<T> get_table() const
            {
                auto it = m_tables.find(T::class_guid());
                assert(it != m_tables.end());
                return std::static_pointer_cast<T>(it->second);
            };
        };
    };
};
