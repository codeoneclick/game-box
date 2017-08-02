//
//  db_characters_table.h
//  gbDemo
//
//  Created by serhii serhiiv on 8/1/17.
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
    class db_characters_table : public gb::db::database_table
    {
    private:
    
    protected:
        
    public:
        
        CTTI_CLASS_GUID(db_characters_table, db_characters_table::g_guids_container, override)
        db_characters_table(const gb::db::database_connection_shared_ptr& database);
        
        void construct() override;
        
        bool load_from_db(i32 id, char* raw_data, i32& size, bool all = false, const read_data_callback_t& callback = nullptr) override;
        bool save_to_db(i32 id, const char* raw_data, i32 size) override;
    };
};
