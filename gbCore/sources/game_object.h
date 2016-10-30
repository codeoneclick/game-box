//
//  game_object.h
//  gbCore
//
//  Created by sergey.sergeev on 8/26/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef game_object_h
#define game_object_h

#include "ces_entity.h"

namespace gb
{
    class game_object : public ces_entity
    {
    private:
        
    protected:
        
        void update_z_order_recursively(const ces_entity_shared_ptr& entity, f32 z_order);
        void update_absolute_transformation_recursively(const ces_entity_shared_ptr& entity);
        
    public:
        
        game_object();
        virtual ~game_object();
        
        void add_child(const ces_entity_shared_ptr& child) override;
        
        std::property_rw<glm::vec2> position;
        std::property_rw<f32> rotation;
        std::property_rw<glm::vec2> scale;
        std::property_rw<f32> z_order;
        
        std::property_rw<glm::vec2> size;
        std::property_ro<glm::vec4> bound;
    };
};

#endif
