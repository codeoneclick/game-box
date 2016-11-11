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
        
        template<typename T_RESOURCE, typename T_LOADING_OPERATION>
        std::shared_ptr<T_RESOURCE> get_resource(const std::string& filename, bool sync = false);
    };
};

#include "resource_accessor.hpp"

#endif
