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
#include "memory_map.h"

namespace gb
{
    namespace ed
    {
        class canvas : public game_object
        {
        private:
            
            std::shared_ptr<memory_map> m_descriptor;
            std::vector<canvas_chunk_texture_mmap_RGB565_shared_ptr> m_data;
            std::vector<texture_shared_ptr> m_textures;
            std::vector<canvas_chunk_shared_ptr> m_chunks;
            glm::ivec2 m_size;
            std::unordered_map<std::string, std::unordered_map<i32, material_shared_ptr>> m_materials;
            
        protected:
            
            std::string get_data_filename(const std::string& filename);
            bool is_data_exist(const std::string& filename);
            void create_data(const std::string& filename, const glm::vec2& size);
            
            material_shared_ptr get_draw_material(const texture_shared_ptr& canvas,
                                                  const texture_shared_ptr& brush);
            
        public:
            
            canvas();
            ~canvas();
            
            void add_material(const std::string& technique_name, i32 technique_pass, const material_shared_ptr& material);
            void remove_material(const std::string& technique_name, i32 technique_pass);
            
            void draw(const glm::vec2& point, const texture_shared_ptr& texture);
        };
    };
};

#endif
