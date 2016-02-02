//
//  landscape.hpp
//  gbEditor
//
//  Created by Serhii Serhiiv on 2/1/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef landscape_h
#define landscape_h

#include "sprite.h"
#include "ed_declarations.h"

namespace gb
{
    namespace ed
    {
        class landscape : public sprite
        {
        public:
            
            static const i8 k_brushes_count_on_main_layer = 3;
            static const i8 k_brushes_count_on_additional_layer = 2;
            
        private:
            
            material_shared_ptr get_main_splatting_material(const texture_shared_ptr& layer_1_brush,
                                                            const texture_shared_ptr& layer_2_brush,
                                                            const texture_shared_ptr& layer_3_brush,
                                                            const texture_shared_ptr& mask);
            
            material_shared_ptr get_additional_splatting_material(const texture_shared_ptr& layer_1_brush,
                                                                  const texture_shared_ptr& layer_2_brush,
                                                                  const texture_shared_ptr& layer_3_brush,
                                                                  const texture_shared_ptr& mask);
            
            texture_shared_ptr create_splatting_mask(const glm::vec2& size);
            
        protected:
            
            std::vector<texture_shared_ptr> m_masks;
            
            std::array<std::pair<texture_shared_ptr, texture_shared_ptr>, k_brushes_count_on_main_layer> m_main_layer_brushes;
            std::vector<std::array<std::pair<texture_shared_ptr, texture_shared_ptr>, k_brushes_count_on_additional_layer>> m_additional_layer_brushes;

        public:
            
            landscape();
            ~landscape();
            
            void add_mask(const texture_shared_ptr& mask);
            void add_brush(const texture_shared_ptr& diffuse, const texture_shared_ptr& normalmap, i32 layer, i32 sampler);
            
            texture_shared_ptr get_brush(i32 layer, i32 sampler);
            
            void generate();
        };
    };
};

#endif
