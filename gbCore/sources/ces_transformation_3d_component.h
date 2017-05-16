//
//  ces_transformation_3d_component.h
//  gbCore
//
//  Created by serhii serhiiv on 5/15/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_transformation_component.h"

namespace gb
{
    class ces_transformation_3d_component : public ces_transformation_component
    {
    private:
        
    protected:
        
    public:
        
        ces_transformation_3d_component();
        ~ces_transformation_3d_component();
        
        ces_transformation_3d_component(const ces_transformation_3d_component& copy) = delete;
        ces_transformation_3d_component& operator=(const ces_transformation_3d_component& copy) = delete;
    };
};
