#pragma once

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
    namespace anim
    {
        class ani_texture_atlas_element;
        class ani_texture_atlas
        {
        public:
            
            struct ani_atlas_metadata
            {
                struct source
                {
                    std::string m_source;
                    f32 m_csf;
                };
                
                typedef std::vector<source> sources_t;
                
                sources_t m_sources;
                ui32 m_id;
            };
            
            static bool compare_atlases_by_id(const ani_atlas_metadata& value_01, const ani_atlas_metadata& value_02);
            
            typedef std::vector<ani_atlas_metadata> atlas_metadatas_t;
            typedef std::map<ui32, std::shared_ptr<ani_texture_atlas_element>> elements_t;
            
            ani_texture_atlas();
            
        private:
            
            f32 m_scale;
            atlas_metadatas_t m_atlas_metadatas;
            elements_t m_elements;
            
        public:
            
            ~ani_texture_atlas();
            
            void push_atlas_metadata(const ani_atlas_metadata& metadata);
            void push_element(ui32 index, const std::shared_ptr<ani_texture_atlas_element>& element);
            bool swap_element(ui32 index, const std::shared_ptr<ani_texture_atlas_element>& element);
            
            void set_scale(f32 value);
            f32 get_scale() const;
            
            const elements_t& get_elements() const;
            const atlas_metadatas_t& get_atlas_metadatas() const;
        };
    };
};
