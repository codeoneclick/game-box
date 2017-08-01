//
//  db_character_entity_controller.h
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
#include "database_entities_controllers.h"
#include "db_character_entity.h"

namespace game
{
    class db_character_entity_controller : public gb::db::database_entity_controller
    {
    private:
    
    protected:
        
    public:
        
        CTTI_CLASS_GUID(db_character_entity_controller, db_character_entity_controller::g_guids_container, override)
        db_character_entity_controller(const gb::db::database_connection_shared_ptr& database);
        
        virtual void construct() override;
        
        bool load_from_db(const db_character_entity_shared_ptr& entity);
        bool save_to_db(const db_character_entity_shared_ptr& entity);
    };
};
