//
//  grid.h
//  gbEditor
//
//  Created by sergey.sergeev on 1/15/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef grid_h
#define grid_h

#include "renderable_game_object.h"
#include "input_context.h"

namespace gb
{
    namespace ed
    {
        class grid : public renderable_game_object
        {
        private:
            
        protected:

            glm::vec4 m_color;
            
        public:
            
            grid();
            ~grid();
            
            void set_color(const glm::vec4& color);
            glm::vec4 get_color() const;
            
            glm::vec4 get_bound() const;
        };
    };
};


#endif
