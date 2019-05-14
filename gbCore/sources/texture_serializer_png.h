//
//  texture_serializer_png.h
//  gbCore
//
//  Created by sergey.sergeev on 8/24/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "resource_serializer.h"

namespace gb
{
    class texture_serializer_png final : public resource_serializer
    {
    private:
        
    protected:
        
        std::string m_filename;
        
    public:
        
        texture_serializer_png(const std::string& filename,
                               const resource_shared_ptr& resource);
        ~texture_serializer_png();
        
        void serialize(const resource_transfering_data_shared_ptr& transfering_data);
        
        static std::tuple<png_byte*, ui32, ui32, ui32, ui32> read_png(const std::shared_ptr<std::istream>& filestream);
    };
};
