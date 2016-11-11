#include "ani_tag_parser_animation_frames2.h"
#include "ani_timeline.h"
#include "ani_subobject_state.h"
#include "ani_animation_frame.h"
#include "ani_timeline_action.h"

namespace gb
{
    namespace anim
    {
        void ani_tag_parser_animation_frames2::read(const ani_parser_t& parser,
                                                    const std::shared_ptr<ani_stream>& stream,
                                                    const std::shared_ptr<ani_asset_metadata>& asset_metadata,
                                                    const std::shared_ptr<ani_timeline>& timeline)
        {
            ui32 count = stream->read_u32();
            
            for (ani_animation_objects_t::const_iterator iterator = timeline->get_animation_objects().begin(), end = timeline->get_animation_objects().end(); iterator != end; ++iterator)
            {
                ui32 object_id = iterator->first;
                std::shared_ptr<ani_subobject_state> state = std::make_shared<ani_subobject_state>();
                state->empty(object_id);
                m_current_states[object_id] = state;
            }
            
            ui32 frame_number = stream->read_u32();
            
            for (ui32 i = 0; i < count; ++i)
            {
                i8 has_changes_in_display_list = stream->read_ubyte();
                i8 has_actions = stream->read_ubyte();
                
                if (has_changes_in_display_list)
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
                }
                
                std::shared_ptr<ani_animation_frame> frame = std::make_shared<ani_animation_frame>();
                
                for (ani_states_t::iterator iterator = m_current_states.begin(), end = m_current_states.end(); iterator != end; ++iterator)
                {
                    frame->push_object_state(iterator->second);
                }
                
                frame->sort_states_by_z_index();
                
                if (has_actions)
                {
                    ui32 actions_count = stream->read_u32();
                    for (ui32 action_index = 0; action_index < actions_count; action_index++)
                    {
                        std::shared_ptr<ani_timeline_action> action = std::make_shared<ani_timeline_action>();
                        
                        e_ani_action_type action_type = static_cast<e_ani_action_type>(stream->read_u32());
                        std::string scope;
                        stream->read_string(&scope);
                        
                        std::vector<std::string> parameters;
                        
                        ui32 parameters_length = stream->read_u32();
                        ui32 start_position = stream->get_position();
                        while (parameters_length > stream->get_position() - start_position)
                        {
                            std::string parameter_value;
                            stream->read_string(&parameter_value);
                            parameters.push_back(parameter_value);
                        }
                        
                        action->set_action(action_type, parameters, scope);
                        frame->push_timeline_action(action);
                    }
                }
                
                if (stream->get_position() < stream->get_tag_expected_position())
                {
                    frame_number = stream->read_u32();
                }
                timeline->push_animation_frame(frame);
            }
            m_current_states.clear();
        }
    }
}
