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
    
    class input_context;
    typedef std::shared_ptr<input_context> input_context_shared_ptr;
    
    class configuration;
    typedef std::shared_ptr<configuration> configuration_shared_ptr;
    
    class model_configuration;
    typedef std::shared_ptr<model_configuration> model_configuration_shared_ptr;
    
    class resource;
    typedef std::shared_ptr<resource> resource_shared_ptr;
    
    class resource_transfering_data;
    typedef std::shared_ptr<resource_transfering_data> resource_transfering_data_shared_ptr;
    
    class shader;
    typedef std::shared_ptr<shader> shader_shared_ptr;
    
    class shader_transfering_data;
    typedef std::shared_ptr<shader_transfering_data> shader_transfering_data_shared_ptr;
    
    class texture;
    typedef std::shared_ptr<texture> texture_shared_ptr;
    
    class texture_transfering_data;
    typedef std::shared_ptr<texture_transfering_data> texture_transfering_data_shared_ptr;
    
    class cubemap_texture;
    typedef std::shared_ptr<cubemap_texture> cubemap_texture_shared_ptr;
    
    class mesh;
    typedef std::shared_ptr<mesh> mesh_shared_ptr;
    
    class mesh_transfering_data;
    typedef std::shared_ptr<mesh_transfering_data> mesh_transfering_data_shared_ptr;
    
    class skeleton_transfering_data;
    typedef std::shared_ptr<skeleton_transfering_data> skeleton_transfering_data_shared_ptr;
    
    class sequence;
    typedef std::shared_ptr<sequence> sequence_shared_ptr;
    
    class sequence_transfering_data;
    typedef std::shared_ptr<sequence_transfering_data> sequence_transfering_data_shared_ptr;
    
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
    
    class mesh_loading_operation;
    typedef std::shared_ptr<mesh_loading_operation> mesh_loading_operation_shared_ptr;
    
    class animation_loading_operation;
    typedef std::shared_ptr<animation_loading_operation> animation_loading_operation_shared_ptr;
    
    class resource_accessor;
    typedef std::shared_ptr<resource_accessor> resource_accessor_shared_ptr;
    
    class configuration_accessor;
    typedef std::shared_ptr<configuration_accessor> configuration_accessor_shared_ptr;
    
    class material;
    typedef std::shared_ptr<material> material_shared_ptr;
    
    class camera;
    typedef std::shared_ptr<camera> camera_shared_ptr;
    
    class frustum;
    typedef std::shared_ptr<frustum> frustum_shared_ptr;
    
    class shadow_cast_light;
    typedef std::shared_ptr<shadow_cast_light> shadow_cast_light_shared_ptr;
    
    class i_scene_graph;
    typedef std::shared_ptr<i_scene_graph> i_scene_graph_shared_ptr;
    
    class i_scene_fabricator;
    typedef std::shared_ptr<i_scene_fabricator> i_scene_fabricator_shared_ptr;
    
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

    class ces_render_component;
    typedef std::shared_ptr<ces_render_component> ces_render_component_shared_ptr;
    
    class ces_animation_component;
    typedef std::shared_ptr<ces_animation_component> ces_animation_component_shared_ptr;
    
    class ces_touch_component;
    typedef std::shared_ptr<ces_touch_component> ces_touch_component_shared_ptr;
    
    class ces_debug_render_component;
    typedef std::shared_ptr<ces_debug_render_component> ces_debug_render_component_shared_ptr;
    
    class ces_bounding_box_debug_component;
    typedef std::shared_ptr<ces_bounding_box_debug_component> ces_bounding_box_debug_component_shared_ptr;
    
    class ces_tbn_debug_component;
    typedef std::shared_ptr<ces_tbn_debug_component> ces_tbn_debug_component_shared_ptr;
    
    class ces_particle_emitter_component;
    typedef std::shared_ptr<ces_particle_emitter_component> ces_particle_emitter_component_shared_ptr;
    
    class ces_box2d_component;
    typedef std::shared_ptr<ces_box2d_component> ces_box2d_component_shared_ptr;
    
    class ces_camera_component;
    typedef std::shared_ptr<ces_camera_component> ces_camera_component_shared_ptr;
    
    class game_object;
    typedef std::shared_ptr<game_object> game_object_shared_ptr;
    
    class renderable_game_object;
    typedef std::shared_ptr<renderable_game_object> renderable_game_object_shared_ptr;
    
    class model3d_static;
    typedef std::shared_ptr<model3d_static> model3d_static_shared_ptr;
    
    class game_transition;
    typedef std::shared_ptr<game_transition> game_transition_shared_ptr;
    
    class game_scene;
    typedef std::shared_ptr<game_scene> game_scene_shared_ptr;
    
    class scene_graph;
    typedef std::shared_ptr<scene_graph> scene_graph_shared_ptr;
    
    class bone;
    typedef std::shared_ptr<bone> bone_shared_ptr;
    
    class bone_data;
    typedef std::shared_ptr<bone_data> bone_data_shared_ptr;
    
    class skeleton;
    typedef std::shared_ptr<skeleton> skeleton_shared_ptr;
    
    class animation_mixer;
    typedef std::shared_ptr<animation_mixer> animation_mixer_shared_ptr;
    
    class model3d_animated;
    typedef std::shared_ptr<model3d_animated> model3d_animated_shared_ptr;
    
    class input_context_listener;
    typedef std::shared_ptr<input_context_listener> input_context_listener_shared_ptr;
    
    class touch_listener;
    typedef std::shared_ptr<touch_listener> touch_listener_shared_ptr;
    
    class particle_emitter;
    typedef std::shared_ptr<particle_emitter> particle_emitter_shared_ptr;
    
    class omni_light;
    typedef std::shared_ptr<omni_light> omni_light_shared_ptr;
    
    class direction_light;
    typedef std::shared_ptr<direction_light> direction_light_shared_ptr;
    
    class instanced_mesh;
    typedef std::shared_ptr<instanced_mesh> instanced_mesh_shared_ptr;
    
    class instanced_models3d_static;
    typedef std::shared_ptr<instanced_models3d_static> instanced_models3d_static_shared_ptr;
    
    class collision_manager;
    typedef std::shared_ptr<collision_manager> collision_manager_shared_ptr;
    
    class game_command_i;
    typedef std::shared_ptr<game_command_i> game_command_i_shared_ptr;
    
    class game_commands_container;
    typedef std::shared_ptr<game_commands_container> game_commands_container_shared_ptr;
    
    class skybox;
    typedef std::shared_ptr<skybox> skybox_shared_ptr;
    
    class ocean;
    typedef std::shared_ptr<ocean> ocean_shared_ptr;
    
    class ces_skybox_component;
    typedef std::shared_ptr<ces_skybox_component> ces_skybox_component_shared_ptr;
    
    class ces_ocean_component;
    typedef std::shared_ptr<ces_ocean_component> ces_ocean_component_shared_ptr;
    
    class ces_batch_component;
    typedef std::shared_ptr<ces_batch_component> ces_batch_component_shared_ptr;
    
    class ces_batch_system;
    typedef std::shared_ptr<ces_batch_system> ces_batch_system_shared_ptr;
    
    class batch;
    typedef std::shared_ptr<batch> batch_shared_ptr;
    
    class thread_operation;
    typedef std::shared_ptr<thread_operation> thread_operation_shared_ptr;
    
    class omni_lights_instances_container;
    typedef std::shared_ptr<omni_lights_instances_container> omni_lights_instances_container_shared_ptr;
    
    class scene_graph_parameters;
    typedef std::shared_ptr<scene_graph_parameters> scene_graph_parameters_shared_ptr;
};

#endif
