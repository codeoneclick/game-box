//
//  db_level_table.h
//  gbDemo
//
//  Created by serhii.s on 6/10/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "declarations.h"
#include "db_declarations.h"
#include "ns_declarations.h"
#include "database_table.h"

namespace game
{
    struct db_level_data
    {
        i32 m_id;
        i32 m_level_index;
        i32 m_is_openned;
        i32 m_is_passed;
        i32 m_drift_time;
        i32 m_stars_count;
    };
    
    class db_level_table : public gb::db::database_table
    {
    private:
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(db_level_table, gb::db::database_table::g_guids_container, override)
        db_level_table(const gb::db::database_connection_shared_ptr& database);
        
        void construct() override;
        bool save_to_db(i32 id, const db_level_data& data);
    };
};

