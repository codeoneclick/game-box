#pragma once

#include "ani_tag_parser_base.h"

namespace gb
{
    namespace anim
    {
        class ani_tag_parser_textfield : public ani_tag_parser_base
        {
        public:
            
            virtual void read(const ani_parser_t& parser,
                              const std::shared_ptr<ani_stream>& stream,
                              const std::shared_ptr<ani_asset_metadata>& asset_metadata,
                              const std::shared_ptr<ani_timeline>& timeline) override;
        };
    };
};
