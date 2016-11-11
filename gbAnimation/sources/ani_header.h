#pragma once

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
    namespace anim
    {
        class ani_header
        {
        public:
            
            enum compression
            {
                e_compression_default = 0,
                e_compressed_none = 0x00474146,
                e_compressed_zip = 0x00474143
            };
            
        public:
            
            compression m_compression;
            ui16 m_version;
            ui32 m_file_lenght;
            
            ui16 m_frames_count;
            glm::vec4 m_frame_size;
            glm::vec2 m_pivot;
            
            std::vector<f32> m_scale_values;
            std::vector<f32> m_csf_values;
            
            inline ui16 get_major_version() const
            {
                return m_version & 0x00FF;
            }
            
            inline ui16 get_minor_version() const
            {
                return m_version >> 8;
            }
        };
    };
};

