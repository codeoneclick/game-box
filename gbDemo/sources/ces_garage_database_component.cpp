//
//  ces_garage_database_component.cpp
//  gbDemo
//
//  Created by serhii.s on 5/16/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_garage_database_component.h"
#include "db_garage_table.h"
#include "db_car_table.h"
#include "database_entity.h"

namespace game
{
    static std::array<i32, 8> g_cars_prices = {1000, 3000, 10000, 20000, 25000, 30000, 40000, 50000};
    
    ces_garage_database_component::garage_dto::car_dto::car_dto(const gb::db::database_coordinator_shared_ptr& database_coordinator) :
    m_database_coordinator(database_coordinator)
    {
        
    }
    
    i32 ces_garage_database_component::garage_dto::car_dto::get_id() const
    {
        return m_id;
    }
    
    bool ces_garage_database_component::garage_dto::car_dto::get_is_openned() const
    {
        return m_is_openned != 0;
    }
    
    bool ces_garage_database_component::garage_dto::car_dto::get_is_bought() const
    {
        return m_is_bought != 0;
    }
    
    i32 ces_garage_database_component::garage_dto::car_dto::get_price() const
    {
        return m_price;
    }
    
    i32 ces_garage_database_component::garage_dto::car_dto::get_car_body_color_id() const
    {
        return m_car_body_color_id;
    }
    
    i32 ces_garage_database_component::garage_dto::car_dto::get_car_windows_color_id() const
    {
        return m_car_windows_color_id;
    }
    
    f32 ces_garage_database_component::garage_dto::car_dto::get_car_speed_upgrade() const
    {
        f32 result = 0.f;
        result = static_cast<f32>(glm::clamp(m_car_speed_upgrade, 0, 100)) / 100.f;
        return result;
    }
    
    f32 ces_garage_database_component::garage_dto::car_dto::get_car_handling_upgrade() const
    {
        f32 result = 0.f;
        result = static_cast<f32>(glm::clamp(m_car_handling_upgrade, 0, 100)) / 100.f;
        return result;
    }
    
    f32 ces_garage_database_component::garage_dto::car_dto::get_car_rigidity_upgrade() const
    {
        f32 result = 0.f;
        result = static_cast<f32>(glm::clamp(m_car_rigidity_upgrade, 0, 100)) / 100.f;
        return result;
    }
    
    ces_garage_database_component::garage_dto::garage_dto(const gb::db::database_coordinator_shared_ptr& database_coordinator) :
    m_database_coordinator(database_coordinator)
    {
        
    }
    
    ces_garage_database_component::ces_garage_database_component()
    {
        
    }
    
    void ces_garage_database_component::set_database_coordinator(const gb::db::database_coordinator_shared_ptr& database_coordinator)
    {
        m_database_coordinator = database_coordinator;
    }
    
    bool ces_garage_database_component::is_garage_exist(i32 garage_id) const
    {
        auto garage_record = std::make_shared<gb::db::database_entity<db_garage_table, db_garage_data>>(m_database_coordinator.lock());
        return garage_record->load_from_db(garage_id);
    }
    
    void ces_garage_database_component::add_garage(i32 garage_id)
    {
        auto garage_record = std::make_shared<gb::db::database_entity<db_garage_table, db_garage_data>>(m_database_coordinator.lock());
        if(!garage_record->load_from_db(garage_id))
        {
            auto& data = garage_record->get_data();
            data.m_id = garage_id;
            data.m_selected_car_id = 1;
            garage_record->save_to_db();
        }
    }
    
    bool ces_garage_database_component::is_car_exist(i32 garage_id, i32 car_id) const
    {
        auto garager_record = std::make_shared<gb::db::database_entity<db_garage_table, db_garage_data>>(m_database_coordinator.lock());
        if(!garager_record->load_from_db(garage_id))
        {
            assert(false);
        }
        else
        {
            auto car_record = std::make_shared<gb::db::database_entity<db_car_table, db_car_data>>(m_database_coordinator.lock());
            if(car_record->load_from_db(car_id))
            {
                auto& data = car_record->get_data();
                return data.m_garage_id == garage_id;
            }
        }
        
        return false;
    }
    
    void ces_garage_database_component::add_car_to_garage(i32 garage_id, i32 car_id)
    {
        auto garager_record = std::make_shared<gb::db::database_entity<db_garage_table, db_garage_data>>(m_database_coordinator.lock());
        if(!garager_record->load_from_db(garage_id))
        {
            assert(false);
        }
        else
        {
            auto car_record = std::make_shared<gb::db::database_entity<db_car_table, db_car_data>>(m_database_coordinator.lock());
            if(!car_record->load_from_db(car_id))
            {
                std::vector<i32> possible_colors = {1, 2, 3, 4};
                i32 upgrade_points = 50;
                auto& data = car_record->get_data();
                data.m_id = car_id;
                data.m_garage_id = garage_id;
                data.m_is_openned = 0;
                data.m_is_bought = 0;
                data.m_price = g_cars_prices.at(car_id - 1);
                data.m_car_body_color_id = possible_colors.at(std::get_random_i(0, 3));
                possible_colors[data.m_car_body_color_id - 1] = possible_colors.at((data.m_car_body_color_id + 1) % possible_colors.size());
                data.m_car_windows_color_id = possible_colors.at(std::get_random_i(0, 3));
                i32 car_speed_upgrade = std::get_random_i(0, upgrade_points);
                car_speed_upgrade -= car_speed_upgrade % 10;
                data.m_car_speed_upgrade = car_speed_upgrade;
                upgrade_points -= car_speed_upgrade;
                i32 car_handling_upgrade = std::get_random_i(0, upgrade_points);
                car_handling_upgrade -= car_handling_upgrade % 10;
                data.m_car_handling_upgrade = car_handling_upgrade;
                upgrade_points -= car_handling_upgrade;
                data.m_car_rigidity_upgrade = upgrade_points;
                data.m_car_skin_id = 1;
                car_record->save_to_db();
            }
        }
    }
    
    void ces_garage_database_component::select_car(i32 garage_id, i32 car_id)
    {
        auto garager_record = std::make_shared<gb::db::database_entity<db_garage_table, db_garage_data>>(m_database_coordinator.lock());
        if(!garager_record->load_from_db(garage_id))
        {
            assert(false);
        }
        else
        {
            auto& data = garager_record->get_data();
            data.m_selected_car_id = car_id;
            garager_record->save_to_db();
        }
    }
    
    void ces_garage_database_component::select_car_skin(i32 garage_id, i32 car_id, i32 car_skin_id)
    {
        auto garager_record = std::make_shared<gb::db::database_entity<db_garage_table, db_garage_data>>(m_database_coordinator.lock());
        if(!garager_record->load_from_db(garage_id))
        {
            assert(false);
        }
        else
        {
            auto car_record = std::make_shared<gb::db::database_entity<db_car_table, db_car_data>>(m_database_coordinator.lock());
            if(!car_record->load_from_db(car_id))
            {
                assert(false);
            }
            else
            {
                auto& data = car_record->get_data();
                data.m_car_skin_id = car_skin_id;
                car_record->save_to_db();
            }
        }
    }
    
    std::shared_ptr<ces_garage_database_component::garage_dto::car_dto> ces_garage_database_component::get_selected_car(i32 garage_id)
    {
        std::shared_ptr<ces_garage_database_component::garage_dto::car_dto> result = nullptr;
        auto garager_record = std::make_shared<gb::db::database_entity<db_garage_table, db_garage_data>>(m_database_coordinator.lock());
        if(!garager_record->load_from_db(garage_id))
        {
            assert(false);
        }
        else
        {
            auto& data = garager_record->get_data();
            i32 selected_car_id = data.m_selected_car_id;
            auto car_record = std::make_shared<gb::db::database_entity<db_car_table, db_car_data>>(m_database_coordinator.lock());
            if(!car_record->load_from_db(selected_car_id))
            {
                assert(false);
            }
            else
            {
                auto car_dto = std::make_shared<ces_garage_database_component::garage_dto::car_dto>(m_database_coordinator.lock());
                car_dto->m_id = car_record->get_data().m_id;
                car_dto->m_garage_id = car_record->get_data().m_garage_id;
                car_dto->m_is_openned = car_record->get_data().m_is_openned;
                car_dto->m_is_bought = car_record->get_data().m_is_bought;
                car_dto->m_price = car_record->get_data().m_price;
                car_dto->m_car_body_color_id = car_record->get_data().m_car_body_color_id;
                car_dto->m_car_windows_color_id = car_record->get_data().m_car_windows_color_id;
                car_dto->m_car_speed_upgrade = car_record->get_data().m_car_speed_upgrade;
                car_dto->m_car_handling_upgrade = car_record->get_data().m_car_handling_upgrade;
                car_dto->m_car_rigidity_upgrade = car_record->get_data().m_car_rigidity_upgrade;
                result = car_dto;
            }
        }
        return result;
    }
    
    void ces_garage_database_component::open_car(i32 garage_id, i32 car_id)
    {
        auto garage_record = std::make_shared<gb::db::database_entity<db_garage_table, db_garage_data>>(m_database_coordinator.lock());
        if(!garage_record->load_from_db(garage_id))
        {
            assert(false);
        }
        else
        {
            auto car_record = std::make_shared<gb::db::database_entity<db_car_table, db_car_data>>(m_database_coordinator.lock());
            if(!car_record->load_from_db(car_id))
            {
                assert(false);
            }
            else
            {
                auto& data = car_record->get_data();
                data.m_is_openned = 1;
                car_record->save_to_db();
            }
        }
    }
    
    void ces_garage_database_component::buy_car(i32 garage_id, i32 car_id)
    {
        auto garage_record = std::make_shared<gb::db::database_entity<db_garage_table, db_garage_data>>(m_database_coordinator.lock());
        if(!garage_record->load_from_db(garage_id))
        {
            assert(false);
        }
        else
        {
            auto car_record = std::make_shared<gb::db::database_entity<db_car_table, db_car_data>>(m_database_coordinator.lock());
            if(!car_record->load_from_db(car_id))
            {
                assert(false);
            }
            else
            {
                auto& data = car_record->get_data();
                data.m_is_bought = 1;
                car_record->save_to_db();
            }
        }
    }
    
    void ces_garage_database_component::close_car(i32 garage_id, i32 car_id)
    {
        auto garager_record = std::make_shared<gb::db::database_entity<db_garage_table, db_garage_data>>(m_database_coordinator.lock());
        if(!garager_record->load_from_db(garage_id))
        {
            assert(false);
        }
        else
        {
            auto car_record = std::make_shared<gb::db::database_entity<db_car_table, db_car_data>>(m_database_coordinator.lock());
            if(!car_record->load_from_db(car_id))
            {
                assert(false);
            }
            else
            {
                auto& data = car_record->get_data();
                data.m_is_openned = 0;
                car_record->save_to_db();
            }
        }
    }
    
    bool ces_garage_database_component::is_car_oppenned(i32 garage_id, i32 car_id) const
    {
        bool result = false;
        auto garager_record = std::make_shared<gb::db::database_entity<db_garage_table, db_garage_data>>(m_database_coordinator.lock());
        if(!garager_record->load_from_db(garage_id))
        {
            assert(false);
        }
        else
        {
            auto car_record = std::make_shared<gb::db::database_entity<db_car_table, db_car_data>>(m_database_coordinator.lock());
            if(!car_record->load_from_db(car_id))
            {
                assert(false);
            }
            else
            {
                result = car_record->get_data().m_is_openned != 0;
            }
        }
        return result;
    }
    
    i32 ces_garage_database_component::get_previewed_car_id() const
    {
        return m_previewed_car_id;
    }
    
    void ces_garage_database_component::set_previewed_car_id(i32 id)
    {
        m_previewed_car_id = id;
    }
    
    i32 ces_garage_database_component::get_max_cars_count() const
    {
        return m_max_cars_count;
    }
    
    void ces_garage_database_component::add_car_progression(i32 car_id, const std::shared_ptr<gb::car_progression_configuration>& car_progression_configuration)
    {
        m_cars_progression_configurations[car_id] = car_progression_configuration;
    }
    
    void ces_garage_database_component::update_cars_according_rank(i32 garage_id, i32 rank)
    {
        for (const auto& car_progression_it : m_cars_progression_configurations)
        {
            if (car_progression_it.second->get_required_rank() <= rank)
            {
                open_car(garage_id, car_progression_it.second->get_car_id());
            }
        }
    }
    
    std::shared_ptr<ces_garage_database_component::garage_dto::car_dto> ces_garage_database_component::get_car(i32 garage_id, i32 car_id)
    {
        std::shared_ptr<ces_garage_database_component::garage_dto::car_dto> result = nullptr;
        auto garager_record = std::make_shared<gb::db::database_entity<db_garage_table, db_garage_data>>(m_database_coordinator.lock());
        if(!garager_record->load_from_db(garage_id))
        {
            assert(false);
        }
        else
        {
            auto car_record = std::make_shared<gb::db::database_entity<db_car_table, db_car_data>>(m_database_coordinator.lock());
            if(!car_record->load_from_db(car_id))
            {
                assert(false);
            }
            else
            {
                auto car_dto = std::make_shared<ces_garage_database_component::garage_dto::car_dto>(m_database_coordinator.lock());
                car_dto->m_id = car_record->get_data().m_id;
                car_dto->m_garage_id = car_record->get_data().m_garage_id;
                car_dto->m_is_openned = car_record->get_data().m_is_openned;
                car_dto->m_is_bought = car_record->get_data().m_is_bought;
                car_dto->m_price = car_record->get_data().m_price;
                car_dto->m_car_body_color_id = car_record->get_data().m_car_body_color_id;
                car_dto->m_car_windows_color_id = car_record->get_data().m_car_windows_color_id;
                car_dto->m_car_speed_upgrade = car_record->get_data().m_car_speed_upgrade;
                car_dto->m_car_handling_upgrade = car_record->get_data().m_car_handling_upgrade;
                car_dto->m_car_rigidity_upgrade = car_record->get_data().m_car_rigidity_upgrade;
                result = car_dto;
            }
        }
        return result;
    }
    
    i32 ces_garage_database_component::get_car_unlocked_rank(i32 car_id) const
    {
        i32 result = 0;
        const auto car_progression_it = m_cars_progression_configurations.find(car_id);
        if (car_progression_it != m_cars_progression_configurations.end())
        {
            result = car_progression_it->second->get_required_rank();
        }
        return result;
    }
    
    i32 ces_garage_database_component::get_price_for_color_switch(i32 garage_id, i32 car_id)
    {
        i32 result = 0;
        const auto car_data = get_car(garage_id, car_id);
        result = car_data->get_id() * m_initial_price_for_color_switch * m_price_for_color_switch_increase_coefficient;
        return result;
    }
    
    i32 ces_garage_database_component::get_price_for_speed_upgrade(i32 garage_id, i32 car_id, f32 delta)
    {
        i32 result = 0;
        i32 delta_upgrades_amount = std::floor(delta * 10.f);
        if (delta_upgrades_amount > 0)
        {
            const auto car_data = get_car(garage_id, car_id);
            i32 current_upgrades_amount = std::ceil(car_data->get_car_speed_upgrade() * 10.f);
            i32 one_upgrade_price = car_data->get_id() * m_initial_price_for_performance_upgrade * m_price_for_performance_upgrade_increase_coefficient_per_car;
            result += current_upgrades_amount * one_upgrade_price * m_price_for_performance_upgrade_increase_coefficient_per_upgrade;
            result += delta_upgrades_amount * one_upgrade_price * m_price_for_performance_upgrade_increase_coefficient_per_upgrade;
        }
        return result;
    }
    
    i32 ces_garage_database_component::get_price_for_handling_upgrade(i32 garage_id, i32 car_id, f32 delta)
    {
        i32 result = 0;
        i32 delta_upgrades_amount = std::floor(delta * 10.f);
        if (delta_upgrades_amount > 0)
        {
            const auto car_data = get_car(garage_id, car_id);
            i32 current_upgrades_amount = std::ceil(car_data->get_car_handling_upgrade() * 10.f);
            i32 one_upgrade_price = car_data->get_id() * m_initial_price_for_performance_upgrade * m_price_for_performance_upgrade_increase_coefficient_per_car;
            result += current_upgrades_amount * one_upgrade_price * m_price_for_performance_upgrade_increase_coefficient_per_upgrade;
            result += delta_upgrades_amount * one_upgrade_price * m_price_for_performance_upgrade_increase_coefficient_per_upgrade;
        }
        return result;
    }
    
    i32 ces_garage_database_component::get_price_for_durability_upgrade(i32 garage_id, i32 car_id, f32 delta)
    {
        i32 result = 0;
        i32 delta_upgrades_amount = std::floor(delta * 10.f);
        if (delta_upgrades_amount > 0)
        {
            const auto car_data = get_car(garage_id, car_id);
            i32 current_upgrades_amount = std::ceil(car_data->get_car_rigidity_upgrade() * 10.f);
            i32 one_upgrade_price = car_data->get_id() * m_initial_price_for_performance_upgrade * m_price_for_performance_upgrade_increase_coefficient_per_car;
            result += current_upgrades_amount * one_upgrade_price * m_price_for_performance_upgrade_increase_coefficient_per_upgrade;
            result += delta_upgrades_amount * one_upgrade_price * m_price_for_performance_upgrade_increase_coefficient_per_upgrade;
        }
        return result;
    }
}
