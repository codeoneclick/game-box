//
//  resource_accessor.h
//  gbCore
//
//  Created by sergey.sergeev on 8/24/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef resource_accessor_h
#define resource_accessor_h

#include "main_headers.h"
#include "declarations.h"
#include "game_loop.h"

namespace gb
{
    class resource_accessor : public i_game_loop
    {
    private:
        
    protected:
        
        std::unordered_map<std::string, resource_loading_operation_shared_ptr> m_operationsQueue;
        std::unordered_map<std::string, resource_shared_ptr> m_resources;
        std::queue<std::tuple<resource_shared_ptr, bool>> m_resources_need_to_callback;
        
        std::mutex m_mutex;
        std::thread m_thread;
        bool m_thread_executed;
        
        void on_thread_update();
        void on_update(f32 deltatime);
        
    public:
        
        resource_accessor();
        ~resource_accessor();
        
        void add_custom_resource(const std::string& guid, const resource_shared_ptr& resource);
        
        shader_shared_ptr get_shader(const std::string& vs_filename,
                                     const std::string& fs_filename, bool sync = false);
        
        texture_shared_ptr get_texture(const std::string& filename, bool sync = false, bool mandatory = true);
        cubemap_texture_shared_ptr get_cubemap_texture(const std::string& xpositive, const std::string& xnegative,
                                                       const std::string& ypositive, const std::string& ynegative,
                                                       const std::string& zpositive, const std::string& znegative,
                                                       bool sync = false);
        
        mesh_shared_ptr get_mesh(const std::string& filename, bool sync = false);
        
        sequence_shared_ptr get_animation(const std::string& filename, bool sync = false);
    };
};

#endif
