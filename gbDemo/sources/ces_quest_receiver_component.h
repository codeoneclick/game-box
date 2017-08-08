//
//  ces_quest_receiver_component.h
//  gbDemo
//
//  Created by serhii serhiiv on 8/8/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"
#include "db_declarations.h"
#include "ces_quest_giver_component.h"

namespace game
{
    class ces_quest_receiver_component : public gb::ces_base_component
    {
    private:
        
        gb::db::database_coordinator_weak_ptr m_database_coordinator;
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_quest_receiver_component, gb::ces_base_component::g_guids_container)
        ces_quest_receiver_component();
        ~ces_quest_receiver_component() = default;
        
        void set_database_coordinator(const gb::db::database_coordinator_shared_ptr& database_coordinator);
        
        bool is_quest_exist(i32 id) const;
        
        void add_to_quest_log(i32 id, const std::shared_ptr<ces_quest_giver_component::quest> &quest_configuration);
    };
};

