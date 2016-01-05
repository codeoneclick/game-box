//
//  resource_serializer.h
//  gbCore
//
//  Created by Sergey Sergeev on 8/12/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef resource_serializer_h
#define resource_serializer_h

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
    enum e_serializer_status
    {
        e_serializer_status_undefined = 0,
        e_serializer_status_in_progress,
        e_serializer_status_failure,
        e_serializer_status_success
    };
    
    class resource_serializer : public std::enable_shared_from_this<resource_serializer>
    {
    private:
        
    protected:
        
        std::string m_guid;
        resource_shared_ptr m_resource;
        e_serializer_status m_status;
        
        void on_transfering_data_serialized(const resource_transfering_data_shared_ptr& data);
        
    public:
        
        resource_serializer(const std::string& guid,
                            const resource_shared_ptr& resource);
        virtual ~resource_serializer() = default;
        
        std::string get_guid() const;
        e_serializer_status get_status() const;
        
        virtual void serialize() = 0;
        
        static std::shared_ptr<std::istream> open_stream(const std::string &filename, e_serializer_status* status);
        static void close_stream(const std::shared_ptr<std::istream>& stream);
    };
};

#endif
