//
//  ces_ocean_component.h
//  gbCore
//
//  Created by sergey.sergeev on 10/13/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#ifndef ces_ocean_component_h
#define ces_ocean_component_h

#include "ces_base_component.h"

namespace gb
{
    class ces_ocean_component : public ces_base_component
    {
    private:
        
    protected:
        
        f32 m_wave_generator_time;
        f32 m_wave_generator_interval;
        
    public:
        
        ces_ocean_component();
        ~ces_ocean_component();
        
        void set_wave_generator_interval(f32 wave_generator_interval);
        f32 get_wave_generator_time() const;
        
        void update(f32 deltatime);
    };
};

#endif
