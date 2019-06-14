//
//  db_character_quests_table.h
//  gbDemo
//
//  Created by serhii serhiiv on 8/8/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "declarations.h"
#include "db_declarations.h"
#include "ns_declarations.h"
#include "database_table.h"

namespace game
{
    struct db_character_quest_data
    {
        i32 m_id;
    };
    
    class db_character_quests_table : public gb::db::database_table
    {
    private:
        
    protected:
        
    public:
        
        STTI_CLASS_GUID(db_character_quests_table, gb::db::database_table::g_guids_container, override)
        db_character_quests_table(const gb::db::database_connection_shared_ptr& database);
        
        void construct() override;
        bool save_to_db(i32 id, const db_character_quest_data& data);
    };
};

