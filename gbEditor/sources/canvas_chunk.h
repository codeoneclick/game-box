//
//  canvas_chunk.hpp
//  gbEditor
//
//  Created by Serhii Serhiiv on 2/12/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef canvas_chunk_h
#define canvas_chunk_h

#include "game_object.h"
#include "memory_map.h"

namespace gb
{
    namespace ed
    {
        class canvas_chunk_texture_mmap_RGB565
        {
        private:
            
            std::shared_ptr<memory_map> m_descriptor;
            i32 m_size;
            i32 m_offset;
            
        protected:
            
        public:
            
            canvas_chunk_texture_mmap_RGB565(const std::shared_ptr<memory_map>& descriptor) :
            m_descriptor(descriptor),
            m_size(0),
            m_offset(0)
            {
                size.setter([=](i32 size) {
                    m_size = size;
                });
                size.getter([=]() {
                    return m_size;
                });
                
                offset.setter([=](i32 offset) {
                    m_offset = offset;
                });
                offset.getter([=]() {
                    return m_offset;
                });
                
                pointer.getter([=]() {
                    ui16* pointer = (ui16* )m_descriptor->pointer();
                    assert(pointer != nullptr);
                    
                    return pointer + m_offset;
                });
            }
            
            ~canvas_chunk_texture_mmap_RGB565(void) = default;
            
            std::property_rw<i32> size;
            std::property_rw<i32> offset;
            std::property_ro<ui16 *> pointer;
        };
        
        class canvas_chunk : public game_object
        {
        public:
            
            static const f32 k_size;
            
        private:
            
        protected:
            
        public:
            
            canvas_chunk();
            ~canvas_chunk();
        };
    };
};

#endif
