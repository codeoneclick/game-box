//
//  ocean.h
//  gbCore
//
//  Created by sergey.sergeev on 10/13/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#ifndef ocean_h
#define ocean_h

#include "renderable_game_object.h"

namespace gb
{
    class ocean : public renderable_game_object
    {
    private:
        
    protected:
        
    public:
        
        ocean();
        ~ocean();
        
        void set_wave_generator_interval(f32 wave_generator_interval);
        
        void set_mesh(const mesh_shared_ptr& mesh);
    };
};


#endif 
