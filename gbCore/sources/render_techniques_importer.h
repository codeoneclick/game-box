//
//  render_techniques_importer.h
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef render_techniques_importer_h
#define render_techniques_importer_h

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
    class render_technique_main;
    class render_technique_ws;
    class render_technique_ss;
    
    class render_techniques_importer
    {
    private:
        
    protected:
        
        std::shared_ptr<graphics_context> m_graphics_context;
        bool m_offscreen;
        std::map<std::string, std::shared_ptr<render_technique_ws>> m_ws_render_techniques;
        std::map<std::string, std::shared_ptr<render_technique_ss>> m_ss_render_techniques;
        std::shared_ptr<render_technique_main> m_main_render_technique;
        
    public:
        
        render_techniques_importer(const std::shared_ptr<graphics_context>& graphics_context, bool is_offscreen);
        virtual ~render_techniques_importer();
        
        void create_main_render_technique(const material_shared_ptr& material);
        
        void save_texture(const texture_shared_ptr& texture, const std::string& filename, ui32 width, ui32 height);
        
        void add_ws_render_technique(const std::string& technique_name, i32 technique_index,
                                     const std::shared_ptr<render_technique_ws>& technique);
        void remove_ws_render_technique(const std::string& technique_name, i32 technique_index);
        
        void add_ss_render_technique(const std::string& technique_name, const std::shared_ptr<render_technique_ss>& technique);
        void remove_ss_render_technique(const std::string& technique_name);
    
        bool is_render_technique_supported(const std::string& technique_name);
    };
};

#endif
