//
//  element.h
//  gbUI
//
//  Created by sergey.sergeev on 9/28/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef element_h
#define element_h

#include "main_headers.h"
#include "ces_entity.h"

namespace gb
{
    namespace ui
    {
        class element : public ces_entity
        {
        private:
            
        protected:
            
            glm::ivec2 m_position;
            glm::ivec2 m_size;
            
        public:
            
            element();
            ~element();
            
            void set_position(const glm::ivec2& position);
            glm::ivec2 get_position() const;
            
            void set_size(const glm::ivec2& size);
            glm::ivec2 get_size() const;

            void add_material(const std::string& technique_name, const material_shared_ptr& material);
            void remove_material(const std::string& technique_name);
            material_shared_ptr get_material(const std::string& technique_name) const;
            
            void set_mesh(const mesh_shared_ptr& mesh);
        };
    };
};

#endif
