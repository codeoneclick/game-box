//
//  particle_emitter.h
//  gbCore
//
//  Created by sergey.sergeev on 9/2/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef particle_emitter_h
#define particle_emitter_h

#include "renderable_game_object.h"
#include "configuration_accessor.h"

namespace gb
{
    class particle_emitter : public renderable_game_object
    {
    private:
        
    protected:
        
    public:
        
        particle_emitter();
        ~particle_emitter();
        
        void set_settings(const std::shared_ptr<particle_emitter_configuration>& settings);
    };
};

#endif
