//
//  cubemap_texture.h
//  gbCore
//
//  Created by sergey.sergeev on 10/13/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#ifndef cubemap_texture_h
#define cubemap_texture_h

#include "texture.h"

namespace gb
{
    class cubemap_texture : public texture, public resource_loading_interface
    {
    private:
        
        texture_shared_ptr m_xpositive;
        texture_shared_ptr m_xnegative;
        texture_shared_ptr m_ypositive;
        texture_shared_ptr m_ynegative;
        texture_shared_ptr m_zpositive;
        texture_shared_ptr m_znegative;
        
    protected:
        
        void on_resource_loaded(const resource_shared_ptr& resource, bool success);
        
    public:
        
        cubemap_texture(const std::string& guid,
                        const texture_shared_ptr& xpositive, const texture_shared_ptr& xnegative,
                        const texture_shared_ptr& ypositive, const texture_shared_ptr& ynegative,
                        const texture_shared_ptr& zpositive, const texture_shared_ptr& znegative);
        
        static cubemap_texture_shared_ptr construct(const std::string& guid,
                                                    const texture_shared_ptr& xpositive, const texture_shared_ptr& xnegative,
                                                    const texture_shared_ptr& ypositive, const texture_shared_ptr& ynegative,
                                                    const texture_shared_ptr& zpositive, const texture_shared_ptr& znegative);
        ~cubemap_texture();
        
        void bind() const;
        void unbind() const;
    };

};

#endif
