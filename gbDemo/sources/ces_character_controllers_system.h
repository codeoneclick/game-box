//
//  ces_character_controllers_system.h
//  gbDemo
//
//  Created by serhii serhiiv on 10/31/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef ces_character_controllers_system_h
#define ces_character_controllers_system_h

#include "ces_base_system.h"

class ces_character_controllers_system : public gb::ces_base_system
{
private:
    
protected:
    
    void on_feed_start(f32 deltatime);
    void on_feed(const gb::ces_entity_shared_ptr& entity, f32 deltatime);
    void on_feed_end(f32 deltatime);
    
    void update_recursively(const gb::ces_entity_shared_ptr& entity, f32 deltatime);
    
public:
    
    CTTI_CLASS_GUID(ces_character_controllers_system, gb::ces_base_system::g_guids_container)
    ces_character_controllers_system();
    ~ces_character_controllers_system();
};


#endif
