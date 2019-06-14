//
//  db_helper.h
//  gbDemo
//
//  Created by serhii.s on 6/14/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "database_coordinator.h"
#include "db_garage_table.h"
#include "db_car_table.h"
#include "db_level_table.h"
#include "db_user_table.h"
#include "levels_set_configuration.h"

namespace game
{
    class db_helper
    {
        private:
        
        protected:
        
        public:
        
        static void fill_initial_values(const gb::ces_entity_shared_ptr& root, const gb::db::database_coordinator_shared_ptr& database_coordinator,
                                        const std::shared_ptr<gb::levels_set_configuration>& levels_set_configuration);
    };
};
