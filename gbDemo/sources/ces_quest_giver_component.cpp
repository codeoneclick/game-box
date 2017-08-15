//
//  ces_quest_giver_component.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 8/8/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_quest_giver_component.h"

namespace game
{
    ces_quest_giver_component::quest::quest(const std::shared_ptr<gb::quest_configuration>& quest_configuration,
                                            const std::vector<std::shared_ptr<gb::quest_dependency_configuration>>& quest_dependencies_configuration,
                                            const std::vector<std::shared_ptr<gb::quest_task_configuration>>& quest_tasks_configuration) :
    m_quest_configuration(quest_configuration),
    m_quest_dependencies_configuration(quest_dependencies_configuration),
    m_quest_tasks_configuration(quest_tasks_configuration)
    {
        
    }
    
    std::vector<std::shared_ptr<gb::quest_task_configuration>>& ces_quest_giver_component::quest::get_quest_tasks()
    {
        return m_quest_tasks_configuration;
    }
    
    std::string ces_quest_giver_component::quest::convert_quest_type_to_string(e_quest_type type)
    {
        switch(type)
        {
            case e_quest_type_kill:
            {
                return "Kill";
            }
                break;
            case e_quest_type_collect:
            {
                return "Collect";
            }
                break;
            case e_quest_type_research:
            {
                return "Research";
            }
                break;
        }
        return "";
    }
    
    ces_quest_giver_component::ces_quest_giver_component()
    {
        
    }
    
    void ces_quest_giver_component::add_quest(const std::shared_ptr<gb::quest_configuration>& quest_configuration,
                                              const std::vector<std::shared_ptr<gb::quest_dependency_configuration>>& quest_dependencies_configuration,
                                              const std::vector<std::shared_ptr<gb::quest_task_configuration>>& quest_tasks_configuration)
    {
        auto quest = std::make_shared<ces_quest_giver_component::quest>(quest_configuration,
                                                                        quest_dependencies_configuration,
                                                                        quest_tasks_configuration);
        m_quests.insert(std::make_pair(quest_configuration->get_id(), quest));
    }
    
    bool ces_quest_giver_component::is_quests_exist() const
    {
        return m_quests.size() != 0;
    }
    
    bool ces_quest_giver_component::is_quest_exist(i32 id) const
    {
        return m_quests.find(id) != m_quests.end();
    }
    
    std::vector<i32> ces_quest_giver_component::get_all_quests_ids() const
    {
        std::vector<i32> quests_ids;
        for(const auto& it : m_quests)
        {
            quests_ids.push_back(it.first);
        }
        return quests_ids;
    }
    
    std::shared_ptr<ces_quest_giver_component::quest> ces_quest_giver_component::get_quest(i32 id) const
    {
        std::shared_ptr<ces_quest_giver_component::quest> quest = nullptr;
        const auto& it = m_quests.find(id);
        if(it != m_quests.end())
        {
            quest = it->second;
        }
        return quest;
    }
    
}
