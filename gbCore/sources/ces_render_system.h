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
    class render_pipeline;
    class graphics_context;
    class ces_render_system : public ces_system
    {
    private:
        
        std::shared_ptr<render_pipeline> m_render_pipeline;
        
    protected:
        
        void on_feed_start(f32 deltatime);
        void on_feed(const std::shared_ptr<ces_entity>& entity, f32 deltatime);
        void on_feed_end(f32 deltatime);
        
    public:
        
        ces_render_system(const std::shared_ptr<graphics_context>& graphic_context, bool is_offscreen);
        ~ces_render_system(void);
        
        std::shared_ptr<render_pipeline> get_render_pipeline(void) const;
    };
};


#endif
