//
//  ces_animation_component.h
//  gbCore
//
//  Created by sergey.sergeev on 8/28/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef ces_animation_component_h
#define ces_animation_component_h

#include "ces_base_component.h"

namespace gb
{
    class ces_animation_component : public ces_base_component
    {
    private:
        
    protected:
        
        animation_mixer_shared_ptr m_animation_mixer;
        std::map<std::string, std::string> m_animation_names_linkage;
        
    public:
        
        ces_animation_component();
        ~ces_animation_component();
        
        void set_animation_mixer(const animation_mixer_shared_ptr& animation_mixer);
        animation_mixer_shared_ptr get_animation_mixer() const;
        
        void update(f32 deltatime);
        void set_animation(const std::string& name);
        void create_animation_linkage(const model_configuration_shared_ptr& configuration);
    };
};

#endif
