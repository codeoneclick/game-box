#include "ani_tag_parser_animation_masks.h"
#include "ani_timeline.h"

namespace gb
{
    namespace anim
    {
        
        void ani_tag_parser_animation_masks::read(const ani_parser_t& parser,
                                                  const std::shared_ptr<ani_stream>& stream,
                                                  const std::shared_ptr<ani_asset_metadata>& asset_metadata,
                                                  const std::shared_ptr<ani_timeline>& timeline)
        {
            ui32 count = stream->read_u32();
            for (ui32 i = 0; i < count; ++i)
            {
                ui32 object_id = stream->read_u32();
                ui32 element_atlas_id_reference = stream->read_u32();
                
                if (stream->get_input()->get_header()->get_major_version() >= 4)
                {
                    ui16 character_type = stream->read_u16();
                    timeline->push_animation_mask(object_id, element_atlas_id_reference, static_cast<e_ani_character_type>(character_type));
                }
                else
                {
                    timeline->push_animation_mask(object_id, element_atlas_id_reference, e_ani_character_type::e_texture);
                }
            }
        }
    }
}
