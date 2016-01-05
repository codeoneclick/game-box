//
//  demo_game_transition.h
//  gbDemo
//
//  Created by Sergey Sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef demo_game_transition_h
#define demo_game_transition_h

#include "game_transition.h"
#include "koth_declarations.h"

class demo_game_transition : public gb::game_transition
{
private:
    
protected:
    
    std::map<i32, bool> m_keys_state;
    
    void create_scene();
    void destroy_scene();
    
    void on_gr_dragged(const glm::ivec2& point, const glm::ivec2& delta, gb::e_input_element input_element);
    
    void on_key_up(i32 key);
    void on_key_down(i32 key);
    
    void update_key_state();
    
public:
    
    demo_game_transition(const std::string& guid, bool is_offscreen);
    ~demo_game_transition();
};

#endif
