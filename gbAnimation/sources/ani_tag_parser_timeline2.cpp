#include "ani_tag_parser_timeline2.h"
#include "ani_timeline.h"
#include "ani_asset_metadata.h"

namespace gb
{
    namespace anim
    {
        void ani_tag_parser_timeline2::read(const ani_parser_t& parser,
                                           const std::shared_ptr<ani_stream>& stream,
                                           const std::shared_ptr<ani_asset_metadata>& asset_metadata,
                                           const std::shared_ptr<ani_timeline>& timeline)
        {
            ui32 id = stream->read_u32();
            ui32 frames_count = stream->read_u32();
            glm::vec4 aabb;
            glm::vec2 pivot;
            
            ani_tag_parser_base::deserialize(stream, &aabb);
            ani_tag_parser_base::deserialize(stream, &pivot);
            
            std::shared_ptr<ani_timeline> new_timeline = std::make_shared<ani_timeline>(timeline, id, aabb, pivot, frames_count);
            
            std::string value;
            stream->read_string(&value);
            new_timeline->set_linkage_name(value);
            
            stream->read_string(&value);
            new_timeline->set_base_class(value);
            
            parser(stream, asset_metadata, new_timeline);
            
            asset_metadata->push_timeline(id, new_timeline);
            if (id == 0)
            {
                asset_metadata->set_root_timeline(0);
            }
        }
    }
}
