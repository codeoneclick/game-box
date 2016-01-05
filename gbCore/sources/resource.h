//
//  resource.h
//  gbCore
//
//  Created by sergey.sergeev on 8/12/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef resource_h
#define resource_h

#include "main_headers.h"
#include "declarations.h"
#include "resource_types.h"

namespace gb
{
    struct resource_transfering_data
    {
    protected:
        
        e_resource_transfering_data_type m_type;
        resource_transfering_data();
        
    public:
        
        virtual ~resource_transfering_data() = default;
        
        e_resource_transfering_data_type get_type() const;
    };
    
    class resource_loading_interface
    {
    private:
        
    protected:
        
        resource_loading_interface() = default;
        
    public:
        
        virtual ~resource_loading_interface() = default;
        
        virtual void on_resource_loaded(const resource_shared_ptr& resource, bool success) = 0;
    };
    
    class resource : public std::enable_shared_from_this<resource>
    {
    public:
        
        typedef std::function<void(const resource_shared_ptr&, bool success)> f_resource_loading_callback;
        
    private:
        
        friend class resource_serializer;
        friend class resource_commiter;
        friend class resource_accessor;
        std::string m_guid;
        
    protected:
        
        ui8 m_status;
        e_resource_type m_type;
        std::set<resource_loading_interface_shared_ptr> m_listeners;
        std::set<std::shared_ptr<f_resource_loading_callback>> m_callbacks;
        
        resource(e_resource_type type, const std::string& guid);
        
        virtual void on_transfering_data_serialized(const std::shared_ptr<resource_transfering_data>& data) = 0;
        virtual void on_transfering_data_commited(const std::shared_ptr<resource_transfering_data>& data) = 0;
        void on_resource_loaded(bool success);
        
    public:
        
        virtual ~resource() = default;
        
        const std::string& get_guid() const;
        e_resource_type get_type() const;
        
        virtual bool is_loaded() const;
        virtual bool is_commited() const;
        
        void add_resource_loading_listener(const resource_loading_interface_shared_ptr& listener);
        void remove_resource_loading_listener(const resource_loading_interface_shared_ptr& listener);
        
        void add_resource_loading_callback(const std::shared_ptr<f_resource_loading_callback>& callback);
        void remove_resource_loading_callback(const std::shared_ptr<f_resource_loading_callback>& callback);
    };
};

#endif
