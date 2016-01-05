//
//  render_pipeline.h
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef render_pipeline_h
#define render_pipeline_h

#include "render_techniques_importer.h"
#include "render_techniques_accessor.h"
#include "declarations.h"

namespace gb
{
    class render_pipeline :
    public render_techniques_importer,
    public render_techniques_accessor
    {
    private:
        
    protected:

    public:
        
        render_pipeline(const std::shared_ptr<graphics_context>& graphic_context, bool is_offscreen);
        ~render_pipeline(void);
        
        void on_draw_begin();
        void on_draw(const ces_entity_shared_ptr& entity);
        void on_draw_end();
        
        std::shared_ptr<texture> get_technique_texture(const std::string& technique_name);
        std::shared_ptr<material> get_technique_material(const std::string& technique_name);
        
        ui32 get_screen_width(void);
        ui32 get_screen_height(void);
        std::shared_ptr<graphics_context> get_graphics_context(void) const;
    };
};

#endif
