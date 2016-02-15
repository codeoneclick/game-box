//
//  grid.h
//  gbEditor
//
//  Created by sergey.sergeev on 1/15/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef grid_h
#define grid_h

#include "game_object.h"

namespace gb
{
    namespace ed
    {
        class grid : public game_object
        {
        private:
            
        protected:

            glm::vec4 m_color;
            
        public:
            
            grid();
            ~grid();
            
            void set_color(const glm::vec4& color);
            glm::vec4 get_color() const;
        };
    };
};

#endif
