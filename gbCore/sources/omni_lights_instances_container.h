//
//  omni_lights_instances_container.h
//  gbCore
//
//  Created by sergey.sergeev on 12/14/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#ifndef omni_lights_instances_container_h
#define omni_lights_instances_container_h

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
    class omni_lights_instances_container
    {
    private:
        
        std::string m_technique_name;
        material_shared_ptr m_material;
        instanced_mesh_shared_ptr m_mesh;
        
        std::shared_ptr<std::vector<glm::vec4>> m_transformations;
        std::shared_ptr<std::vector<glm::vec4>> m_colors;
        
    protected:
        
        std::set<omni_light_shared_ptr> m_lights;
        
    public:
        
        omni_lights_instances_container();
        ~omni_lights_instances_container();
        
        static const ui32 k_max_lights_in_container;
        
        void add_omni_light(const omni_light_shared_ptr& omni_light);
        void remove_omni_light(const omni_light_shared_ptr& omni_light);
        
        ui32 get_omni_lights_count() const;
        
        void set_material(const material_shared_ptr& material, const std::string& technique_name);
        void set_mesh(const instanced_mesh_shared_ptr& mesh);
    };
};


#endif
