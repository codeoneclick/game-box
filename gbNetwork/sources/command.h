//
//  command.h
//  gbNetwork
//
//  Created by serhii serhiiv on 12/12/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "net_declarations.h"

namespace gb
{
    namespace net
    {
        class command_pimpl;
        class command
        {
        public:
            
            static const i32 k_header_size;
            
        private:
            
            std::shared_ptr<command_pimpl> m_pimpl;
            
        protected:
            
            static std::set<uintptr_t> g_guids_container;
            
            std::streambuf& get_buffer();
            
        public:
            
            CTTI_CLASS_GUID(command, command::g_guids_container)
            
            command();
            virtual ~command();
            
            virtual std::streambuf& serialize() = 0;
            virtual void deserialize(std::streambuf&& buffer, i32 size) = 0;
        };
    };
};
