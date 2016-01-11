//
//  ces_render_system.h
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef ces_render_system_h
#define ces_render_system_h

#include "ces_system.h"

namespace gb
{
    class ces_render_system : public ces_system
    {
    private:
        
        std::shared_ptr<render_pipeline> m_render_pipeline;
        
    protected:
        
        void on_feed_start(f32 deltatime);
        void on_feed(const ces_entity_shared_ptr& entity, f32 deltatime);
        void on_feed_end(f32 deltatime);
        
        void draw_recursively(const ces_entity_shared_ptr& entity, const std::string &technique_name, i32 technique_pass);
        
    public:
        
        ces_render_system(const std::shared_ptr<graphics_context>& graphic_context, bool is_offscreen);
        ~ces_render_system(void);
        
        std::shared_ptr<render_pipeline> get_render_pipeline(void) const;
    };
};

#endif
