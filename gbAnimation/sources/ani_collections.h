#pragma once

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
    namespace anim
    {
        class ani_texture_atlas;
        
        enum class e_ani_filter_type : uint32_t
        {
            e_drop_shadow = 0,
            e_drop_blur = 1,
            e_drop_glow = 2,
            e_drop_color_matrix = 6
        };
        
        enum class e_ani_character_type : uint32_t
        {
            e_texture = 0,
            e_textfield,
            e_timeline,
            e_external
        };
        
        enum class e_ani_object_type : uint32_t
        {
            e_none = 0,
            e_movie_clip,
            e_mask
        };
        
        enum class e_ani_rotation : int8_t
        {
            ccw_90 = -1,
            none = 0,
            cw_90 = 1,
        };
        
                
        enum
        {
            e_ani_first_frame_index = 0
        };
        
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
        typedef std::vector<ani_texture_atlas*>                       TextureAtlases_t;
        typedef std::unordered_map<uint32_t, AnimationObjectEx_t>   AnimationMasks_t;      // Object id -> Element Atlas Id, Type
        typedef std::unordered_map<uint32_t, AnimationObjectEx_t>   AnimationObjects_t;    // Object id -> Element Atlas Id, Type
        typedef std::vector<GAFAnimationFrame*>                     AnimationFrames_t;
        
        typedef std::vector<GAFFilterData*>                         Filters_t;
        typedef std::unordered_map<uint32_t, GAFTimeline*>          Timelines_t;
        typedef std::unordered_map<uint32_t, GAFTextData*>          TextsData_t;
        
        typedef std::unordered_map<uint32_t, int>                   CaptureObjects_t;      // Object id -> capture flags
        
        typedef std::unordered_map<std::string, GAFAnimationSequence>         AnimationSequences_t;
        typedef std::multimap<std::string, uint32_t>                          NamedParts_t;
        typedef std::unordered_map<uint32_t, ExternalObject*>                 ExternalObjects_t;
        
        typedef std::unordered_map<uint32_t, GAFSoundInfo*>         SoundInfos_t;
        
        static const uint32_t IDNONE = UINT_MAX;
        
        // Custom properties
        struct CustomProperty
        {
            std::string name;
            std::vector<std::string> possibleValues;
        };
        
        typedef std::vector<CustomProperty> CustomProperties_t;
        
    };
};
