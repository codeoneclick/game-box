//
//  ces_text_system.hpp
//  gbCore
//
//  Created by sergey.sergeev on 1/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef ces_text_system_h
#define ces_text_system_h

#include "ces_system.h"

namespace gb
{
    class ces_text_system : public ces_system
    {
    private:
        
    protected:
        
        void on_feed_start(f32 deltatime);
        void on_feed(const ces_entity_shared_ptr& entity, f32 deltatime);
        void on_feed_end(f32 deltatime);
        
        void update_recursively(const ces_entity_shared_ptr& entity);
        
        mesh_shared_ptr convert_text_to_geometry(const std::string& text);
        
    public:
        
        ces_text_system();
        ~ces_text_system();
    };
};

#endif