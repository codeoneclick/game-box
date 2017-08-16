//
//  ces_quest_receiver_component.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 8/8/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_quest_receiver_component.h"
#include "db_character_quests_table.h"
#include "db_character_quest_tasks_table.h"
#include "database_entity.h"

namespace game
{
    ces_quest_receiver_component::quest_dto::task_dto::task_dto(const gb::db::database_coordinator_shared_ptr& database_coordinator) :
    m_database_coordinator(database_coordinator)
    {
        
    }
    
    ces_quest_receiver_component::quest_dto::task_dto::~task_dto()
    {
        
    }
    
    ces_quest_receiver_component::quest_dto::quest_dto(const gb::db::database_coordinator_shared_ptr& database_coordinator) :
    m_database_coordinator(database_coordinator)
    {
        
    }
    
    ces_quest_receiver_component::quest_dto::~quest_dto()
    {
        
    }
    
    ces_quest_receiver_component::ces_quest_receiver_component()
    {
        
    }
    
    void ces_quest_receiver_component::set_database_coordinator(const gb::db::database_coordinator_shared_ptr& database_coordinator)
    {
        m_database_coordinator = database_coordinator;
    }
    
    bool ces_quest_receiver_component::is_quest_exist(i32 id) const
    {
        auto quest_entity = std::make_shared<gb::db::database_entity<db_character_quests_table, db_character_quest_data>>(m_database_coordinator.lock());
        return quest_entity->load_from_db(id);
    }
    
    void ces_quest_receiver_component::add_to_questlog(i32 id, const std::shared_ptr<ces_quest_giver_component::quest> &quest_configuration)
    {
        auto quest_entity = std::make_shared<gb::db::database_entity<db_character_quests_table, db_character_quest_data>>(m_database_coordinator.lock());
        if(quest_entity->load_from_db(id))
        {
            assert(false);
        }
        else
        {
            for(const auto& quest_task : quest_configuration->get_quest_tasks())
            {
                auto quest_task_entity = std::make_shared<gb::db::database_entity<db_character_quest_tasks_table, db_character_quest_task_data>>(m_database_coordinator.lock());
                if(!quest_task_entity->load_from_db(quest_task->get_id()))
                {
                    auto& data = quest_task_entity->get_data();
                    data.m_id = quest_task->get_id();
                    data.m_quest_id = id;
                    data.m_type = quest_task->get_type();
                    data.m_goal_id = quest_task->get_goal_id();
                    data.m_max_goal_count = quest_task->get_goal_count();
                    data.m_max_goal_time = quest_task->get_goal_time();
                    data.m_current_goal_count = 0;
                    data.m_current_goal_time = 0.f;
                    quest_task_entity->save_to_db();
                }
            }
            auto& data = quest_entity->get_data();
            data.m_id = id;
            quest_entity->save_to_db();
        }
    }
    
    void ces_quest_receiver_component::remove_from_questlog(i32 id)
    {
        auto quest_entity = std::make_shared<gb::db::database_entity<db_character_quests_table, db_character_quest_data>>(m_database_coordinator.lock());
        if(quest_entity->load_from_db(id))
        {
            std::stringstream predicate;
            predicate<<"select * from "<<"quest_tasks";
            predicate<<" where quest_id=="<<id;
            auto quest_tasks_entities = gb::db::database_entity<db_character_quest_tasks_table, db_character_quest_task_data>::load_all_from_db(m_database_coordinator.lock(), predicate.str());
            for(const auto& quest_task_entity : quest_tasks_entities)
            {
                quest_task_entity->delete_from_db();
            }
            quest_entity->delete_from_db();
        }
        else
        {
            assert(false);
        }
    }
    
    std::unordered_map<i32, std::shared_ptr<ces_quest_receiver_component::quest_dto>> ces_quest_receiver_component::get_all_quests() const
    {
        std::unordered_map<i32, std::shared_ptr<ces_quest_receiver_component::quest_dto>> quests;
        auto quests_entities = gb::db::database_entity<db_character_quests_table, db_character_quest_data>::load_all_from_db(m_database_coordinator.lock());
        for(const auto& quest_entity : quests_entities)
        {
            auto quest_dto = std::make_shared<ces_quest_receiver_component::quest_dto>(m_database_coordinator.lock());
            quest_dto->m_id = quest_entity->get_data().m_id;
           
            std::stringstream predicate;
            predicate<<"select * from "<<"quest_tasks";
            predicate<<" where quest_id=="<<quest_dto->m_id;

            auto quest_tasks_entities = gb::db::database_entity<db_character_quest_tasks_table, db_character_quest_task_data>::load_all_from_db(m_database_coordinator.lock(), predicate.str());
            for(const auto& quest_task_entity : quest_tasks_entities)
            {
                auto quest_task_dto = std::make_shared<ces_quest_receiver_component::quest_dto::task_dto>(m_database_coordinator.lock());
                quest_task_dto->m_id = quest_task_entity->get_data().m_id;
                quest_dto->m_tasks.insert(std::make_pair(quest_task_dto->m_id, quest_task_dto));
            }
            quests.insert(std::make_pair(quest_dto->m_id, quest_dto));
        }
        return quests;
    }
}
