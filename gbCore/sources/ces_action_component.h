//
//  ces_action_component.h
//  gbCore
//
//  Created by sergey.sergeev on 1/22/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef ces_action_component_h
#define ces_action_component_h

#include "ces_base_component.h"

namespace gb
{
    class ces_action_component : public ces_base_component
    {
    public:
        
         typedef std::function<void(const ces_entity_shared_ptr&, f32)> t_update_callback;
        
    private:
        
    protected:
        
        t_update_callback m_update_callback;
        
    public:
        
        ces_action_component();
        ~ces_action_component();
        
        void on_update(const ces_entity_shared_ptr& entity, f32 deltatime);
        void set_update_callback(const t_update_callback& callback);
    };
};

#endif
