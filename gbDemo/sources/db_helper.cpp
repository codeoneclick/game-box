//
//  db_helper.cpp
//  gbDemo
//
//  Created by serhii.s on 6/14/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "db_helper.h"
#include "ces_entity.h"
#include "ces_garage_database_component.h"
#include "ces_levels_database_component.h"
#include "ces_user_database_component.h"

namespace game
{
    void db_helper::fill_initial_values(const gb::ces_entity_shared_ptr& root, const gb::db::database_coordinator_shared_ptr& database_coordinator,
                                        const std::shared_ptr<gb::levels_set_configuration>& levels_set_configuration)
    {
        const auto user_database_component = std::make_shared<game::ces_user_database_component>();
        user_database_component->set_database_coordinator(database_coordinator);
        root->add_component(user_database_component);
        
        user_database_component->add_user(1);
        
        const auto levels_database_component = std::make_shared<game::ces_levels_database_component>();
        levels_database_component->set_database_coordinator(database_coordinator);
        root->add_component(levels_database_component);
        
        ui32 level_index = 1;
        const auto levels = levels_set_configuration->get_levels_configuration();
        for (auto level_configuration_it : levels)
        {
            const auto level_configuration = std::static_pointer_cast<gb::level_configuration>(level_configuration_it);
            levels_database_component->add_level(level_index, level_configuration);
            level_index++;
        }
        levels_database_component->open_level(1);
        
        const auto garage_database_component = std::make_shared<ces_garage_database_component>();
        garage_database_component->set_database_coordinator(database_coordinator);
        root->add_component(garage_database_component);
        
        if (!garage_database_component->is_garage_exist(1))
        {
            garage_database_component->add_garage(1);
        }
        
        if (!garage_database_component->is_car_exist(1, 1))
        {
            garage_database_component->add_car_to_garage(1, 1);
            garage_database_component->open_car(1, 1);
            garage_database_component->select_car(1, 1);
        }
        
        if (!garage_database_component->is_car_exist(1, 2))
        {
            garage_database_component->add_car_to_garage(1, 2);
        }
        
        if (!garage_database_component->is_car_exist(1, 3))
        {
            garage_database_component->add_car_to_garage(1, 3);
        }
        
        if (!garage_database_component->is_car_exist(1, 4))
        {
            garage_database_component->add_car_to_garage(1, 4);
        }
        
        if (!garage_database_component->is_car_exist(1, 5))
        {
            garage_database_component->add_car_to_garage(1, 5);
        }
        
        if (!garage_database_component->is_car_exist(1, 6))
        {
            garage_database_component->add_car_to_garage(1, 6);
        }
        
        if (!garage_database_component->is_car_exist(1, 7))
        {
            garage_database_component->add_car_to_garage(1, 7);
        }
        
        if (!garage_database_component->is_car_exist(1, 8))
        {
            garage_database_component->add_car_to_garage(1, 8);
        }
    }
}
