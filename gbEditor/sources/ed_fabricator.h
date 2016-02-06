//
//  ed_fabricator.h
//  gbEditor
//
//  Created by sergey.sergeev on 1/15/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef ed_fabricator_h
#define ed_fabricator_h

#include "main_headers.h"
#include "declarations.h"
#include "ed_declarations.h"

namespace gb
{
    namespace ed
    {
        class ed_fabricator
        {
        private:
            
        protected:
            
            scene_fabricator_shared_ptr m_fabricator;
            std::set<game_object_shared_ptr> m_game_objects_container;
            
        public:
            
            ed_fabricator(const scene_fabricator_shared_ptr& fabricator);
            ~ed_fabricator();
            
            grid_shared_ptr create_grid(const std::string& filename, i32 num_rows, i32 num_columns, f32 rows_gap, f32 columns_gap);
            stroke_shared_ptr create_stroke(const std::string& filename);
            landscape_shared_ptr create_landscape(const std::string& filename, const glm::vec2& size,
                                                  const std::vector<std::string>& masks_filenames,
                                                  const std::vector<std::string>& brushes_filenames);
            brush_shared_ptr create_brush(const std::string& filename);
        };
    };
};

#endif
