//
//  command_client_connection_established.h
//  gbNetwork
//
//  Created by serhii serhiiv on 1/19/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "command.h"

namespace gb
{
    namespace net
    {
        class command_client_connection_established : public command
        {
        private:
            
        protected:
            
            ui32 m_udid;
            
        public:
            
            command_client_connection_established();
            command_client_connection_established(ui32 udid);
            ~command_client_connection_established();
            
            static command_client_connection_established_shared_ptr create(std::streambuf&& buffer, i32 size);
            
            std::streambuf& serialize() override;
            void deserialize(std::streambuf&& buffer, i32 size) override;
            
            ui32 get_udid() const;
        };
    };
};

