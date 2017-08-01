//
//  db_character_entity.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 8/1/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "db_character_entity.h"
#include "db_character_entity_controller.h"

namespace game
{
    
    db_character_entity::db_character_entity(const gb::db::database_entities_controllers_shared_ptr& database_entities_controllers) :
    m_database_entities_controllers(database_entities_controllers)
    {
        
    }
    
    db_character_entity::~db_character_entity()
    {
        
    }
    
    void db_character_entity::save_to_db()
    {
        auto character_entities_controller = m_database_entities_controllers->get_entity_controller<db_character_entity_controller>();
        character_entities_controller->save_to_db(shared_from_this());
    }
    
    void db_character_entity::load_from_db(i32 id)
    {
        m_id = id;
        auto character_entities_controller = m_database_entities_controllers->get_entity_controller<db_character_entity_controller>();
        character_entities_controller->load_from_db(shared_from_this());
    }
}
