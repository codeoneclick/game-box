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
                                        const std::shared_ptr<gb::levels_set_configuration>& levels_set_configuration,
                                        const std::shared_ptr<gb::cars_progression_configuration>& cars_progression_configuration)
    {
        const auto user_database_component = std::make_shared<game::ces_user_database_component>();
        user_database_component->set_database_coordinator(database_coordinator);
        root->add_component(user_database_component);
        
        user_database_component->add_user(1);
        
        const auto cars_progression = cars_progression_configuration->get_cars_progression_configuration();
        for (auto car_progression_it : cars_progression)
        {
            const auto cars_progression = std::static_pointer_cast<gb::car_progression_configuration>(car_progression_it);
            user_database_component->add_car_progression(cars_progression->get_car_id(), cars_progression);
        }
        user_database_component->update_rank_according_stars_count(1);
        
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

        for (auto car_progression_it : cars_progression)
        {
            const auto cars_progression = std::static_pointer_cast<gb::car_progression_configuration>(car_progression_it);
            garage_database_component->add_car_progression(cars_progression->get_car_id(), cars_progression);
            if (!garage_database_component->is_car_exist(1, cars_progression->get_car_id()))
            {
                garage_database_component->add_car_to_garage(1, cars_progression->get_car_id());
            }
        }
        garage_database_component->update_cars_according_rank(1, user_database_component->get_rank(1));
    }
}
