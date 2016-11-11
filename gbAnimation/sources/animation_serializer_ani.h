//
//  animation_serializer_ani.h
//  gbAnimation
//
//  Created by serhii serhiiv on 11/3/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef animation_serializer_ani_h
#define animation_serializer_ani_h

#include "resource_serializer.h"
#include "ani_tag_parser_base.h"

namespace gb
{
    namespace anim
    {
        class ani_header;
        class ani_stream;
        class ani_tag_parser_base;
        
        class animation_serializer_ani final : public resource_serializer
        {
        private:
            
            typedef std::map<ui32, std::shared_ptr<ani_tag_parser_base>> tags_parsers_t;
            tags_parsers_t m_tags_parsers;
            
            void read_header_end(const std::shared_ptr<ani_header>& header, const std::shared_ptr<ani_stream>& stream);
            void read_header_end_v4(const std::shared_ptr<ani_header>& header, const std::shared_ptr<ani_stream>& stream);
            
            void register_tag_loaders_v3();
            void register_tag_loaders_v4();
            void register_tag_loaders_common();
            
            void parse(const std::shared_ptr<ani_stream>& stream,
                       const std::shared_ptr<ani_asset_metadata>& asset_metadata,
                       const std::shared_ptr<ani_timeline>& timeline);
            
            ani_tag_parser_base::ani_parser_t m_parser;
            
        protected:
            
            std::string m_filename;
            
        public:
            
            animation_serializer_ani(const std::string& filename,
                                     const resource_shared_ptr& resource);
            ~animation_serializer_ani();
            
            void serialize(const resource_transfering_data_shared_ptr& transfering_data);
        };
    };
};

#endif
