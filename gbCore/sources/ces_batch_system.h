//
//  ces_batch_system.h
//  gbCore
//
//  Created by sergey.sergeev on 10/28/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#ifndef ces_batch_system_h
#define ces_batch_system_h

#include "ces_system.h"

namespace gb
{
    class ces_batch_system : public ces_system
    {
    private:
        
        std::unordered_map<std::string, batch_shared_ptr> m_batches;
        
    protected:
        
        void on_feed_start(f32 deltatime);
        void on_feed(const ces_entity_shared_ptr& entity, f32 deltatime);
        void on_feed_end(f32 deltatime);
        
    public:
        
        ces_batch_system();
        ~ces_batch_system();
    };
};

#endif
