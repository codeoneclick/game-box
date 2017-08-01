//
//  db_character_entity.h
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

namespace game
{
    class db_character_entity : public std::enable_shared_from_this<db_character_entity>
    {
    public:
        
        struct data_t
        {
            i32 m_level = 10;
            f32 m_hp = 100;
            f32 m_damage = 5;
        };
        
    private:
        
        friend class db_character_entity_controller;
        
    protected:
        
        i32 m_id = 1;
        data_t m_data;
        gb::db::database_entities_controllers_shared_ptr m_database_entities_controllers;
        
    public:
        
        db_character_entity(const gb::db::database_entities_controllers_shared_ptr& database_entities_controllers);
        ~db_character_entity();
        
        void save_to_db();
        void load_from_db(i32 id);
    };
};
