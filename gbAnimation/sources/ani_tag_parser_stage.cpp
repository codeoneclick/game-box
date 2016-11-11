#include "ani_tag_parser_stage.h"
#include "ani_timeline.h"
#include "ani_asset_metadata.h"

namespace gb
{
    namespace anim
    {
        void ani_tag_parser_stage::read(const ani_parser_t& parser,
                                        const std::shared_ptr<ani_stream>& stream,
                                        const std::shared_ptr<ani_asset_metadata>& asset_metadata,
                                        const std::shared_ptr<ani_timeline>& timeline)
        {
            
            __attr_unused ui32 fps = stream->read_u8();
            glm::u8vec4 color;
            ani_tag_parser_base::deserialize(stream, &color);
            __attr_unused ui32 width = stream->read_u16();
            __attr_unused ui32 height = stream->read_u16();
        }
    }
}
