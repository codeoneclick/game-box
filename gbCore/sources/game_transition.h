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
#include "game_loop.h"
#include "ces_entity.h"
#include "ces_system_types.h"
#include "input_context.h"

namespace gb
{
    class game_transition : public input_context_listener,
    public i_game_loop,
    public std::enable_shared_from_this<game_transition>
    {
    private:
        
        std::string m_guid;
        bool m_offscreen;
        ces_systems_feeder_shared_ptr m_system_feeder;
        
        input_context_shared_ptr m_input_context;
        
        i32 m_width;
        i32 m_height;
        
    protected:
        
        void on_update(f32 deltatime);
        
        virtual void create_scene() = 0;
        virtual void destroy_scene() = 0;
        
        game_scene_shared_ptr m_scene;
        
        configuration_accessor_shared_ptr m_configuration_accessor;
        resource_accessor_shared_ptr m_resource_accessor;
        
        std::map<i32, i_scene_fabricator_shared_ptr> m_fabricators;
        std::map<i32, i_scene_graph_shared_ptr> m_graphs;
        
        virtual void on_gr_pressed(const glm::ivec2& point, e_input_element input_element) { };
        virtual void on_gr_released(const glm::ivec2& point, e_input_element input_element) { };
        virtual void on_gr_moved(const glm::ivec2& point, const glm::ivec2& delta) { };
        virtual void on_gr_dragged(const glm::ivec2& point, const glm::ivec2& delta, e_input_element input_element) { };
        
        virtual void on_key_up(i32 key) { };
        virtual void on_key_down(i32 key) { };
        
    public:
        
        game_transition(const std::string& guid, bool is_offscreen);
        virtual ~game_transition();
        
        void on_activated(const graphics_context_shared_ptr& graphics_context,
                          const input_context_shared_ptr& input_context,
                          const configuration_accessor_shared_ptr& configurations_accessor,
                          const resource_accessor_shared_ptr& resource_accessor);
        
        void on_deactivated(void);
        
        std::string get_guid() const;
        
        i_scene_fabricator_shared_ptr get_fabricator(i32 guid) const;
        i_scene_graph_shared_ptr get_graph(i32 guid) const;
        
        void add_system(const ces_system_shared_ptr& system);
        ces_system_shared_ptr get_system(const std::string& type);
        
        void add_fabricator(const i_scene_fabricator_shared_ptr& fabricator, i32 guid);
        void remove_fabricator(i32 guid);
        
        void add_graph(const i_scene_graph_shared_ptr& graph, i32 guid);
        void remove_graph(i32 guid);
        
        i32 get_width() const;
        i32 get_height() const;
        
        input_context_shared_ptr get_input_context() const;
    };
};

#endif
