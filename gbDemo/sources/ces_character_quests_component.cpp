//
//  ces_character_quests_component.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 8/8/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_character_quests_component.h"
#include "db_character_quests_table.h"
#include "db_character_quest_tasks_table.h"
#include "database_entity.h"

namespace game
{
    ces_character_quests_component::ces_character_quests_component()
    {
        
    }
    
    void ces_character_quests_component::set_database_coordinator(const gb::db::database_coordinator_shared_ptr& database_coordinator)
    {
        m_database_coordinator = database_coordinator;
    }
    
    bool ces_character_quests_component::is_quest_exist(i32 id) const
    {
        auto character_quest_entity = std::make_shared<gb::db::database_entity<db_character_quests_table, db_character_quest_entity>>(m_database_coordinator.lock());
        return character_quest_entity->load_from_db(id);
    }
    
    void ces_character_quests_component::add_to_quest_log(i32 id)
    {
        auto character_quest_entity = std::make_shared<gb::db::database_entity<db_character_quests_table, db_character_quest_entity>>(m_database_coordinator.lock());
        if(character_quest_entity->load_from_db(id))
        {
            assert(false);
        }
        else
        {
            auto& data = character_quest_entity->get_data();
            data.m_id = id;
            character_quest_entity->save_to_db();
        }
    }
}
