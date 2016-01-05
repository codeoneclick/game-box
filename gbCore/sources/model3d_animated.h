//
//  model3d_animated.h
//  gbCore
//
//  Created by sergey.sergeev on 8/28/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef model3d_animated_h
#define model3d_animated_h

#include "model3d_static.h"
#include "configuration_accessor.h"

namespace gb
{
    class model3d_animated : public model3d_static
    {
    private:
        
    protected:
        
    public:
        
        model3d_animated();
        ~model3d_animated();
        
        void set_animation_mixer(const animation_mixer_shared_ptr& animation_mixer);
        void set_animation(const std::string& name);
        void create_animation_linkage(const model_configuration_shared_ptr& configuration);
    };
};

#endif
