//
//  ces_npc_component.h
//  gbDemo
//
//  Created by serhii serhiiv on 8/8/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"
#include "quest_configuration.h"

namespace game
{
    class ces_npc_component : public gb::ces_base_component
    {
    public:
        
        class quest
        {
        private:
            
            std::shared_ptr<gb::quest_configuration> m_quest_configuration;
            std::vector<std::shared_ptr<gb::quest_dependency_configuration>> m_quest_dependencies_configuration;
            std::vector<std::shared_ptr<gb::quest_task_configuration>> m_quest_tasks_configuration;
            
        protected:
            
        public:
            
            quest(const std::shared_ptr<gb::quest_configuration>& quest_configuration,
                  const std::vector<std::shared_ptr<gb::quest_dependency_configuration>>& quest_dependencies_configuration,
                  const std::vector<std::shared_ptr<gb::quest_task_configuration>>& quest_tasks_configuration);
            ~quest() = default;
            
            
        };
        
    private:
        
        std::unordered_map<i32, std::shared_ptr<quest>> m_quests;
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_npc_component, gb::ces_base_component::g_guids_container)
        ces_npc_component();
        ~ces_npc_component() = default;
        
        void add_quest(const std::shared_ptr<gb::quest_configuration>& quest_configuration,
                       const std::vector<std::shared_ptr<gb::quest_dependency_configuration>>& quest_dependencies_configuration,
                       const std::vector<std::shared_ptr<gb::quest_task_configuration>>& quest_tasks_configuration);
        
        bool is_quests_exist() const;
        bool is_quest_exist(i32 id) const;
        std::vector<i32> get_all_quests_ids() const;
        std::shared_ptr<quest> get_quest(i32 id) const;
    };
};
