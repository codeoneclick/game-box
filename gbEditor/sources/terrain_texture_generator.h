//
//  terrain_texture_generator.h
//  gbEditor
//
//  Created by sergey.sergeev on 1/29/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef terrain_texture_generator_h
#define terrain_texture_generator_h

#include "main_headers.h"
#include "declarations.h"
#include "ed_declarations.h"

namespace gb
{
    namespace ed
    {
        class terrain_texture_generator
        {
        public:
            
        private:
            
        protected:
            
            material_shared_ptr get_splatting_material(const std::vector<texture_shared_ptr>& textures,
                                                       const texture_shared_ptr& mask);
            
        public:
            
            terrain_texture_generator();
            ~terrain_texture_generator();
            
            texture_shared_ptr create_splatting_mask_texture(i32 width, i32 height);
            texture_shared_ptr create_splatting_diffuse_texture(i32 width, i32 height,
                                                                const texture_shared_ptr& mask_texture,
                                                                const std::vector<texture_shared_ptr>& diffuse_textures);
            
        };
    };
};

#endif
