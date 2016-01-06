//
//  ces_text_component.h
//  gbCore
//
//  Created by sergey.sergeev on 1/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef ces_text_component_h
#define ces_text_component_h

#include "ces_base_component.h"

namespace gb
{
    class ces_text_component : public ces_base_component
    {
    private:
        
    protected:
        
        std::string m_text;
        bool m_is_text_changed;
        
    public:
        
        ces_text_component();
        ~ces_text_component();
        
        void set_text(const std::string& text);
        std::string get_text() const;
        
        bool is_text_changed() const;
        
        void reset();
    };
};


#endif
