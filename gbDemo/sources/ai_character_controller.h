//
//  ai_character_controller.h
//  gbDemo
//
//  Created by sergey.sergeev on 10/15/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#ifndef ai_character_controller_h
#define ai_character_controller_h

#include "character_controller.h"
#include "pathfinder.h"

namespace koth
{
    class ai_character_controller : public character_controller
    {
    private:
        
    protected:
        
        std::shared_ptr<pathfinder> m_pathfinder;
        level_shared_ptr m_level;
        
        ai_actions_processor_shared_ptr m_actions_processor;
        
        glm::vec3 m_goal_position;
        glm::ivec2 m_goal_position_index;
        
        static f32 m_trashhold_distance;
        
        std::list<ai_action_shared_ptr> m_current_actions;
        
    public:
        
        ai_character_controller(const gb::game_object_shared_ptr& game_object,
                                const level_shared_ptr& level);
        ~ai_character_controller();
        
        void update(f32 deltatime);
        
        void set_position(const glm::vec3& position);
        void set_goal_position(const glm::vec3& position);
    };
};

#endif
