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
#include <condition_variable>

namespace gb
{
#define k_max_threads_pet_queue 5
    
    class thread_operations_pool
    {
    private:
        
    protected:
        
        static std::shared_ptr<thread_operations_pool> m_shared_instance;
        
        std::array<std::array<std::queue<thread_operation_shared_ptr>, thread_operation::e_thread_operation_queue_max>, k_max_threads_pet_queue> m_operations;
        
        bool m_is_running;
        // A root dependency chain belongs to only one executor at a time.
        std::array<std::array<bool, thread_operation::e_thread_operation_queue_max>, k_max_threads_pet_queue> m_claimed = {};
        std::condition_variable m_work_available;
        size_t m_work_generation = 0;
        std::array<std::thread, k_max_threads_pet_queue> m_threads;
        std::mutex m_mutex;
        
        bool process_operation(ui32 thread_id, thread_operation::e_thread_operation_queue operation_queue,
                               thread_operation::e_thread_operation_queue executor_queue);
        
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
