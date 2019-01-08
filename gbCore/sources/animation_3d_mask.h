//
//  animation_3d_mask.hpp
//  gbCore
//
//  Created by serhii.s on 1/8/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

/*#include "resource.h"

namespace gb
{
    class bone_3d_mask_data final
    {
    private:
        
    protected:
        
        std::string m_name;
        f32 m_weight;
        
    public:
        
        bone_3d_mask_data(const std::string& name,
                          f32 weight);
        
        ~bone_3d_mask_data();
        
        const std::string get_name() const;
        f32 get_weight() const;
    };
    
    class animation_3d_mask_transfering_data final : public resource_transfering_data
    {
    private:
        
    protected:
        
        std::vector<bone_3d_mask_data_shared_ptr> m_bones_mask;
        
    public:
        
        animation_3d_mask_transfering_data(const std::vector<bone_3d_mask_data_shared_ptr>& bones);
        ~animation_3d_mask_transfering_data() = default;
        
        const std::vector<bone_3d_mask_data_shared_ptr>& get_bones_mask() const;
    };
    
    class animation_3d_mask : public resource
    {
    private:
        
    protected:
        
        animation_3d_mask_transfering_data_shared_ptr m_animation_mask_data;
        
        void on_transfering_data_serialized(const std::shared_ptr<resource_transfering_data>& data);
        void on_transfering_data_commited(const std::shared_ptr<resource_transfering_data>& data);
        
    public:
        
        animation_3d_mask(const std::string& guid);
        
        static animation_3d_mask_shared_ptr construct(const std::string& guid,
                                                      const animation_3d_mask_transfering_data_shared_ptr& data);
        
        ~animation_3d_mask() = default;
        
         const std::vector<bone_3d_mask_data_shared_ptr>& get_bones_mask() const;
    };
};*/

