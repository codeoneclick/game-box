//
//  ed_declarations.h
//  gbEditor
//
//  Created by sergey.sergeev on 1/15/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef ed_declarations_h
#define ed_declarations_h

namespace gb
{
    namespace ed
    {
        class ed_fabricator;
        typedef std::shared_ptr<ed_fabricator> ed_fabricator_shared_ptr;
        
        class grid;
        typedef std::shared_ptr<grid> grid_shared_ptr;
        
        class stroke;
        typedef std::shared_ptr<stroke> stroke_shared_ptr;
        
        class landscape;
        typedef std::shared_ptr<landscape> landscape_shared_ptr;
        
        class brush;
        typedef std::shared_ptr<brush> brush_shared_ptr;
        
        class canvas;
        typedef std::shared_ptr<canvas> canvas_shared_ptr;
        
        class canvas_chunk;
        typedef std::shared_ptr<canvas_chunk> canvas_chunk_shared_ptr;
        
        class terrain_texture_generator;
        typedef std::shared_ptr<terrain_texture_generator> terrain_texture_generator_shared_ptr;
        
        class drag_controller;
        typedef std::shared_ptr<drag_controller> drag_controller_shared_ptr;
        
        class drag_camera_controller;
        typedef std::shared_ptr<drag_camera_controller> drag_camera_controller_shared_ptr;
        
        class drag_game_objects_controller;
        typedef std::shared_ptr<drag_game_objects_controller> drag_game_objects_controller_shared_ptr;
        
        class drag_brush_controller;
        typedef std::shared_ptr<drag_brush_controller> drag_brush_controller_shared_ptr;
    };
};
#endif
