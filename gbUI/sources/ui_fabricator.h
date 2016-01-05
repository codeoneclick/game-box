//
//  ui_fabricator.h
//  gbUI
//
//  Created by sergey.sergeev on 10/5/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef ui_fabricator_h
#define ui_fabricator_h

#include "i_scene_fabricator.h"
#include "ui_declarations.h"

#define ui_fabricator_id 1
#define ui_fabricator_inst std::static_pointer_cast<ui::gb::ui_fabricator>(game_scene::get_transition()->get_fabricator(ui_fabricator_id))

namespace gb
{
    namespace ui
    {
        class ui_fabricator : public i_scene_fabricator
        {
        private:
            
        protected:
            
        public:
            
            ui_fabricator();
            ~ui_fabricator();
        };
    };
};


#endif
