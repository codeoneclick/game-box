//
//  ces_bound_touch_component.hpp
//  gbCore
//
//  Created by sergey.sergeev on 1/14/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"

#if USED_GRAPHICS_API != NO_GRAPHICS_API

#include "input_context.h"

namespace gb
{
    class ces_bound_touch_component : public ces_base_component
    {
    public:
        
        typedef std::function<void(const ces_entity_shared_ptr&,
        const glm::vec2&,
        e_input_source,
        e_input_state)> t_callback;
        
        enum e_mode
        {
            e_mode_unknown = -1,
            e_mode_2d = 0,
            e_mode_3d
        };
        
    private:
        
    protected:
        
        e_mode m_mode;
        
        std::array<std::array<bool, e_input_source_max>, e_input_state_max> m_responders;
        std::array<std::map<std::string, t_callback>, e_input_state_max> m_callbacks;
        
    public:
        
        STTI_CLASS_GUID(ces_bound_touch_component, ces_base_component::g_guids_container)
        ces_bound_touch_component();
        ~ces_bound_touch_component();
        
        void enable(e_input_state state, e_input_source source, bool value);
        bool is_respond_to(e_input_state state, e_input_source source) const;
        
        std::string add_callback(e_input_state input_state, const t_callback& callback);
        void remove_callback(e_input_state input_state, const std::string& guid);
        void remove_callback(const std::string& guid);
        
        std::list<t_callback> get_callbacks(e_input_state input_state) const;
        
        bool is_2d() const;
        bool is_3d() const;
        
        ces_bound_touch_2d_component_shared_ptr as_2d();
        ces_bound_touch_3d_component_shared_ptr as_3d();
    };
};

#endif
