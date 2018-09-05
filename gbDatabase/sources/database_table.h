//
//  database_table.h
//  gb_database
//
//  Created by serhii serhiiv on 8/2/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "db_declarations.h"
#include "database_records_container.h"

namespace gb
{
    namespace db
    {
        class database_table
        {
        public:
            
            typedef std::function<void(i32, char*, i32)> read_data_callback_t;
            typedef std::function<void(const gb::db::database_records_container::record_iterator&)> read_record_callback_t;
            
        private:
            
        protected:
            
            database_connection_shared_ptr m_database;
            static std::set<guid_t> g_guids_container;
            std::string m_name;
            
        public:
            
            CTTI_CLASS_GUID(database_table, database_table::g_guids_container)
            database_table(const database_connection_shared_ptr& database, const std::string& name);
            
            virtual void construct() = 0;
            
            virtual bool load_from_db(i32 id, const read_data_callback_t& callback);
            virtual bool load_all_from_db(const read_data_callback_t& callback);
            virtual bool load_from_db_with_custom_predicate(const std::string& predicate, const read_data_callback_t& callback);
            virtual bool load_from_db_with_custom_predicate(const std::string& predicate, const read_record_callback_t& callback);
            virtual bool delete_from_db(i32 id);
        };
    };
};
