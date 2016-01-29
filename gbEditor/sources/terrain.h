//
//  terrain.h
//  gbEditor
//
//  Created by sergey.sergeev on 1/29/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef terrain_h
#define terrain_h

#include "sprite.h"
#include "ed_declarations.h"

namespace gb
{
    namespace ed
    {
        class terrain : public sprite
        {
        private:
            
        protected:
            
            texture_shared_ptr m_mask_texture;
            std::vector<texture_shared_ptr> m_diffuse_textures;
            std::vector<texture_shared_ptr> m_normalmap_textures;
            
            terrain_texture_generator_shared_ptr m_texture_generator;
            
        public:
            
            terrain();
            ~terrain();
            
            void set_mask_texture(const texture_shared_ptr& texture);
            void set_diffuse_textures(const std::vector<texture_shared_ptr>& textures);
            void set_normalmap_textures(const std::vector<texture_shared_ptr>& textures);
            
            void generate();
        };
    };
};

#endif
