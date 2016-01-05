//
//  configuration_types.h
//  gbCore
//
//  Created by sergey.sergeev on 8/12/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef configuration_types_h
#define configuration_types_h

namespace gb
{
    enum e_configuration_type
    {
        e_configuration_type_undefined = 0,
        e_configuration_type_model,
        e_configuration_type_particle_emitter,
        e_configuration_type_ws_technique,
        e_configuration_type_ss_technique,
        e_configuration_type_transition,
        e_configuration_type_max
    };
};


#endif
