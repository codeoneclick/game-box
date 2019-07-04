//
//  db_user_table.hpp
//  gbDemo
//
//  Created by serhii.s on 6/14/19.
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
    struct db_user_data
    {
        i32 m_id;
        i32 m_tickets;
        i32 m_last_ticket_dec_timestamp;
        i32 m_rank;
        i32 m_claimed_rank;
        i32 m_stars_collected;
        i32 m_is_purchased_no_ads;
    };
    
    class db_user_table : public gb::db::database_table
    {
    private:
        
    protected:
        
    public:
        
        STTI_CLASS_GUID(db_user_table, gb::db::database_table::g_guids_container, override)
        db_user_table(const gb::db::database_connection_shared_ptr& database);
        
        void construct() override;
        bool save_to_db(i32 id, const db_user_data& data);
    };
};
