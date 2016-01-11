//
//  ces_light_compoment.h
//  gbCore
//
//  Created by sergey.sergeev on 1/11/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef ces_light_compoment_h
#define ces_light_compoment_h

#include "ces_base_component.h"

namespace gb
{
    class ces_light_compoment : public ces_base_component
    {
    private:
        
    protected:
        
        std::list<ces_entity_shared_ptr> m_shadow_casters;
        
    public:
        
        ces_light_compoment();
        ~ces_light_compoment();
        
        void add_shadow_caster(const ces_entity_shared_ptr& shadow_caster);
        const std::list<ces_entity_shared_ptr>& get_shadow_casters() const;
        
        void cleanup();
    };
};

#endif
