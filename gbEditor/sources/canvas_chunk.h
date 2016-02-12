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

namespace gb
{
    namespace ed
    {
        class canvas_chunk : public game_object
        {
        public:
            
            constexpr static const f32 k_size = 256.f;
            
        private:
            
        protected:
            
        public:
            
            canvas_chunk();
            ~canvas_chunk();
        };
    };
};

#endif
