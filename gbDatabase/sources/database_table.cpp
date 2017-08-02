//
//  database_table.cpp
//  gb_database
//
//  Created by serhii serhiiv on 8/2/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "database_table.h"

namespace gb
{
    namespace db
    {
        std::set<uintptr_t> database_table::g_guids_container;
        
        database_table::database_table(const database_connection_shared_ptr& database) :
        m_database(database)
        {
            
        }
    }
}
