//
//  animation_3d_mask.cpp
//  gbCore
//
//  Created by serhii.s on 1/8/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "animation_3d_mask.h"
#include "resource_status.h"

/*namespace gb
{
    bone_3d_mask_data::bone_3d_mask_data(const std::string& name,
                                         f32 weight) :
    m_name(name),
    m_weight(weight)
    {
        
    }
    
    const std::string bone_3d_mask_data::get_name() const
    {
        return m_name;
    }
    
    f32 bone_3d_mask_data::get_weight() const
    {
        return m_weight;
    }
    
    animation_3d_mask_transfering_data::animation_3d_mask_transfering_data(const std::vector<bone_3d_mask_data_shared_ptr>& bones) :
    m_bones_mask(bones)
    {
        m_type = e_resource_transfering_data_type_animation_mask;
    }
    
    const std::vector<bone_3d_mask_data_shared_ptr>& animation_3d_mask_transfering_data::get_bones_mask() const
    {
        return m_bones_mask;
    }
    
    animation_3d_mask::animation_3d_mask(const std::string& guid) :
    resource(e_resource_type_animation_mask, guid),
    m_animation_mask_data(nullptr)
    {
        
    }
    
    animation_3d_mask_shared_ptr animation_3d_mask::construct(const std::string& guid,
                                                              const animation_3d_mask_transfering_data_shared_ptr& data)
    {
        const auto animation_mask= std::make_shared<animation_3d_mask>(guid);
        animation_mask->on_transfering_data_serialized(data);
        animation_mask->on_transfering_data_commited(data);
        return animation_mask;
    }
    
    void animation_3d_mask::on_transfering_data_serialized(const std::shared_ptr<resource_transfering_data>& data)
    {
        assert(data != nullptr);
        switch(data->get_type())
        {
            case e_resource_transfering_data_type_animation_mask:
            {
                m_animation_mask_data = std::static_pointer_cast<animation_3d_mask_transfering_data>(data);
                m_status |= e_resource_status_loaded;
            }
                break;
                
            default:
            {
                assert(false);
            }
                break;
        }
    }
    
    void animation_3d_mask::on_transfering_data_commited(const std::shared_ptr<resource_transfering_data>& data)
    {
        assert(data != nullptr);
        switch(data->get_type())
        {
            case e_resource_transfering_data_type_animation_mask:
            {
                m_status |= e_resource_status_commited;
            }
                break;
                
            default:
            {
                assert(false);
            }
                break;
        }
    }
    
    const std::vector<bone_3d_mask_data_shared_ptr>& animation_3d_mask::get_bones_mask() const
    {
        return resource::is_loaded() ? m_animation_mask_data->get_bones_mask() : std::move(std::vector<bone_3d_mask_data_shared_ptr>());
    }
}*/
