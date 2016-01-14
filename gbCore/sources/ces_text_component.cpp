//
//  ces_text_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_text_component.h"

namespace gb
{
    ces_text_component::ces_text_component() :
    m_is_text_changed(false),
    m_text("undefined")
    {
        m_type = e_ces_component_type_text;
    }
    
    ces_text_component::~ces_text_component()
    {
        
    }
    
    const std::array<i32, 64> ces_text_component::get_letters_sizes()
    {
        static std::array<i32, 64> letters_sizes =
        {
            36, 29, 30, 34, 25, 25, 34, 33,
            11, 20, 31, 24, 48, 35, 39, 29,
            42, 31, 27, 31, 34, 35, 46, 35,
            31, 27, 30, 26, 28, 26, 31, 28,
            28, 28, 29, 29, 14, 24, 30, 18,
            26, 14, 14, 14, 25, 28, 31, 0,
            0, 38, 39, 12, 36, 34, 0, 0,
            0, 38, 0, 0, 0, 0, 0, 0
        };
        return letters_sizes;
    }
    
    i32 ces_text_component::convert_symbol_to_index(i32 c_val)
    {
        i32 index = -1;
        
        if(c_val > 64 && c_val < 91) // A-Z
        {
            index = c_val - 65;
        }
        else if(c_val > 96 && c_val < 123) // a-z
        {
            index = c_val - 97;
        }
        else if(c_val > 47 && c_val < 58) // 0-9
        {
            index = c_val - 48 + 26;
        }
        else if(c_val == 43) // +
        {
            index = 38;
        }
        else if(c_val == 45) // -
        {
            index = 39;
        }
        else if(c_val == 33) // !
        {
            index = 36;
        }
        else if(c_val == 63) // ?
        {
            index = 37;
        }
        else if(c_val == 61) // =
        {
            index = 40;
        }
        else if(c_val == 58) // :
        {
            index = 41;
        }
        else if(c_val == 46) // .
        {
            index = 42;
        }
        else if(c_val == 44) // ,
        {
            index = 43;
        }
        else if(c_val == 42) // *
        {
            index = 44;
        }
        else if(c_val == 36) // $
        {
            index = 45;
        }
        return index;
    }
    
    void ces_text_component::set_text(const std::string& text)
    {
        m_is_text_changed = true;
        m_text = text;
    }
    
    std::string ces_text_component::get_text() const
    {
        return m_text;
    }
    
    bool ces_text_component::is_text_changed() const
    {
        return m_is_text_changed;
    }
    
    void ces_text_component::reset()
    {
        m_is_text_changed = false;
    }
    
    glm::vec2 ces_text_component::get_min_bound() const
    {
        return glm::vec2(0.f);
    }
    
    glm::vec2 ces_text_component::get_max_bound() const
    {
        f32 text_length = 0.f;
        for(const char& symbol : m_text)
        {
            i32 c_val = static_cast<i32>(symbol);
            i32 index = ces_text_component::convert_symbol_to_index(c_val);
            text_length += ces_text_component::get_letters_sizes()[index] / 2;
        }
        return glm::vec2(text_length, 32.f);
    }
}