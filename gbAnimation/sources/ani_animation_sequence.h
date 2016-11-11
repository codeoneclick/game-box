#pragma once

#include "main_headers.h"
#include "declarations.h"
#include "tags_defines.h"

namespace gb
{
    namespace anim
    {
        class ani_animation_sequence
        {
        public:
            
            std::string m_name;
            ui32 m_start_frame;
            ui32 m_end_frame;
            
            inline ui32 length() const
            {
                assert(m_end_frame > m_start_frame);
                return m_end_frame - m_start_frame;
            }
        };
    };
};
