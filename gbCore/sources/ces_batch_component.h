//
//  ces_batch_component.h
//  gbCore
//
//  Created by sergey.sergeev on 10/28/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#ifndef ces_batch_component_h
#define ces_batch_component_h

#include "ces_base_component.h"
#include "batch.h"

namespace gb
{
    class ces_batch_component : public ces_base_component
    {
    private:
        
    protected:
        
        batch_shared_ptr m_batch;
        std::string m_guid;
        
    public:
        
        ces_batch_component(const std::string& guid);
        ~ces_batch_component();
        
        batch::e_batch_render_state get_render_state(const std::string& guid) const;
        void set_render_state(const std::string& guid, batch::e_batch_render_state state);
        
        void set_batch(const batch_shared_ptr& batch);
        batch_shared_ptr get_batch() const;
        
        const std::string& get_guid() const;
        
        void cleanup();
    };
};

#endif
