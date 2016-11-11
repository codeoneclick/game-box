#include "ani_tag_parser_sequences.h"
#include "ani_timeline.h"
#include "ani_asset_metadata.h"

namespace gb
{
    namespace anim
    {
        void ani_tag_parser_sequences::read(const ani_parser_t& parser,
                                            const std::shared_ptr<ani_stream>& stream,
                                            const std::shared_ptr<ani_asset_metadata>& asset_metadata,
                                            const std::shared_ptr<ani_timeline>& timeline)
        {
            ui32 count = stream->read_u32();
            
            for (ui32 i = 0; i < count; ++i)
            {
                std::string id;
                stream->read_string(&id);
                ui16 start = stream->read_u16() - 1;
                ui16 end = stream->read_u16();
                timeline->push_animation_sequence(id, start, end);
            }
        }
    }
}
