//
//  animation.h
//  gbAnimation
//
//  Created by serhii serhiiv on 11/3/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef animation_h
#define animation_h

#include "resource.h"

namespace gb
{
    namespace anim
    {
        class ani_asset_metadata;
        struct animation_transfering_data : public resource_transfering_data
        {
            std::shared_ptr<ani_asset_metadata> m_metadata;
            
            animation_transfering_data();
            ~animation_transfering_data();
        };
        
        class animation : public resource
        {
        private:
            
            
        protected:
            
            std::shared_ptr<animation_transfering_data> m_data;
            
            void on_transfering_data_serialized(const std::shared_ptr<resource_transfering_data>& data);
            void on_transfering_data_commited(const std::shared_ptr<resource_transfering_data>& data);
            
        public:
            
            animation(const std::string& guid);
            ~animation();
        };
    };
};

#endif
