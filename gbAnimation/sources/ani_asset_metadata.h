#pragma once

#include "main_headers.h"
#include "declarations.h"
#include "tags_defines.h"

namespace gb
{
    namespace anim
    {
        class ani_header;
        class ani_resource_metadata;
        class ani_asset_metadata
        {
        private:
            
            std::string m_filename;
            std::shared_ptr<ani_header> m_header;
            ani_timelines_t	m_timelines;
            std::shared_ptr<ani_timeline> m_root_timeline;
            std::shared_ptr<ani_texture_atlas> m_current_texture_atlas;

            void set_root_timeline(const std::shared_ptr<ani_timeline>& timeline);
            
        public:
            
            bool init(const std::string& filepath);
            
            void push_timeline(ui32 timeline_id_reference, const std::shared_ptr<ani_timeline>& timeline);
           
            void set_header(const std::shared_ptr<ani_header>& header);
            const std::shared_ptr<ani_header> get_header() const;
            
            bool set_root_timeline(const std::string& name);
            bool set_root_timeline(ui32 id);
            std::shared_ptr<ani_timeline> get_root_timeline() const;
            std::shared_ptr<ani_timeline> get_timeline_by_name(const std::string& name) const;
            
            const ani_timelines_t& get_timelines() const;
            ani_timelines_t& get_timelines();
            
            static std::shared_ptr<ani_asset_metadata> create(const std::string& filepath);
            
            ani_asset_metadata();
            ~ani_asset_metadata();
            
            const std::string& get_filename() const;
        };
    };
};

