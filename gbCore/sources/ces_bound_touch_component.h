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
        
        typedef std::function<void(const ces_entity_shared_ptr&, const glm::vec2&, e_input_source, e_input_state)> t_callback;
        
    private:
        
    protected:
        
        glm::vec4 m_frame;
        
        std::array<std::array<bool, e_input_source_max>, e_input_state_max> m_responders;
        std::array<std::map<std::string, t_callback>, e_input_state_max> m_callbacks;
        
    public:
        
        CTTI_CLASS_GUID(ces_bound_touch_component, ces_base_component::g_guids_container)
        ces_bound_touch_component();
        ~ces_bound_touch_component();
        
        void set_frame(const glm::vec4& frame);
        glm::vec4 get_frame() const;
        
        void enable(e_input_state state, e_input_source source, bool value);
        bool is_respond_to(e_input_state state, e_input_source source) const;
        
        std::string add_callback(e_input_state input_state, const t_callback& callback);
        void remove_callback(e_input_state input_state, const std::string& guid);
        void remove_callback(const std::string& guid);
        
        std::list<t_callback> get_callbacks(e_input_state input_state) const;
    };
    
#define unsafe_get_bound_touch_component(entity) static_cast<ces_bound_touch_component*>(entity->get_component(ces_bound_touch_component::class_guid()).get())
#define unsafe_get_bound_touch_component_from_this static_cast<ces_bound_touch_component*>(ces_entity::get_component(ces_bound_touch_component::class_guid()).get())
    
};

#endif
