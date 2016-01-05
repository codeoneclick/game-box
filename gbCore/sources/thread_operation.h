//
//  thread_operation.h
//  gbCore
//
//  Created by sergey.sergeev on 10/28/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#ifndef thread_operation_h
#define thread_operation_h

#include "main_headers.h"
#include "declarations.h"

namespace gb
{

class thread_operation : public std::enable_shared_from_this<thread_operation>
{
public:
    
    enum e_thread_operation_queue
    {
        e_thread_operation_queue_main = 0,
        e_thread_operation_queue_background,
        e_thread_operation_queue_max
    };

private:
    
    enum e_thread_operation_status
    {
        e_thread_operation_status_none = 1,
        e_thread_operation_status_executed = 2,
        e_thread_operation_status_completed = 4,
        e_thread_operation_status_canceled = 8,
    };
    
protected:
    
    e_thread_operation_queue m_operation_queue;
    
    std::deque<thread_operation_shared_ptr> m_dependecies;
    
    std::function<void()> m_execution_callback;
    std::function<void()> m_cancel_callback;
    
    mutable std::recursive_mutex m_mutex;
    ui8 m_status;
    
public:
    
    thread_operation(e_thread_operation_queue operation_queue);
    ~thread_operation();
    
    void set_execution_callback(std::function<void()> callback);
    void set_cancel_callback(std::function<void()> callback);
    
    void add_dependency(const thread_operation_shared_ptr& operation);
    void execute();
    void add_to_execution_queue();
    void cancel();
    
    bool is_executed() const;
    bool is_canceled() const;
    bool is_completed() const;
    
    e_thread_operation_queue get_operation_queue_name() const;
    thread_operation_shared_ptr next_operation();
    bool pop_operation();
    bool is_queue_empty() const;
};
};


#endif
