//
//  skybox.h
//  gbCore
//
//  Created by sergey.sergeev on 10/13/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#ifndef skybox_h
#define skybox_h

#include "renderable_game_object.h"

namespace gb
{
    class skybox : public renderable_game_object
    {
    private:
        
    protected:
        
    public:
        
        skybox();
        ~skybox();
        
        void set_mesh(const mesh_shared_ptr& mesh);
    };
};

#endif