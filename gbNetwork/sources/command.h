//
//  command.h
//  gbNetwork
//
//  Created by serhii serhiiv on 12/12/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "asio.hpp"
#include "net_declarations.h"

namespace gb
{
    namespace net
    {
        class command
        {
        public:
            
            static const i32 k_header_size;
            
        private:
            
        protected:
            
            i32 m_id;
            asio::streambuf m_buffer;
            
        public:
            
            command();
            virtual ~command();
            
            virtual asio::streambuf& serialize() = 0;
            virtual void deserialize(asio::streambuf&& buffer) = 0;
        };
    };
};
