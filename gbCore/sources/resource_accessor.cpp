#include "resource_accessor.h"
#include "resource.h"

namespace gb
{
    resource_accessor::resource_accessor() : m_thread_executed(true)
    {
        m_thread = std::thread(&resource_accessor::on_thread_update, this);
    }

    resource_accessor::~resource_accessor()
    {
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            m_thread_executed = false;
        }
        m_work_available.notify_all();
        if(m_thread.joinable()) m_thread.join();
        // Worker is stopped before operations/resources are released.
        m_operationsQueue.clear();
        m_resources.clear();
    }

    void resource_accessor::on_thread_update()
    {
#if defined(__IOS__) || defined(__OSX__) || defined(__TVOS__)
        pthread_setname_np("gb.core.resources.accessor");
#endif
        for(;;)
        {
            resource_loading_operation_shared_ptr operation;
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_work_available.wait(lock, [this]() { return !m_thread_executed || !m_operations_to_serialize.empty(); });
                if(!m_thread_executed) return;
                operation = m_operations_to_serialize.front();
                m_operations_to_serialize.pop();
            }
            operation->serialize();
            {
                std::lock_guard<std::mutex> guard(m_mutex);
                m_operations_to_commit.push(operation);
            }
        }
    }

    void resource_accessor::on_update(f32 deltatime)
    {
        std::queue<resource_loading_operation_shared_ptr> ready;
        std::queue<std::tuple<resource_shared_ptr, bool>> callbacks;
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            ready.swap(m_operations_to_commit);
            callbacks.swap(m_resources_need_to_callback);
        }
        while(!ready.empty())
        {
            const auto operation = ready.front();
            ready.pop();
            if(operation->get_status() == e_resource_loading_operation_status_waiting) operation->commit();
            resource_shared_ptr resource;
            {
                std::lock_guard<std::mutex> guard(m_mutex);
                m_operationsQueue.erase(operation->get_guid());
                const auto cached = m_resources.find(operation->get_guid());
                assert(cached != m_resources.end());
                if(cached != m_resources.end()) resource = cached->second;
            }
            if(resource) resource->on_resource_loaded(operation->get_status() == e_resource_loading_operation_status_success);
        }
        // Callbacks run outside the queue mutex and may request more resources.
        while(!callbacks.empty())
        {
            const auto callback = callbacks.front();
            callbacks.pop();
            std::get<0>(callback)->on_resource_loaded(std::get<1>(callback));
        }
    }

    void resource_accessor::add_custom_resource(const std::string& guid, const resource_shared_ptr& resource)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_resources[guid] = resource;
    }
}
