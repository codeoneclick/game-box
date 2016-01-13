//
//  control.h
//  gbUI
//
//  Created by sergey.sergeev on 9/28/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef control_h
#define control_h

#include "main_headers.h"
#include "ui_declarations.h"
#include "game_object.h"

namespace gb
{
    namespace ui
    {
        class control : public game_object
        {
        private:
            
            std::weak_ptr<scene_fabricator> m_fabricator;
            
        protected:
            
            std::map<std::string, renderable_game_object_shared_ptr> m_elements;
            
            scene_fabricator_shared_ptr get_fabricator() const;
            
        public:
            
            control(const scene_fabricator_shared_ptr& fabricator);
            ~control();
            
            virtual void create() = 0;
        };
    };
};

#endif
