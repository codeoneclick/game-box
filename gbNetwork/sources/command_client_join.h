//
//  command_client_join.h
//  gbNetwork
//
//  Created by serhii serhiiv on 4/12/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "command.h"

namespace gb
{
    namespace net
    {
        class command_client_join : public command
        {
        private:
            
        protected:
            
            i32 m_udid;
            
        public:
            
            command_client_join();
            command_client_join(i32 udid);
            ~command_client_join();
            
            static command_client_join_shared_ptr create(const std::shared_ptr<std::streambuf>& buffer);
            void init() override;
            
            std::shared_ptr<std::streambuf> serialize() override;
            void deserialize(const std::shared_ptr<std::streambuf>& buffer) override;
            
            std::property_ro<i32> udid;
        };
    };
};
