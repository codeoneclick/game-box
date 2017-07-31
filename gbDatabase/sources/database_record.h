//
//  database_record.h
//  gb_database
//
//  Created by serhii serhiiv on 7/31/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"

namespace gb
{
    namespace db
    {
        class database_record
        {
        public:
            
            enum e_database_record_type
            {
                e_database_record_type_unknown = 0,
                e_database_record_type_float,
                e_database_record_type_int,
                e_database_record_type_string,
                e_database_record_type_blob
            };
            
        private:
            
        protected:
            
            std::string m_name;
            e_database_record_type m_type;
            
        public:
            
            database_record();
            ~database_record();
            
            bool operator==(const database_record& copy) const
            {
                return (m_name == copy.m_name) && (m_type == copy.m_type);
            }
            
            void set_name(const std::string& name);
            std::string get_name() const;
            
            void set_type(e_database_record_type type);
            e_database_record_type get_type() const;
        };
    };
};
