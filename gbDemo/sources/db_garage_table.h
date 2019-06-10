//
//  db_garage_table.hpp
//  gbDemo
//
//  Created by serhii.s on 5/16/19.
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
    struct db_garage_data
    {
        i32 m_id;
        i32 m_selected_car_id;
    };
    
    class db_garage_table : public gb::db::database_table
    {
        private:
        
        protected:
        
        public:
        
        CTTI_CLASS_GUID(db_garage_table, gb::db::database_table::g_guids_container, override)
        db_garage_table(const gb::db::database_connection_shared_ptr& database);
        
        void construct() override;
        bool save_to_db(i32 id, const db_garage_data& data);
    };
};
