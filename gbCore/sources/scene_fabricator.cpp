//
//  scene_fabricator.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/26/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "scene_fabricator.h"
#include "resource_accessor.h"
#include "mesh.h"
#include "camera.h"
#include "instanced_mesh.h"
#include "material.h"
#include "sequence.h"
#include "renderable_game_object.h"
#include "animation_mixer.h"
#include "direction_light.h"
#include "omni_light.h"
#include "omni_lights_instances_container.h"
#include "model3d_static.h"
#include "instanced_models3d_static.h"
#include "model3d_animated.h"
#include "particle_emitter.h"
#include "skybox.h"
#include "ocean.h"
#include "mesh_constructor.h"
#include "texture_configuration.h"
#include "materials_constructor.h"

namespace gb
{
    static const std::string k_deffered_rendering_normal_texture = "ws.forward.rendering.normal.color";
    static const std::string k_deffered_rendering_depth_texture = "ws.base.depth";
    static const std::string k_deffered_rendering_material_name = "ws.deferred.lighting";
    
    scene_fabricator::scene_fabricator()
    {
        
    }
    
    scene_fabricator::~scene_fabricator()
    {
        
    }
    
    camera_shared_ptr scene_fabricator::create_camera(f32 fov, f32 near, f32 far,const glm::ivec4& viewport)
    {
        camera_shared_ptr camera = std::make_shared<gb::camera>(fov, near, far, viewport);
        m_cameras_container.insert(camera);
        return camera;
    }
    
    void scene_fabricator::destroy_camera(const camera_shared_ptr& camera)
    {
        m_cameras_container.erase(camera);
    }
    
    void scene_fabricator::add_materials(const renderable_game_object_shared_ptr& renderable_game_object,
                                         const std::vector<std::shared_ptr<configuration>>& configurations)
    {
        for(const auto& iterator : configurations)
        {
            std::shared_ptr<material_configuration> material_configuration =
            std::static_pointer_cast<gb::material_configuration>(iterator);
            
            material_shared_ptr material = material::construct(material_configuration);
            gb::material::set_shader(material, material_configuration, m_resource_accessor);
            gb::material::set_textures(material, material_configuration, m_resource_accessor);
            renderable_game_object->add_material(material_configuration->get_technique_name(),
                                                 material_configuration->get_technique_pass(), material);
        }
    }
    
    direction_light_shared_ptr scene_fabricator::create_direction_light()
    {
        mesh_shared_ptr mesh = mesh_constructor::create_screen_quad();;
        material_shared_ptr material = materials_constructor::create_direction_light_material();
        
        texture_shared_ptr texture_01 = m_resource_accessor->get_texture(k_deffered_rendering_normal_texture);
        assert(texture_01);
        texture_shared_ptr texture_02 = m_resource_accessor->get_texture(k_deffered_rendering_depth_texture);
        assert(texture_02);
        
        material->set_texture(texture_01, e_shader_sampler_01);
        material->set_texture(texture_02, e_shader_sampler_02);
        
        direction_light_shared_ptr direction_light = std::make_shared<gb::direction_light>();
        
        direction_light->add_material(k_deffered_rendering_material_name, 0, material);
        direction_light->set_mesh(mesh);
        m_direction_lights_container.insert(direction_light);
        return direction_light;
    }
    
    void scene_fabricator::destroy_direction_light(const direction_light_shared_ptr &direction_light)
    {
        m_direction_lights_container.erase(direction_light);
    }
    
    omni_lights_instances_container_shared_ptr scene_fabricator::add_omni_lights_instances_container()
    {
        omni_lights_instances_container_shared_ptr omni_lights_instances_container = std::make_shared<gb::omni_lights_instances_container>();
        material_shared_ptr material = materials_constructor::create_omni_light_material();
        
        texture_shared_ptr texture_01 = m_resource_accessor->get_texture(k_deffered_rendering_normal_texture);
        assert(texture_01);
        texture_shared_ptr texture_02 = m_resource_accessor->get_texture(k_deffered_rendering_depth_texture);
        assert(texture_02);
        
        material->set_texture(texture_01, e_shader_sampler_01);
        material->set_texture(texture_02, e_shader_sampler_02);
        
        instanced_mesh_shared_ptr mesh = mesh_constructor::create_instanced_sphere(1.f, 16, 16);
        
        omni_lights_instances_container->set_material(material, k_deffered_rendering_material_name);
        omni_lights_instances_container->set_mesh(mesh);
        
        return omni_lights_instances_container;
    }
    
    omni_light_shared_ptr scene_fabricator::create_omni_light(f32 radius, const glm::vec4& color)
    {
        omni_lights_instances_container_shared_ptr omni_lights_instances_container = nullptr;
        if(m_omni_lights_containers.size() == 0)
        {
            omni_lights_instances_container = scene_fabricator::add_omni_lights_instances_container();
            m_omni_lights_containers.push_back(omni_lights_instances_container);
        }
        else
        {
            omni_lights_instances_container = *(m_omni_lights_containers.end() - 1);
            if(omni_lights_instances_container->get_omni_lights_count() < omni_lights_instances_container::k_max_lights_in_container)
            {
                omni_lights_instances_container = scene_fabricator::add_omni_lights_instances_container();
                m_omni_lights_containers.push_back(omni_lights_instances_container);
            }
        }
        
        assert(omni_lights_instances_container);
        omni_light_shared_ptr omni_light = std::make_shared<gb::omni_light>();
        omni_lights_instances_container->add_omni_light(omni_light);
        
        omni_light->set_radius(radius);
        omni_light->set_color(color);
        
        return omni_light;
    }
    
    void scene_fabricator::destroy_omni_light(const omni_light_shared_ptr& omni_light)
    {
        
    }
    
    model3d_static_shared_ptr scene_fabricator::create_model3d_static(const std::string& filename)
    {
        std::shared_ptr<model_configuration> model_configuration =
        std::static_pointer_cast<gb::model_configuration>(m_configuration_accessor->get_model_configuration(filename));
        assert(model_configuration);
        model3d_static_shared_ptr model3d_static = nullptr;
        if(model_configuration)
        {
            model3d_static = std::make_shared<gb::model3d_static>();
            
            mesh_shared_ptr mesh = nullptr;
            if(model_configuration->get_mesh_filename().length() != 0)
            {
                mesh = m_resource_accessor->get_mesh(model_configuration->get_mesh_filename());
            }
            else if(model_configuration->get_mesh_base_class().length() != 0)
            {
                if(model_configuration->get_mesh_base_class() == "plane")
                {
                    mesh = mesh_constructor::create_plane(glm::vec2(1.f));
                }
                else if(model_configuration->get_mesh_base_class() == "box")
                {
                    mesh = mesh_constructor::create_box(glm::vec3(-.5f), glm::vec3(.5f));
                }
                else
                {
                    assert(false);
                }
            }
            else
            {
                assert(false);
            }
        
            assert(mesh);
            model3d_static->set_mesh(mesh);
            scene_fabricator::add_materials(model3d_static, model_configuration->get_materials_configurations());
            m_game_objects_container.insert(model3d_static);
        }
        return model3d_static;
    }
    
    model3d_animated_shared_ptr scene_fabricator::create_model3d_animated(const std::string &filename)
    {
        std::shared_ptr<model_configuration> model_configuration =
        std::static_pointer_cast<gb::model_configuration>(m_configuration_accessor->get_model_configuration(filename));
        assert(model_configuration);
        model3d_animated_shared_ptr model3d_animated = nullptr;
        if(model_configuration)
        {
            model3d_animated = std::make_shared<gb::model3d_animated>();
            
            mesh_shared_ptr mesh = m_resource_accessor->get_mesh(model_configuration->get_mesh_filename());
            assert(mesh);

            model3d_animated->set_mesh(mesh);
            mesh->add_resource_loading_callback(std::make_shared<resource::f_resource_loading_callback>([model_configuration, model3d_animated, this](const resource_shared_ptr& resource, bool success) {
                
                mesh_shared_ptr mesh = std::static_pointer_cast<gb::mesh>(resource);
                assert(mesh->get_skeleton_data());
                assert(mesh->get_bindpose_data());

                model3d_animated->set_mesh(mesh);
                animation_mixer_shared_ptr animation_mixer = std::make_shared<gb::animation_mixer>(mesh->get_skeleton_data(),
                                                                                                   mesh->get_bindpose_data());
                
                for(const auto& iterator : model_configuration->get_animations_configurations())
                {
                    std::shared_ptr<animation_configuration> animation_configuration = std::static_pointer_cast<gb::animation_configuration>(iterator);
                    sequence_shared_ptr sequence = m_resource_accessor->get_animation(animation_configuration->get_animation_filename());
                    sequence->add_resource_loading_callback(std::make_shared<resource::f_resource_loading_callback>([animation_mixer](const resource_shared_ptr& resource, bool success) {
                        
                        sequence_shared_ptr sequence = std::static_pointer_cast<gb::sequence>(resource);
                        animation_mixer->add_sequence(sequence);
                    }));
                }
                model3d_animated->set_animation_mixer(animation_mixer);
                model3d_animated->create_animation_linkage(model_configuration);
            }));
            
            scene_fabricator::add_materials(model3d_animated, model_configuration->get_materials_configurations());
            m_game_objects_container.insert(model3d_animated);
        }
        return model3d_animated;
    }
    
    particle_emitter_shared_ptr scene_fabricator::create_particle_emitter(const std::string& filename)
    {
        std::shared_ptr<particle_emitter_configuration> particle_emitter_configuration =
        std::static_pointer_cast<gb::particle_emitter_configuration>(m_configuration_accessor->get_particle_emitter_configuration(filename));
        assert(particle_emitter_configuration);
        particle_emitter_shared_ptr particle_emitter = nullptr;
        if(particle_emitter_configuration)
        {
            particle_emitter = std::make_shared<gb::particle_emitter>();
            particle_emitter->set_settings(particle_emitter_configuration);
            
            scene_fabricator::add_materials(particle_emitter, particle_emitter_configuration->get_materials_configurations());
            m_game_objects_container.insert(particle_emitter);
        }
        return particle_emitter;
    }
    
    skybox_shared_ptr scene_fabricator::create_skybox(const std::string& filename)
    {
        std::shared_ptr<skybox_configuration> skybox_configuration =
        std::static_pointer_cast<gb::skybox_configuration>(m_configuration_accessor->get_skybox_configuration(filename));
        assert(skybox_configuration);
        skybox_shared_ptr skybox = nullptr;
        if(skybox_configuration)
        {
            skybox = std::make_shared<gb::skybox>();
            
            mesh_shared_ptr mesh = mesh_constructor::create_box(glm::vec3(-1.f), glm::vec3(1.f));
            assert(mesh);
            skybox->set_mesh(mesh);
            
            scene_fabricator::add_materials(skybox, skybox_configuration->get_materials_configurations());
            m_game_objects_container.insert(skybox);
        }
        return skybox;
    }
    
    ocean_shared_ptr scene_fabricator::create_ocean(const std::string& filename)
    {
        std::shared_ptr<ocean_configuration> ocean_configuration =
        std::static_pointer_cast<gb::ocean_configuration>(m_configuration_accessor->get_ocean_configuration(filename));
        assert(ocean_configuration);
        ocean_shared_ptr ocean = nullptr;
        if(ocean_configuration)
        {
            ocean = std::make_shared<gb::ocean>();
            
            mesh_shared_ptr mesh = mesh_constructor::create_ocean_plane(ocean_configuration->get_size(), ocean_configuration->get_altitude());
            assert(mesh);
            ocean->set_mesh(mesh);
            
            ocean->set_wave_generator_interval(ocean_configuration->get_wave_generation_interval());
            
            scene_fabricator::add_materials(ocean, ocean_configuration->get_materials_configurations());
            m_game_objects_container.insert(ocean);
        }
        return ocean;
    }
    
    model3d_static_shared_ptr scene_fabricator::create_grid(i32 rows, i32 columns)
    {
        material_shared_ptr material = materials_constructor::create_wireframe_grid_material();
        mesh_shared_ptr mesh = mesh_constructor::create_grid(rows, columns);

        model3d_static_shared_ptr grid = std::make_shared<gb::model3d_static>();;
        grid->set_mesh(mesh);
        grid->add_material("ws.base", 0, material);
        m_game_objects_container.insert(grid);
        return grid;
    }
    
    model3d_static_shared_ptr scene_fabricator::create_3d_tile_cursor()
    {
        material_shared_ptr material = materials_constructor::create_3d_cursor_material();
        mesh_shared_ptr mesh = mesh_constructor::create_3d_tile_cursor();
        
        model3d_static_shared_ptr tile_cursor = std::make_shared<gb::model3d_static>();;
        tile_cursor->set_mesh(mesh);
        tile_cursor->add_material("ws.base", 0, material);
        m_game_objects_container.insert(tile_cursor);
        return tile_cursor;
    }
    
    void scene_fabricator::destroy_game_object(const game_object_shared_ptr& game_object)
    {
        m_game_objects_container.erase(game_object);
    }
}