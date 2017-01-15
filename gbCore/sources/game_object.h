//
//  game_object.h
//  gbCore
//
//  Created by sergey.sergeev on 8/26/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_entity.h"

namespace gb
{
    class game_object : public ces_entity
    {
    private:
        
    protected:
        
        void update_z_order_recursively(const ces_entity_shared_ptr& entity, f32& z_order);
        
    public:
        
        game_object();
        virtual ~game_object();
        
        void add_child(const ces_entity_shared_ptr& child) override;
        void rearrange_children_according_to_z_order() override;
        
        std::property_rw<glm::vec2> position;
        std::property_rw<f32> rotation;
        std::property_rw<glm::vec2> scale;
        
        std::property_ro<f32> z_order;
        
        std::property_rw<glm::vec2> size;
    };
};
