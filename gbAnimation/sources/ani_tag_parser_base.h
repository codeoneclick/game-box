//
//  ani_tag_parser_base.h
//  gbAnimation
//
//  Created by serhii serhiiv on 11/3/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef ani_tag_parser_base_h
#define ani_tag_parser_base_h

#include "ani_stream.h"

namespace gb
{
    namespace anim
    {
        class ani_asset_metadata;
        class ani_timeline;
        
        class ani_tag_parser_base
        {
        public:
            
            typedef std::function<void(const std::shared_ptr<ani_stream>& stream,
            const std::shared_ptr<ani_asset_metadata>& asset_metadata,
            const std::shared_ptr<ani_timeline>& timeline)> ani_parser_t;
            
            ani_parser_t m_parser;
            
        public:
            
            virtual ~ani_tag_parser_base() = default;
            virtual void read(const ani_parser_t& parser,
                              const std::shared_ptr<ani_stream>& stream,
                              const std::shared_ptr<ani_asset_metadata>& asset_metadata,
                              const std::shared_ptr<ani_timeline>& timeline) = 0;
            
            static void deserialize(const std::shared_ptr<ani_stream>& stream, glm::vec2* out);
            static void deserialize(const std::shared_ptr<ani_stream>& stream, glm::vec4* out);
            static void deserialize(const std::shared_ptr<ani_stream>& stream, glm::u8vec4* out);
            static void deserialize(const std::shared_ptr<ani_stream>& stream, ani_affine_transform* out);
        };
    };
};

#endif
