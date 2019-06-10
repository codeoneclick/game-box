//
//  db_car_table.hpp
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
    struct db_car_data
    {
        i32 m_id;
        i32 m_garage_id;
        i32 m_car_index;
        i32 m_is_openned;
        i32 m_car_skin_index;
    };
    
    class db_car_table : public gb::db::database_table
    {
        private:
        
        protected:
        
        public:
        
        CTTI_CLASS_GUID(db_car_table, gb::db::database_table::g_guids_container, override)
        db_car_table(const gb::db::database_connection_shared_ptr& database);
        
        void construct() override;
        bool save_to_db(i32 id, const db_car_data& data);
    };
};
