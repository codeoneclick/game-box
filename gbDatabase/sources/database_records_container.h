//
//  database_records_container.h
//  gb_database
//
//  Created by serhii serhiiv on 7/31/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "db_declarations.h"

struct sqlite3_stmt;

namespace gb
{
    namespace db
    {
        class database_records_container
        {
        public:
            
            class record_iterator
            {
            protected:

                database_records_container_shared_ptr m_records_container;
                
            public:

                record_iterator(const database_records_container_shared_ptr& records_container);
                void operator++()
                {
                    m_records_container->move_to_next();
                };
                explicit operator bool() const
                {
                    return !m_records_container->is_at_end();
                };
                bool operator !() const
                {
                    return !(bool)*this;
                };
                database_records_container_shared_ptr operator*() const
                {
                    return m_records_container;
                };
                database_records_container_shared_ptr operator->() const
                {
                    return m_records_container;
                };
            };
            
        private:
            
            std::vector<database_record_shared_ptr> m_records;
            sqlite3_stmt* m_query = nullptr;
            i32 m_records_count = 0;
            i32 m_step_status = 0;
            
        protected:
            
            template<typename T, typename P>
            typename std::vector<T>::size_type get_index_with_predicate(const std::vector<T>& array, P predicate) const
            {
                const typename std::vector<T>::const_iterator position = std::find_if(array.begin(), array.end(), predicate);
                if(position != array.end())
                {
                    return position - array.begin();
                }
                return -1;
            };

            void move_to_first();
            void move_to_next();
            bool is_at_end() const;
            
        public:
            
            database_records_container(sqlite3_stmt*& statement);
            ~database_records_container();
            
            i32 get_records_count() const;
            
            std::string get_string(const std::string& record_name) const;
            i32 get_i32(const std::string& record_name) const;
            f32 get_f32(const std::string& record_name) const;
            i64 get_i64(const std::string& record_name) const;
            const char* get_blob(const std::string& record_name, int& size) const;
            
            std::vector<database_record_shared_ptr> get_records() const;
        };
    };
};
