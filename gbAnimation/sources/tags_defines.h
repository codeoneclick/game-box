#pragma once
#include "main_headers.h"
#include "declarations.h"

namespace gb
{
    namespace anim
    {
        class ani_texture_atlas;
        class ani_animation_frame;
        class ani_animation_sequence;
        class ani_external_object;
        class ani_timeline;
        
        enum class e_ani_character_type : uint32_t
        {
            e_texture = 0,
            e_textfield,
            e_timeline,
            e_external
        };
        
        enum class e_ani_action_type : int32_t
        {
            e_none = -1,
            e_stop = 0,
            e_play,
            e_goto_and_stop,
            e_goto_and_play,
            e_dispatch_event,
        };
        
        struct ani_custom_property
        {
            std::string m_name;
            std::vector<std::string> m_possible_values;
        };
        
        typedef std::vector<ani_custom_property> ani_custom_properties_t;
        
        class ani_any_i
        {
        public:
            ~ani_any_i() = default;
        };
        
        template <typename T>
        class ani_any : public ani_any_i {
        public:
            explicit ani_any(const T& data) : m_data(data) {}
            T m_data;
        };
        
        typedef std::tuple<ui32, e_ani_character_type> animation_object_ex_t;
        typedef std::vector<std::shared_ptr<ani_texture_atlas>> ani_texture_atlases_t;
        typedef std::unordered_map<ui32, animation_object_ex_t> ani_animation_masks_t;
        typedef std::unordered_map<ui32, animation_object_ex_t> ani_animation_objects_t;
        typedef std::vector<std::shared_ptr<ani_animation_frame>> ani_animation_frames_t;
        typedef std::unordered_map<std::string, std::shared_ptr<ani_animation_sequence>> ani_animation_sequences_t;
        typedef std::multimap<std::string, ui32> ani_named_parts_t;
        typedef std::unordered_map<ui32, std::shared_ptr<ani_external_object>> ani_external_objects_t;
        typedef std::unordered_map<ui32, std::shared_ptr<ani_timeline>> ani_timelines_t;
        
        struct ani_affine_transform
        {
            f32 a, b, c, d;
            f32 tx, ty;
        };
        
        struct tags
        {
            enum names
            {
                TagEnd = 0,
                TagDefineAtlas = 1,
                TagDefineAnimationMasks = 2,
                TagDefineAnimationObjects = 3,
                TagDefineAnimationFrames = 4,
                TagDefineNamedParts = 5,
                TagDefineSequences = 6,
                TagDefineTextFields = 7,
                TagDefineAtlas2 = 8,
                TagDefineStage = 9,
                TagDefineAnimationObjects2 = 10,
                TagDefineAnimationMasks2 = 11,
                TagDefineAnimationFrames2 = 12,
                TagDefineTimeline = 13,
                TagDefineSounds = 14,
                TagDefineAtlas3 = 15,
                TagDefineAtlas4 = 16,
                TagDefineTimeline2 = 17,
                TagDefineExternalObjects = 18,
                TagDefineAnimationFrames3 = 19,
                TagDefineTimeline3 = 20,
                TagDefineExternalObjects2 = 21,
            };
            
            static std::string to_string(names name)
            {
                switch (name)
                {
                    case names::TagEnd:
                        return "TagEnd";
                        
                    case names::TagDefineAtlas:
                        return "TagDefineAtlas";
                        
                    case names::TagDefineAnimationMasks:
                        return "TagDefineAnimationMasks";
                        
                    case names::TagDefineAnimationObjects:
                        return "TagDefineAnimationObjects";
                        
                    case names::TagDefineAnimationFrames:
                        return "TagDefineAnimationFrames";
                        
                    case names::TagDefineNamedParts:
                        return "TagDefineNamedParts";
                        
                    case names::TagDefineSequences:
                        return "TagDefineSequences";
                        
                    case names::TagDefineStage:
                        return "TagDefineStage";
                        
                    case names::TagDefineAtlas2:
                        return "TagDefineAtlas2";
                        
                    case names::TagDefineTimeline:
                        return "TagDefineTimeline";
                        
                    case names::TagDefineTextFields:
                        return "TagDefineTextFields";
                        
                    case names::TagDefineAnimationMasks2:
                        return "TagDefineAnimationMasks2";
                        
                    case names::TagDefineAnimationObjects2:
                        return "TagDefineAnimationObjects2";
                        
                    case names::TagDefineAnimationFrames2:
                        return "TagDefineAnimationFrames2";
                        
                    case names::TagDefineSounds:
                        return "TagDefineSounds";
                        
                    case names::TagDefineAtlas3:
                        return "TagDefineAtlas3";
                        
                    case names::TagDefineAtlas4:
                        return "TagDefineAtlas4";
                        
                    case names::TagDefineTimeline2:
                        return "TagDefineTimeline2";
                        
                    case names::TagDefineExternalObjects:
                        return "TagDefineExternalObjects";
                        
                    case names::TagDefineAnimationFrames3:
                        return "TagDefineAnimationFrames3";
                        
                    case names::TagDefineTimeline3:
                        return "TagDefineTimeline3";
                        
                    case names::TagDefineExternalObjects2:
                        return "TagDefineExternalObjects2";
                        
                    default:
                        return "This tag is not yet implemented";
                }
            }
        };
    };
};
