//
//  thread_operation.cpp
//  gbCore
//
//  Created by sergey.sergeev on 10/28/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#include "thread_operation.h"
#include "thread_operations_pool.h"

namespace gb
{
    thread_operation::thread_operation(e_thread_operation_queue operation_queue) :
    m_operation_queue(operation_queue),
    m_execution_callback(nullptr),
    m_cancel_callback(nullptr),
    m_status(e_thread_operation_status_none)
    {
        
    }
    
    thread_operation::~thread_operation()
    {
        m_execution_callback = nullptr;
        m_cancel_callback = nullptr;
        while (!m_dependecies.empty())
        {
            m_dependecies.pop_front();
        }
    }
    
    void thread_operation::set_execution_callback(std::function<void()> callback)
    {
        m_execution_callback = callback;
    }
    
    void thread_operation::set_cancel_callback(std::function<void()> callback)
    {
        m_cancel_callback = callback;
    }
    
    void thread_operation::add_dependency(const thread_operation_shared_ptr& operation)
    {
        std::lock_guard<std::recursive_mutex> guard(m_mutex);
        m_dependecies.push_back(operation);
    }
    
    thread_operation_shared_ptr thread_operation::next_operation()
    {
        std::lock_guard<std::recursive_mutex> guard(m_mutex);
        if(!thread_operation::is_queue_empty())
        {
            thread_operation_shared_ptr operation = m_dependecies.front();
            return operation->next_operation();
        }
        return shared_from_this();
    }
    
    bool thread_operation::pop_operation()
    {
        std::lock_guard<std::recursive_mutex> guard(m_mutex);
        if(!thread_operation::is_queue_empty())
        {
            thread_operation_shared_ptr operation = m_dependecies.front();
            if(operation->pop_operation())
            {
                m_dependecies.pop_front();
            }
        }
        else
        {
            return true;
        }
        return false;
    }
    
    void thread_operation::execute()
    {
        assert(m_execution_callback);
        if(m_execution_callback)
        {
            m_status |= e_thread_operation_status_executed;
            m_execution_callback();
            m_status |= e_thread_operation_status_completed;
        }
    }
    
    void thread_operation::add_to_execution_queue()
    {
        thread_operations_pool::shared_instance()->add_operation(shared_from_this(), m_operation_queue);
    }
    
    void thread_operation::cancel()
    {
        for(const auto& operation : m_dependecies)
        {
            operation->cancel();
        }
        if(m_cancel_callback)
        {
            m_cancel_callback();
        }
        if(!thread_operation::is_executed())
        {
            m_status |= e_thread_operation_status_completed;
        }
        m_status |= e_thread_operation_status_canceled;
    }
    
    bool thread_operation::is_executed() const
    {
        return m_status & e_thread_operation_status_executed;
    }
    
    bool thread_operation::is_canceled() const
    {
        return m_status & e_thread_operation_status_canceled;
    }
    
    bool thread_operation::is_completed() const
    {
        return m_status & e_thread_operation_status_completed;
    }
    
    bool thread_operation::is_queue_empty() const
    {
        std::lock_guard<std::recursive_mutex> guard(m_mutex);
        return m_dependecies.empty();
    }
    
    thread_operation::e_thread_operation_queue thread_operation::get_operation_queue_name() const
    {
        return m_operation_queue;
    }
}