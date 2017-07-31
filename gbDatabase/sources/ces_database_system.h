//
//  ces_database_system.h
//  gb_sound
//
//  Created by serhii serhiiv on 6/2/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_system.h"
#include "sqlite3.h"

namespace gb
{
    namespace al
    {
        class ces_database_system : public ces_base_system
        {
        private:
            
        protected:
            
            void on_feed_start(f32 dt);
            void on_feed(const ces_entity_shared_ptr& root, f32 dt);
            void on_feed_end(f32 dt);
            
        public:
            
            CTTI_CLASS_GUID(ces_database_system, ces_base_system::g_guids_container)
            ces_database_system();
            ~ces_database_system();
        };
    };
};
