//
//  thread_operations_pool.cpp
//  gbCore
//
//  Created by sergey.sergeev on 10/28/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#include "thread_operations_pool.h"

namespace gb
{
    std::shared_ptr<thread_operations_pool> thread_operations_pool::m_shared_instance = nullptr;
    
    std::shared_ptr<thread_operations_pool> thread_operations_pool::shared_instance()
    {
        if(!m_shared_instance)
        {
            m_shared_instance = std::make_shared<thread_operations_pool>();
        }
        return m_shared_instance;
    }
    
    thread_operations_pool::thread_operations_pool()
    {
        m_is_running = 1;
        for(ui32 index = 0; index < k_max_threads_pet_queue; ++index)
        {
            m_threads.at(index) = std::thread([this, index]() {
                thread_operations_pool::update_thread(index);
            });
        }
    }
    
    thread_operations_pool::~thread_operations_pool()
    {
        m_is_running = 0;
        for(ui32 index = 0; index < k_max_threads_pet_queue; ++index)
        {
            m_threads.at(index).join();
        }
    }
    
    void thread_operations_pool::add_operation(const thread_operation_shared_ptr& operation, thread_operation::e_thread_operation_queue operation_queue)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        ui32 thread_id = 0;
        size_t min_operations_count = m_operations.at(0).at(operation_queue).size();
        for (ui32 i = 0; i < m_operations.size(); ++i)
        {
            if(m_operations.at(i).at(operation_queue).size() < min_operations_count)
            {
                min_operations_count = m_operations.at(i).at(operation_queue).size();
                thread_id = i;
            }
        }
        m_operations.at(thread_id).at(operation_queue).push(operation);
    }
    
    void thread_operations_pool::update_thread(ui32 thread_id)
    {
        while (m_is_running)
        {
            if(!thread_operations_pool::is_queue_empty(thread_id, thread_operation::e_thread_operation_queue_background))
            {
                thread_operation_shared_ptr operation = thread_operations_pool::next_operation(thread_id, thread_operation::e_thread_operation_queue_background);
                if(!operation)
                {
                    continue;
                }
                thread_operation_shared_ptr dependency_operation = operation->next_operation();
                if(dependency_operation->get_operation_queue_name() == thread_operation::e_thread_operation_queue_background)
                {
                    if(!dependency_operation->is_canceled())
                    {
                        dependency_operation->execute();
                    }
                    operation->pop_operation();
                }
                if(dependency_operation == operation)
                {
                    thread_operations_pool::pop_operation(thread_id, thread_operation::e_thread_operation_queue_background);
                }
            }
            
            if(!thread_operations_pool::is_queue_empty(thread_id, thread_operation::e_thread_operation_queue_main))
            {
                thread_operation_shared_ptr operation = thread_operations_pool::next_operation(thread_id, thread_operation::e_thread_operation_queue_main);
                if(!operation)
                {
                    continue;
                }
                thread_operation_shared_ptr dependency_operation = operation->next_operation();
                if(dependency_operation->get_operation_queue_name() == thread_operation::e_thread_operation_queue_background)
                {
                    if(!dependency_operation->is_canceled())
                    {
                        dependency_operation->execute();
                    }
                    operation->pop_operation();
                }
            }
        }
    }
    
    void thread_operations_pool::update()
    {
        for(ui32 thread_id = 0; thread_id < m_threads.size(); ++thread_id)
        {
            if(!thread_operations_pool::is_queue_empty(thread_id, thread_operation::e_thread_operation_queue_main))
            {
                thread_operation_shared_ptr operation = thread_operations_pool::next_operation(thread_id, thread_operation::e_thread_operation_queue_main);
                if(!operation)
                {
                    return;
                }
                thread_operation_shared_ptr dependency_operation = operation->next_operation();
                if(dependency_operation->get_operation_queue_name() == thread_operation::e_thread_operation_queue_main)
                {
                    if(!dependency_operation->is_canceled())
                    {
                        dependency_operation->execute();
                    }
                    operation->pop_operation();
                }
                if(dependency_operation == operation)
                {
                    thread_operations_pool::pop_operation(thread_id, thread_operation::e_thread_operation_queue_main);
                }
            }
            
            if(!thread_operations_pool::is_queue_empty(thread_id, thread_operation::e_thread_operation_queue_background))
            {
                thread_operation_shared_ptr operation = thread_operations_pool::next_operation(thread_id, thread_operation::e_thread_operation_queue_background);
                if(!operation)
                {
                    return;
                }
                thread_operation_shared_ptr dependency_operation = operation->next_operation();
                if(dependency_operation->get_operation_queue_name() == thread_operation::e_thread_operation_queue_main)
                {
                    if(!dependency_operation->is_canceled())
                    {
                        dependency_operation->execute();
                    }
                    operation->pop_operation();
                }
            }
        }
    }
    
    thread_operation_shared_ptr thread_operations_pool::next_operation(ui32 thread_id, thread_operation::e_thread_operation_queue operation_queue)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        thread_operation_shared_ptr operation = m_operations.at(thread_id).at(operation_queue).empty() ? nullptr : m_operations.at(thread_id).at(operation_queue).front();
        return operation;
    }
    
    void thread_operations_pool::pop_operation(ui32 threadId, thread_operation::e_thread_operation_queue operation_queue)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_operations.at(threadId).at(operation_queue).pop();
    }
    
    bool thread_operations_pool::is_queue_empty(ui32 threadId, thread_operation::e_thread_operation_queue operation_queue)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        return m_operations.at(threadId).at(operation_queue).empty();
    }
}