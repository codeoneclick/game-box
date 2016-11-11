//
//  character_controller.h
//  gbDemo
//
//  Created by serhii serhiiv on 10/31/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef character_controller_h
#define character_controller_h

#include "main_headers.h"
#include "declarations.h"
#include "ui_declarations.h"
#include "ed_declarations.h"
#include "cs_declarations.h"
#include "ces_entity.h"

class character_navigator;

class character_controller : public gb::ces_entity
{
private:
    
protected:
    
    gb::ui::joystick_shared_ptr m_joystick;
    gb::camera_shared_ptr m_camera;
    gb::game_object_shared_ptr m_character;
    
    f32 m_joystick_angle;
    bool m_is_dragging;
    
    std::shared_ptr<character_navigator> m_navigator;
    
    void on_joystick_dragging(const gb::ces_entity_shared_ptr& joystick, const glm::vec2& delta);
    void on_joystick_end_dragging(const gb::ces_entity_shared_ptr& joystick);
    
public:
    
    character_controller(const gb::camera_shared_ptr& camera, const gb::game_object_shared_ptr& character);
    ~character_controller();
    
    void set_joystick(const gb::ui::joystick_shared_ptr& joystick);
    
    void update(const gb::ces_entity_shared_ptr& entity, f32 deltatime);
    
};

#endif
