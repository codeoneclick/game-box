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
        ~render_pipeline();
        
        void on_draw_begin();
        void on_draw_end();
        
        texture_shared_ptr get_technique_texture(const std::string& technique_name);
        material_shared_ptr get_technique_material(const std::string& technique_name);
        
        ui32 get_screen_width();
        ui32 get_screen_height();
        
        std::shared_ptr<graphics_context> get_graphics_context() const;
        
        const std::list<std::shared_ptr<render_technique_ws>>& get_ws_techniques() const;
    };
};

#endif
