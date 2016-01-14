//
//  ces_bound_touch_component.hpp
//  gbCore
//
//  Created by sergey.sergeev on 1/14/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef ces_bound_touch_component_h
#define ces_bound_touch_component_h

#include "ces_base_component.h"
#include "input_context.h"

namespace gb
{
    class ces_bound_touch_component : public ces_base_component
    {
    public:
        
        typedef std::function<void(const ces_entity_shared_ptr&, const glm::vec2& point, e_input_element input_element, e_input_state input_state)> t_callback;
        
    private:
        
    protected:
        
        glm::vec4 m_frame;
        
        std::array<bool, e_input_state_max> m_responders;
        std::array<t_callback, e_input_state_max> m_callbacks;
        
    public:
        
        ces_bound_touch_component();
        ~ces_bound_touch_component();
        
        void set_frame(const glm::vec4& frame);
        glm::vec4 get_frame() const;
        
        void enable(e_input_state state, bool value);
        bool is_enabled(e_input_state state) const;
        
        void set_callback(e_input_state input_state, const t_callback& callback);
        
        t_callback get_callback(e_input_state input_state) const;
    };
};

#endif
