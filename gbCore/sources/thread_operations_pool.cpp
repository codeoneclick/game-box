#include "thread_operations_pool.h"
#include <exception>

namespace gb
{
    std::shared_ptr<thread_operations_pool> thread_operations_pool::m_shared_instance = nullptr;

    std::shared_ptr<thread_operations_pool> thread_operations_pool::shared_instance()
    {
        // Function-local initialization serializes concurrent first use.
        static const auto instance = std::make_shared<thread_operations_pool>();
        return instance;
    }

    thread_operations_pool::thread_operations_pool() : m_is_running(true)
    {
        for(ui32 index = 0; index < k_max_threads_pet_queue; ++index)
            m_threads.at(index) = std::thread([this, index]() { update_thread(index); });
    }

    thread_operations_pool::~thread_operations_pool()
    {
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            m_is_running = false;
        }
        m_work_available.notify_all();
        for(auto& thread : m_threads) if(thread.joinable()) thread.join();
    }

    void thread_operations_pool::add_operation(const thread_operation_shared_ptr& operation,
                                               thread_operation::e_thread_operation_queue operation_queue)
    {
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            if(!m_is_running) return;
            ui32 thread_id = 0;
            auto min_count = m_operations.at(0).at(operation_queue).size();
            for(ui32 i = 1; i < m_operations.size(); ++i)
            {
                const auto count = m_operations.at(i).at(operation_queue).size();
                if(count < min_count) { min_count = count; thread_id = i; }
            }
            m_operations.at(thread_id).at(operation_queue).push(operation);
            ++m_work_generation;
        }
        m_work_available.notify_all();
    }

    bool thread_operations_pool::process_operation(ui32 thread_id,
        thread_operation::e_thread_operation_queue queue, thread_operation::e_thread_operation_queue executor)
    {
        thread_operation_shared_ptr operation;
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            auto& operations = m_operations.at(thread_id).at(queue);
            if(!m_is_running || operations.empty() || m_claimed.at(thread_id).at(queue)) return false;
            m_claimed.at(thread_id).at(queue) = true;
            operation = operations.front();
        }
        // Do not hold the pool mutex while traversing dependencies or calling user code.
        const auto dependency = operation->next_operation();
        bool runnable = dependency->get_operation_queue_name() == executor;
        std::exception_ptr failure;
        if(runnable)
        {
            try
            {
                dependency->execute();
                // A dependency may be shared with a chain on another worker.
                // Its callback must finish before this chain advances.
                runnable = dependency->is_completed();
                if(runnable) operation->pop_operation(dependency);
            }
            catch(...)
            {
                failure = std::current_exception();
                operation->pop_operation(dependency);
            }
        }
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            if(runnable && dependency == operation) m_operations.at(thread_id).at(queue).pop();
            m_claimed.at(thread_id).at(queue) = false;
            if(runnable) ++m_work_generation;
        }
        if(runnable) m_work_available.notify_all();
        if(failure) std::rethrow_exception(failure);
        return runnable;
    }

    void thread_operations_pool::update_thread(ui32 thread_id)
    {
#if defined(__IOS__) || defined(__OSX__) || defined(__TVOS__)
        const auto name = std::string("gb.core.thread.pool.") + std::to_string(thread_id);
        pthread_setname_np(name.c_str());
#endif
        for(;;)
        {
            size_t generation;
            {
                std::lock_guard<std::mutex> guard(m_mutex);
                if(!m_is_running) return;
                generation = m_work_generation;
            }
            const bool background = process_operation(thread_id, thread_operation::e_thread_operation_queue_background,
                                                       thread_operation::e_thread_operation_queue_background);
            const bool main = process_operation(thread_id, thread_operation::e_thread_operation_queue_main,
                                                 thread_operation::e_thread_operation_queue_background);
            if(background || main) continue;
            std::unique_lock<std::mutex> lock(m_mutex);
            m_work_available.wait(lock, [this, generation]() { return !m_is_running || m_work_generation != generation; });
        }
    }

    void thread_operations_pool::update()
    {
        for(ui32 thread_id = 0; thread_id < m_threads.size(); ++thread_id)
        {
            process_operation(thread_id, thread_operation::e_thread_operation_queue_main,
                               thread_operation::e_thread_operation_queue_main);
            process_operation(thread_id, thread_operation::e_thread_operation_queue_background,
                               thread_operation::e_thread_operation_queue_main);
        }
    }
}
