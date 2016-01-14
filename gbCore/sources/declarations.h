//
//  declarations.h
//  gbCore
//
//  Created by sergey.sergeev on 8/24/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef declarations_h
#define declarations_h

namespace gb
{
    class ogl_window;
    typedef std::shared_ptr<ogl_window> ogl_window_shared_ptr;
    
    class graphics_context;
    typedef std::shared_ptr<graphics_context> graphics_context_shared_ptr;
    
    class render_pipeline;
    typedef std::shared_ptr<render_pipeline> render_pipeline_shared_ptr;
    
    class input_context;
    typedef std::shared_ptr<input_context> input_context_shared_ptr;
    
    class configuration;
    typedef std::shared_ptr<configuration> configuration_shared_ptr;
    
    class sprite_configuration;
    typedef std::shared_ptr<sprite_configuration> sprite_configuration_shared_ptr;
    
    class resource;
    typedef std::shared_ptr<resource> resource_shared_ptr;
    
    struct resource_transfering_data;
    typedef std::shared_ptr<resource_transfering_data> resource_transfering_data_shared_ptr;
    
    class shader;
    typedef std::shared_ptr<shader> shader_shared_ptr;
    
    struct shader_transfering_data;
    typedef std::shared_ptr<shader_transfering_data> shader_transfering_data_shared_ptr;
    
    class texture;
    typedef std::shared_ptr<texture> texture_shared_ptr;
    
    struct texture_transfering_data;
    typedef std::shared_ptr<texture_transfering_data> texture_transfering_data_shared_ptr;
    
    class mesh;
    typedef std::shared_ptr<mesh> mesh_shared_ptr;
    
    class vbo;
    typedef std::shared_ptr<vbo> vbo_shared_ptr;
    
    class ibo;
    typedef std::shared_ptr<ibo> ibo_shared_ptr;
    
    class resource_serializer;
    typedef std::shared_ptr<resource_serializer> resource_serializer_shared_ptr;
    
    class resource_commiter;
    typedef std::shared_ptr<resource_commiter> resource_commiter_shared_ptr;
    
    class resource_loading_operation;
    typedef std::shared_ptr<resource_loading_operation> resource_loading_operation_shared_ptr;
    
    class resource_loading_interface;
    typedef std::shared_ptr<resource_loading_interface> resource_loading_interface_shared_ptr;
    
    class shader_loading_operation;
    typedef std::shared_ptr<shader_loading_operation> shader_loading_operation_shared_ptr;
    
    class texture_loading_operation;
    typedef std::shared_ptr<texture_loading_operation> texture_loading_operation_shared_ptr;
    
    class resource_accessor;
    typedef std::shared_ptr<resource_accessor> resource_accessor_shared_ptr;
    
    class configuration_accessor;
    typedef std::shared_ptr<configuration_accessor> configuration_accessor_shared_ptr;
    
    class material;
    typedef std::shared_ptr<material> material_shared_ptr;
    
    class scene_graph;
    typedef std::shared_ptr<scene_graph> scene_graph_shared_ptr;
    
    class scene_fabricator;
    typedef std::shared_ptr<scene_fabricator> scene_fabricator_shared_ptr;
    
    class ces_entity;
    typedef std::shared_ptr<ces_entity> ces_entity_shared_ptr;
    
    class ces_systems_feeder;
    typedef std::shared_ptr<ces_systems_feeder> ces_systems_feeder_shared_ptr;
    
    class ces_system;
    typedef std::shared_ptr<ces_system> ces_system_shared_ptr;
    
    class ces_base_component;
    typedef std::shared_ptr<ces_base_component> ces_base_component_shared_ptr;
    
    class ces_transformation_component;
    typedef std::shared_ptr<ces_transformation_component> ces_transformation_component_shared_ptr;
    
    class ces_geometry_component;
    typedef std::shared_ptr<ces_geometry_component> ces_geometry_component_shared_ptr;
    
    class ces_geometry_quad_component;
    typedef std::shared_ptr<ces_geometry_quad_component> ces_geometry_quad_component_shared_ptr;
    
    class ces_geometry_freeform_component;
    typedef std::shared_ptr<ces_geometry_freeform_component> ces_geometry_freeform_component_shared_ptr;

    class ces_material_component;
    typedef std::shared_ptr<ces_material_component> ces_material_component_shared_ptr;
    
    class ces_text_component;
    typedef std::shared_ptr<ces_text_component> ces_text_component_shared_ptr;
    
    class ces_scene_component;
    typedef std::shared_ptr<ces_scene_component> ces_scene_component_shared_ptr;
    
    class ces_light_compoment;
    typedef std::shared_ptr<ces_light_compoment> ces_light_compoment_shared_ptr;
    
    class ces_shadow_component;
    typedef std::shared_ptr<ces_shadow_component> ces_shadow_component_shared_ptr;
    
    class ces_convex_hull_component;
    typedef std::shared_ptr<ces_convex_hull_component> ces_convex_hull_component_shared_ptr;
    
    class ces_bound_touch_component;
    typedef std::shared_ptr<ces_bound_touch_component> ces_bound_touch_component_shared_ptr;
    
    class game_object;
    typedef std::shared_ptr<game_object> game_object_shared_ptr;
    
    class renderable_game_object;
    typedef std::shared_ptr<renderable_game_object> renderable_game_object_shared_ptr;
    
    class game_transition;
    typedef std::shared_ptr<game_transition> game_transition_shared_ptr;
    
    class input_context_listener;
    typedef std::shared_ptr<input_context_listener> input_context_listener_shared_ptr;
    
    class touch_listener;
    typedef std::shared_ptr<touch_listener> touch_listener_shared_ptr;
    
    class collision_manager;
    typedef std::shared_ptr<collision_manager> collision_manager_shared_ptr;
    
    class game_command_i;
    typedef std::shared_ptr<game_command_i> game_command_i_shared_ptr;
    
    class game_commands_container;
    typedef std::shared_ptr<game_commands_container> game_commands_container_shared_ptr;
    
    class thread_operation;
    typedef std::shared_ptr<thread_operation> thread_operation_shared_ptr;
    
    class render_technique_ws;
    typedef std::shared_ptr<render_technique_ws> render_technique_ws_shared_ptr;
    
    class sprite;
    typedef std::shared_ptr<sprite> sprite_shared_ptr;
    
    class label;
    typedef std::shared_ptr<label> label_shared_ptr;
    
    class light;
    typedef std::shared_ptr<light> light_shared_ptr;
    
    class camera;
    typedef std::shared_ptr<camera> camera_shared_ptr;
};

#endif
