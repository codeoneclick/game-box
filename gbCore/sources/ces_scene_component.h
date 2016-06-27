//
//  ces_scene_component.h
//  gbCore
//
//  Created by Sergey Sergeev on 1/10/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef ces_scene_component_h
#define ces_scene_component_h

#include "ces_base_component.h"

namespace gb
{
    class ces_scene_component : public ces_base_component
    {
    private:
        
        std::weak_ptr<scene_graph> m_scene;
        
    protected:
        
    public:
        
        CTTI_GUID_H(ces_scene_component)
        ces_scene_component();
        ~ces_scene_component();
        
        void set_scene(const scene_graph_shared_ptr& scene);
        
        camera_shared_ptr get_camera() const;
    };
    
#define unsafe_get_scene_component(entity) static_cast<ces_scene_component*>(entity->get_component(ces_scene_component::class_guid()).get())
#define unsafe_get_scene_component_from_this static_cast<ces_scene_component*>(ces_entity::get_component(ces_scene_component::class_guid()).get())
    
};

#endif
