//
//  ces_animation_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/28/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "ces_animation_component.h"
#include "animation_mixer.h"
#include "model_configuration.h"
#include "animation_configuration.h"

namespace gb
{
    ces_animation_component::ces_animation_component() :
    m_animation_mixer(nullptr)
    {
        m_type = e_ces_component_type_animation;
    }
    
    ces_animation_component::~ces_animation_component()
    {
        
    }
    
    void ces_animation_component::set_animation_mixer(const animation_mixer_shared_ptr& animation_mixer)
    {
        m_animation_mixer = animation_mixer;
    }
    
    animation_mixer_shared_ptr ces_animation_component::get_animation_mixer() const
    {
        return m_animation_mixer;
    }
    
    void ces_animation_component::update(f32 deltatime)
    {
        if(m_animation_mixer)
        {
            m_animation_mixer->update(deltatime);
        }
    }
    
    void ces_animation_component::set_animation(const std::string &name)
    {
        if(m_animation_mixer)
        {
            const auto& animation_name = m_animation_names_linkage.find(name);
            if(animation_name != m_animation_names_linkage.end())
            {
                m_animation_mixer->set_animation(animation_name->second);
            }
        }
    }
    
    void ces_animation_component::create_animation_linkage(const model_configuration_shared_ptr& configuration)
    {
        for(ui32 i = 0; i < configuration->get_animations_configurations().size(); ++i)
        {
            std::shared_ptr<animation_configuration> animation_configuration =
            std::static_pointer_cast<gb::animation_configuration>(configuration->get_animations_configurations().at(i));
            
            m_animation_names_linkage.insert(std::make_pair(animation_configuration->get_animation_name(),
                                                            animation_configuration->get_animation_filename()));
        }
    }
}