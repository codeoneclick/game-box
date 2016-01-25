//
//  renderable_game_object.h
//  gbCore
//
//  Created by sergey.sergeev on 12/10/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#ifndef renderable_game_object_h
#define renderable_game_object_h

#include "game_object.h"

namespace gb
{
    class renderable_game_object : public game_object
    {
    private:
        
    protected:
        
    public:
        
        renderable_game_object();
        virtual ~renderable_game_object();
        
        void add_material(const std::string& technique_name, i32 technique_pass, const material_shared_ptr& material);
        void remove_material(const std::string& technique_name, i32 technique_pass);
        material_shared_ptr get_material(const std::string& technique_name, i32 technique_pass) const;
    };
};

#endif
