//
//  thread_operations_pool.h
//  gbCore
//
//  Created by sergey.sergeev on 10/28/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#ifndef thread_operations_pool_h
#define thread_operations_pool_h

#include "main_headers.h"
#include "declarations.h"
#include "thread_operation.h"

namespace gb
{
#define k_max_threads_pet_queue 5
    
    class thread_operations_pool
    {
    private:
        
    protected:
        
        static std::shared_ptr<thread_operations_pool> m_shared_instance;
        
        std::array<std::array<std::queue<thread_operation_shared_ptr>, thread_operation::e_thread_operation_queue_max>, k_max_threads_pet_queue> m_operations;
        
        ui8 m_is_running;
        std::array<std::thread, k_max_threads_pet_queue> m_threads;
        std::mutex m_mutex;
        
        thread_operation_shared_ptr next_operation(ui32 thread_id, thread_operation::e_thread_operation_queue operation_queue);
        void pop_operation(ui32 thread_id, thread_operation::e_thread_operation_queue operation_queue);
        bool is_queue_empty(ui32 thread_id, thread_operation::e_thread_operation_queue operation_queue);
        
        void update_thread(ui32 thread_id);
        
    public:
        
        thread_operations_pool();
        ~thread_operations_pool();
        
        static std::shared_ptr<thread_operations_pool> shared_instance();
        
        void add_operation(const thread_operation_shared_ptr& operation, thread_operation::e_thread_operation_queue operation_queue);
        
        void update();
    };

};

#endif
