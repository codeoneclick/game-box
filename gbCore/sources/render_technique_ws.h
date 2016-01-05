//
//  render_technique_ws.h
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef render_technique_ws_h
#define render_technique_ws_h

#include "render_technique_base.h"

namespace gb
{
    class render_technique_ws : public render_technique_base
    {
    private:
        
    protected:
        
        texture_shared_ptr m_color_attachment_texture;
        texture_shared_ptr m_depth_attachment_texture;
        
        std::vector<std::vector<std::queue<ces_entity_shared_ptr>>> m_entities;
        
        i32 m_num_passes;
        
    public:
        
        render_technique_ws(ui32 width, ui32 height, const std::string& name, ui32 index,
                            bool is_depth_compare_mode_enabled, i32 num_passes);
        ~render_technique_ws();
        
        texture_shared_ptr get_color_attachment_texture() const;
        texture_shared_ptr get_depth_attachment_texture() const;
        
        void bind();
        void unbind();
        void draw();
        
        void add_entity(const ces_entity_shared_ptr& entity, i32 technique_pass, const material_shared_ptr& material);
        
        bool is_need_to_draw() const;
        
        i32 get_num_passes() const;
    };
};

#endif
