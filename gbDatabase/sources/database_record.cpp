//
//  database_record.cpp
//  gb_database
//
//  Created by serhii serhiiv on 7/31/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "database_record.h"

namespace gb
{
    namespace db
    {
        database_record::database_record() :
        m_type(e_database_record_type_unknown)
        {
            
        }
        
        database_record::~database_record()
        {
            
        }
        
        void  database_record::set_name(const std::string& name)
        {
            m_name = name;
        }
        
        std::string database_record::get_name() const
        {
            return m_name;
        }
        
        void database_record::set_type(e_database_record_type type)
        {
            m_type = type;
        }
        
        database_record::e_database_record_type database_record::get_type() const
        {
            return m_type;
        }
    }
}
