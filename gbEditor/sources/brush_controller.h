//
//  brush_controller.hpp
//  gbEditor
//
//  Created by Serhii Serhiiv on 2/2/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef brush_controller_hpp
#define brush_controller_hpp

#include "main_headers.h"
#include "declarations.h"
#include "ed_declarations.h"

namespace gb
{
    namespace ed
    {
        class brush_controller
        {
        private:
            
            material_shared_ptr get_brush_material(const texture_shared_ptr& brush);
            
        protected:

            landscape_shared_ptr m_landscape;
            sprite_shared_ptr m_brush;
            
        public:
            
            brush_controller(const landscape_shared_ptr& landscape, const sprite_shared_ptr& brush);
            ~brush_controller();
            
            void set_active_brush(i32 layer, i32 sampler);
        };
    };
};


#endif 
