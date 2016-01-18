//
//  game_transition.h
//  gbCore
//
//  Created by sergey.sergeev on 8/12/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef game_transition_h
#define game_transition_h

#include "main_headers.h"
#include "declarations.h"
#include "input_context.h"

namespace gb
{
    class game_transition : public std::enable_shared_from_this<game_transition>
    {
    private:
        
        std::string m_guid;
        bool m_offscreen;
        
        ces_systems_feeder_shared_ptr m_system_feeder;
        
        input_context_shared_ptr m_input_context;
        
    protected:
        
        virtual void create_scene() = 0;
        virtual void destroy_scene() = 0;
        
        scene_graph_shared_ptr m_scene;
        
        configuration_accessor_shared_ptr m_configuration_accessor;
        resource_accessor_shared_ptr m_resource_accessor;
        
        i32 m_screen_width;
        i32 m_screen_height;
        
    public:
        
        game_transition(const std::string& guid, bool is_offscreen);
        virtual ~game_transition();
        
        void on_activated(const graphics_context_shared_ptr& graphics_context,
                          const input_context_shared_ptr& input_context,
                          const configuration_accessor_shared_ptr& configurations_accessor,
                          const resource_accessor_shared_ptr& resource_accessor);
        
        void on_deactivated();
        
        std::string get_guid() const;
        
        void set_fabricator(const scene_fabricator_shared_ptr& fabricator);
        
        void add_system(const ces_system_shared_ptr& system);
        ces_system_shared_ptr get_system(i32 type);
        
        input_context_shared_ptr get_input_context() const;
        
        i32 get_screen_width() const;
        i32 get_screen_height() const;
    };
};

#endif
