//
//  database_entities_controllers.cpp
//  gb_database
//
//  Created by serhii serhiiv on 8/1/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "database_coordinator.h"
#include "database_connection.h"
#include "database_table.h"
#include "common.h"

namespace gb
{
    namespace db
    {
        database_coordinator::database_coordinator() :
        m_database(nullptr)
        {
            
        }
        
        database_coordinator::~database_coordinator()
        {
            if(m_database)
            {
                m_database->close();
            }
        }
        
        bool database_coordinator::open(const std::string& filename)
        {
            m_database.reset(new database_connection());
            bool opened = m_database->open(documentspath().append(filename));
            if (!opened)
            {
                m_database.reset();
            }
            return opened;
        }
        
        void database_coordinator::register_table(uint8_t guid, const database_table_shared_ptr& table)
        {
            m_tables.insert(std::make_pair(guid, table));
            table->construct();
        }
    }
}
