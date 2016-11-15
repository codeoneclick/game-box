#include "ani_tag_parser_animation_frames.h"
#include "ani_timeline.h"
#include "ani_subobject_state.h"
#include "ani_animation_frame.h"

namespace gb
{
    namespace anim
    {
        void ani_tag_parser_animation_frames::read(const ani_parser_t& parser,
                                                   const std::shared_ptr<ani_stream>& stream,
                                                   const std::shared_ptr<ani_asset_metadata>& asset_metadata,
                                                   const std::shared_ptr<ani_timeline>& timeline)
        {
            stream->read_u32();
            
            if (timeline->get_animation_objects().empty())
            {
                return;
            }
            
            for (ani_animation_objects_t::const_iterator iterator = timeline->get_animation_objects().begin(), end = timeline->get_animation_objects().end(); iterator != end; ++iterator)
            {
                ui32 object_id = iterator->first;
                std::shared_ptr<ani_subobject_state> state = std::make_shared<ani_subobject_state>();
                state->empty(object_id);
                m_current_states[object_id] = state;
            }
            
            const ui16 total_frame_count = stream->get_input()->get_header()->m_frames_count;
            ui32 frame_number = stream->read_u32();
            
            for (ui32 i = 0; i < total_frame_count; ++i)
            {
                if ((frame_number - 1) == i)
                {
                    ui32 num_objects = stream->read_u32();
                    
                    typedef std::list<std::shared_ptr<ani_subobject_state>> states_list_t;
                    states_list_t states_list;
                    
                    for (ui32 j = 0; j < num_objects; ++j)
                    {
                        std::shared_ptr<ani_subobject_state> state = extract_state(stream);
                        states_list.push_back(state);
                    }
                    
                    for (states_list_t::iterator iterator = states_list.begin(), end = states_list.end(); iterator != end; ++iterator)
                    {
                        std::shared_ptr<ani_subobject_state> state = *iterator;
                        m_current_states[state->m_object_id_reference] = state;
                    }
                    
                    if (stream->get_position() < stream->get_tag_expected_position())
                    {
                        frame_number = stream->read_u32();
                    }
                }
                
                std::shared_ptr<ani_animation_frame> frame = std::make_shared<ani_animation_frame>();
                for (ani_states_t::iterator iterator = m_current_states.begin(), end = m_current_states.end(); iterator != end; ++iterator)
                {
                    frame->push_object_state(iterator->second);
                }
                
                frame->sort_states_by_z_index();
                timeline->push_animation_frame(frame);
            }
        }
        
        std::shared_ptr<ani_subobject_state> ani_tag_parser_animation_frames::extract_state(const std::shared_ptr<ani_stream>& stream)
        {
            std::shared_ptr<ani_subobject_state> state = std::make_shared<ani_subobject_state>();
            
            f32 colors_unused[7];
            
            i8 has_color_transform = stream->read_ubyte();
            i8 has_masks = stream->read_ubyte();
            i8 has_effect = stream->read_ubyte();
            
            state->m_object_id_reference = stream->read_u32();
            i32 z_index = stream->read_s32();
            state->set_z_index(z_index);
            
            stream->read_float();
            
            ani_affine_transform affine_transform;
            ani_tag_parser_base::deserialize(stream, &affine_transform);
            state->set_affine_transform(affine_transform);

            if (has_color_transform)
            {
                stream->read_n_bytes(colors_unused, sizeof(f32) * 7);
            }
            
            if (has_effect)
            {
                ui8 effects_count = stream->read_ubyte();
                
                for (ui32 i = 0; i < effects_count; ++i)
                {
                    ui32 effect = stream->read_u32();
                    
                    if (effect == 1)
                    {
                        glm::vec2 point;
                        ani_tag_parser_base::deserialize(stream, &point);
                    }
                    else if (effect == 6)
                    {
                        for (ui32 j = 0; j < 4; ++j)
                        {
                            for (ui32 k = 0; k < 4; ++k)
                            {
                                stream->read_float();
                            }
                            stream->read_float();
                        }
                    }
                    else if (effect == 2)
                    {
                        stream->read_u32();
                        glm::vec2 size;
                        ani_tag_parser_base::deserialize(stream, &size);
                        
                        stream->read_float();
                        stream->read_ubyte();
                        stream->read_ubyte();
                    }
                    else if (effect == 0)
                    {
                        stream->read_u32();
                        glm::vec2 size;
                        ani_tag_parser_base::deserialize(stream, &size);
                        
                        stream->read_float();
                        stream->read_float();
                        stream->read_float();
                        stream->read_ubyte();
                        stream->read_ubyte();
                    }
                }
            }
            
            if (has_masks)
            {
                state->m_mask_object_id_reference = stream->read_u32();
            }
            
            return state;
        }
    }
}
