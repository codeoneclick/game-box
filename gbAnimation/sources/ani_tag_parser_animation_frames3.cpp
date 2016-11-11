#include "ani_tag_parser_animation_frames3.h"
#include "ani_timeline.h"
#include "ani_subobject_state.h"
#include "ani_animation_frame.h"
#include "ani_timeline_action.h"

namespace gb
{
    namespace anim
    {
        std::shared_ptr<ani_subobject_state> ani_tag_parser_animation_frames3::extract_state(const std::shared_ptr<ani_stream>& stream)
        {
            std::shared_ptr<ani_subobject_state> state = ani_tag_parser_animation_frames::extract_state(stream);
            
            ui16 custom_properties_count = stream->read_u16();
            for (ui16 i = 0; i < custom_properties_count; ++i)
            {
                ui32 index = stream->read_u32();
                state->get_custom_properties_value_indexes().push_back(index);
            }
            return state;
        }
    }
}

