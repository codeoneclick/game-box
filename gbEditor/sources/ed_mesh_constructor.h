//
//  ed_mesh_constructor.h
//  gbEditor
//
//  Created by sergey.sergeev on 1/15/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef ed_mesh_constructor_h
#define ed_mesh_constructor_h

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
    namespace ed
    {
        class mesh_constructor
        {
        private:
            
        protected:
            
        public:
            
            mesh_constructor() = default;
            ~mesh_constructor() = default;
            
            static mesh_shared_ptr create_grid(i32 num_rows, i32 num_columns, f32 rows_gap, f32 columns_gap);
            static mesh_shared_ptr create_stroke();
        };
    };
};

#endif
