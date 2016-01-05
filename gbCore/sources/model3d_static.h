//
//  model3d_static.h
//  gbCore
//
//  Created by sergey.sergeev on 8/26/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef model3d_static_h
#define model3d_static_h

#include "renderable_game_object.h"

namespace gb
{
    class model3d_static : public renderable_game_object
    {
    private:
        
    protected:
        
        void create_physics_component();
        std::pair<bool, bool> m_physics_cached_parameters;
        
    public:
        
        model3d_static();
        ~model3d_static();
        
        void set_mesh(const mesh_shared_ptr& mesh);
        
        void set_touches_receives_enabled(bool value);
        void set_bounding_box_debug_enabled(bool value);
        void set_enable_box2d_physics(bool value, bool is_static);
        void set_is_batched(bool value, const std::string& guid = "");
    };
};

#endif
