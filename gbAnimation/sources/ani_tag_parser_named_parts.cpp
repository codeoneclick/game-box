#include "ani_tag_parser_named_parts.h"
#include "ani_timeline.h"
#include "ani_asset_metadata.h"

namespace gb
{
    namespace anim
    {
        void ani_tag_parser_named_parts::read(const ani_parser_t& parser,
                                              const std::shared_ptr<ani_stream>& stream,
                                              const std::shared_ptr<ani_asset_metadata>& asset_metadata,
                                              const std::shared_ptr<ani_timeline>& timeline)
        {
            ui32 count = stream->read_u32();
            
            for (ui32 i = 0; i < count; ++i)
            {
                ui32 object_id_reference = stream->read_u32();
                std::string name;
                stream->read_string(&name);
                timeline->push_named_part(object_id_reference, name);
                std::cout<<"named part: "<<name<<std::endl;
            }
        }
    }
}
