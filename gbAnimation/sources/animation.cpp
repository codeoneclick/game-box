//
//  animation.cpp
//  gbAnimation
//
//  Created by serhii serhiiv on 11/3/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "animation.h"
#include "resource_status.h"

namespace gb
{
    namespace anim
    {
        animation_transfering_data::animation_transfering_data() :
        m_metadata(nullptr)
        {
            
        }
        
        animation_transfering_data::~animation_transfering_data()
        {
            
        }
        
        animation::animation(const std::string& guid) :
        gb::resource(e_resource_type_undefined, guid),
        m_data(nullptr)
        {
            
        }
        
        animation::~animation()
        {
            
        }
        
        void animation::on_transfering_data_serialized(const std::shared_ptr<resource_transfering_data> &data)
        {
            assert(data != nullptr);
            m_data = std::static_pointer_cast<animation_transfering_data>(data);
            m_status |= e_resource_status_loaded;
        }
        
        void animation::on_transfering_data_commited(const std::shared_ptr<resource_transfering_data> &data)
        {
            assert(data != nullptr);
            m_status |= e_resource_status_commited;
        }
        
        std::shared_ptr<ani_asset_metadata> animation::get_metadata() const
        {
            return m_data->m_metadata;
        }
    }
}
