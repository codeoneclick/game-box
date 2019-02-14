//
//  scene_2d.hpp
//  gbCore
//
//  Created by serhii.s on 2/13/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "resource.h"

namespace gb
{
    class scene_2d_transfering_data final : public resource_transfering_data
    {
    private:
        
    protected:
        
        std::shared_ptr<Tmx::Map> m_map = nullptr;
        
    public:
        
        scene_2d_transfering_data(const std::shared_ptr<Tmx::Map>& map);
        ~scene_2d_transfering_data();
        
        std::shared_ptr<Tmx::Map> get_map() const;
    };
    
    class scene_2d : public resource
    {
    private:
        
    protected:
        
        scene_2d_transfering_data_shared_ptr m_scene_2d_data;
        
        void on_transfering_data_serialized(const std::shared_ptr<resource_transfering_data>& data);
        void on_transfering_data_commited(const std::shared_ptr<resource_transfering_data>& data);
        
    public:
        
        scene_2d(const std::string& guid);
        
        static scene_2d_shared_ptr construct(const std::string& guid,
                                             const scene_2d_transfering_data_shared_ptr& data);
        ~scene_2d();
        
        std::shared_ptr<Tmx::Map> get_map() const;
    };
};


