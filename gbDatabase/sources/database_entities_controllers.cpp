//
//  database_entities_controllers.cpp
//  gb_database
//
//  Created by serhii serhiiv on 8/1/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "database_entities_controllers.h"
#include "database_connection.h"
#include "common.h"

namespace gb
{
    namespace db
    {
        std::set<uintptr_t> database_entity_controller::g_guids_container;
        
        database_entity_controller::database_entity_controller(const database_connection_shared_ptr& database) :
        m_database(database)
        {
            
        }
        
        database_entities_controllers::database_entities_controllers() :
        m_database(nullptr)
        {
            
        }
        
        database_entities_controllers::~database_entities_controllers()
        {
            if(m_database)
            {
                m_database->close();
            }
        }
        
        bool database_entities_controllers::open(const std::string& filename)
        {
            m_database.reset(new database_connection());
            bool opened = m_database->open(documentspath().append(filename));
            if (!opened)
            {
                m_database.reset();
            }
            return opened;
        }
        
        void database_entities_controllers::register_entity_controller(uint8_t guid, const database_entity_controller_shared_ptr& controller)
        {
            m_entities_controllers.insert(std::make_pair(guid, controller));
            controller->construct();
        }
    }
}
