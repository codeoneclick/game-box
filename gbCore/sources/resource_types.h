//
//  resource_types.h
//  gbCore
//
//  Created by sergey.sergeev on 8/12/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

namespace gb
{
    enum e_resource_type
    {
        e_resource_type_undefined = 0,
        e_resource_type_texture,
        e_resource_type_shader,
        e_resource_type_animation_sequence,
        e_resource_type_animation_mask,
        e_resource_type_mesh_2d,
        e_resource_type_mesh_3d,
        e_resource_type_scene_3d,
        e_resource_type_scene_2d,
        e_resource_type_max
    };
    
    enum e_resource_transfering_data_type
    {
        e_resource_transfering_data_type_undefined = 0,
        e_resource_transfering_data_type_texture,
        e_resource_transfering_data_type_shader,
        e_resource_transfering_data_type_sequence,
        e_resource_transfering_data_type_animation_mask,
        e_resource_transfering_data_type_mesh_3d,
        e_resource_transfering_data_type_skeleton_3d,
        e_resource_transfering_data_type_vbo,
        e_resource_transfering_data_type_ibo,
        e_resource_transfering_data_type_scene_3d,
        e_resource_transfering_data_type_scene_2d
    };
};
