//
//  ui_graph.h
//  gbUI
//
//  Created by sergey.sergeev on 10/5/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef ui_graph_h
#define ui_graph_h

#include "i_scene_graph.h"
#include "ui_declarations.h"

#define ui_graph_id 1
#define ui_graph_inst std::static_pointer_cast<ui::gb::ui_graph>(game_scene::get_transition()->get_graph(ui_graph_id))

namespace gb
{
    namespace ui
    {
        class ui_graph : public i_scene_graph
        {
        private:
            
        protected:
            
            void on_update(f32 deltatime);
            
        public:
            
            ui_graph();
            ~ui_graph();
        };
    };
};


#endif
