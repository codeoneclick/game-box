//
//  canvas.h
//  gbEditor
//
//  Created by Serhii Serhiiv on 2/12/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef canvas_h
#define canvas_h

#include "game_object.h"
#include "ed_declarations.h"

namespace gb
{
    namespace ed
    {
        class canvas : public game_object
        {
        private:
            
            std::vector<canvas_chunk_shared_ptr> m_chunks;
            glm::ivec2 m_size;
            std::unordered_map<std::string, std::unordered_map<i32, material_shared_ptr>> m_materials;
            
        protected:
            
        public:
            
            canvas();
            ~canvas();
            
            std::property_rw<glm::vec2> size;
            
            void add_material(const std::string& technique_name, i32 technique_pass, const material_shared_ptr& material);
            void remove_material(const std::string& technique_name, i32 technique_pass);
        };
    };
};

#endif
