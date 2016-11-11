//
//  ani_tag_parser_base.cpp
//  gbAnimation
//
//  Created by serhii serhiiv on 11/3/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ani_tag_parser_base.h"
namespace gb
{
    namespace anim
    {
        void ani_tag_parser_base::deserialize(const std::shared_ptr<ani_stream>& stream, glm::vec2* out)
        {
            out->x = stream->read_float();
            out->y = stream->read_float();
        }
        
        void ani_tag_parser_base::deserialize(const std::shared_ptr<ani_stream>& stream, glm::vec4* out)
        {
            out->x = stream->read_float();
            out->y = stream->read_float();
            out->w = stream->read_float();
            out->z = stream->read_float();
        }
        
        void ani_tag_parser_base::deserialize(const std::shared_ptr<ani_stream>& stream, glm::u8vec4* out)
        {
            stream->read_n_bytes(out, 4);
        }
        
        void ani_tag_parser_base::deserialize(const std::shared_ptr<ani_stream>& stream, ani_affine_transform* out)
        {
            stream->read_n_bytes(out, sizeof(ani_affine_transform));
        }
    }
}
