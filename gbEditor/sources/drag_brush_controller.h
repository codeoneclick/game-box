//
//  brush_controller.hpp
//  gbEditor
//
//  Created by Serhii Serhiiv on 2/2/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef drag_brush_controller_h
#define drag_brush_controller_h

#include "drag_controller.h"

namespace gb
{
    namespace ed
    {
        class drag_brush_controller
        {
        private:
            
            material_shared_ptr get_brush_material(const texture_shared_ptr& brush);
            
        protected:

            landscape_shared_ptr m_landscape;
            sprite_shared_ptr m_brush;
            
        public:
            
            drag_brush_controller(const landscape_shared_ptr& landscape, const sprite_shared_ptr& brush);
            ~drag_brush_controller();
            
            void set_active_brush(i32 layer, i32 sampler);
        };
    };
};

#endif 
