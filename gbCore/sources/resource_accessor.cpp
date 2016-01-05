//
//  resource_accessor.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/24/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "resource_accessor.h"
#include "resource_loading_operation.h"
#include "shader_loading_operation.h"
#include "texture_loading_operation.h"
#include "mesh_loading_operation.h"
#include "animation_loading_operation.h"
#include "resource.h"
#include "shader.h"
#include "texture.h"
#include "cubemap_texture.h"
#include "mesh.h"
#include "sequence.h"

namespace gb
{
    resource_accessor::resource_accessor()
    {
        m_thread_executed = true;
        m_thread = std::thread(&resource_accessor::on_thread_update, this);
    }
    
    resource_accessor::~resource_accessor()
    {
        m_thread_executed = false;
        m_thread.join();
        m_operationsQueue.clear();
        m_resources.clear();
    }
    
    void resource_accessor::on_thread_update()
    {
        while (m_thread_executed)
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            for(const auto& iterator : m_operationsQueue)
            {
                resource_loading_operation_shared_ptr operation = iterator.second;
                if(operation->get_status() == e_resource_loading_operation_status_undefined)
                {
                    operation->serialize();
                }
            }
            if(m_operationsQueue.empty())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
    }
    
    void resource_accessor::on_update(f32 deltatime)
    {
        auto iterator = m_operationsQueue.begin();
        while(iterator != m_operationsQueue.end())
        {
            resource_loading_operation_shared_ptr operation = iterator->second;
            if(operation->get_status() == e_resource_loading_operation_status_waiting)
            {
                operation->commit();
            }
            else if(operation->get_status() == e_resource_loading_operation_status_failure ||
                    operation->get_status() == e_resource_loading_operation_status_success)
            {
                std::lock_guard<std::mutex> guard(m_mutex);
                m_operationsQueue.erase(iterator++);
                
                const auto& resource = m_resources.find(operation->get_guid());
                assert(resource != m_resources.end());
                if(resource != m_resources.end())
                {
                    resource->second->on_resource_loaded(operation->get_status() == e_resource_loading_operation_status_success);
                }
            }
            else
            {
                ++iterator;
            }
        }
        while (!m_resources_need_to_callback.empty())
        {
            std::get<0>(m_resources_need_to_callback.front())->on_resource_loaded(std::get<1>(m_resources_need_to_callback.front()));
            m_resources_need_to_callback.pop();
        }
    }
    
    void resource_accessor::add_custom_resource(const std::string& guid, const resource_shared_ptr& resource)
    {
        m_resources.insert(std::make_pair(guid, resource));
    }
    
    shader_shared_ptr resource_accessor::get_shader(const std::string &vs_filename, const std::string &fs_filename, bool sync)
    {
        std::string guid = std::string().append(vs_filename).append(fs_filename);
        shader_shared_ptr resource = nullptr;
        if(m_resources.find(guid) != m_resources.end())
        {
            resource = std::static_pointer_cast<shader>(m_resources.find(guid)->second);
        }
        else
        {
            resource = std::make_shared<shader>(guid);
            shader_loading_operation_shared_ptr operation = std::make_shared<shader_loading_operation>(vs_filename,
                                                                                                       fs_filename,
                                                                                                       resource);
            m_resources.insert(std::make_pair(guid, resource));
            if(!sync)
            {
                m_operationsQueue.insert(std::make_pair(guid, operation));
            }
            else
            {
                operation->serialize();
                operation->commit();
                
                m_resources_need_to_callback.push(std::make_tuple(resource, operation->get_status() == e_resource_loading_operation_status_success));
            }
        }
        return resource;
    }
    
    texture_shared_ptr resource_accessor::get_texture(const std::string &filename, bool sync, bool mandatory)
    {
        std::string guid = filename;
        texture_shared_ptr resource = nullptr;
        if(m_resources.find(guid) != m_resources.end())
        {
            resource = std::static_pointer_cast<texture>(m_resources.find(guid)->second);
        }
        else if(mandatory)
        {
            resource = std::make_shared<texture>(guid);
            texture_loading_operation_shared_ptr operation = std::make_shared<texture_loading_operation>(filename,
                                                                                                         resource);
            m_resources.insert(std::make_pair(guid, resource));
            if(!sync)
            {
                m_operationsQueue.insert(std::make_pair(guid, operation));
            }
            else
            {
                operation->serialize();
                operation->commit();
                
                m_resources_need_to_callback.push(std::make_tuple(resource, operation->get_status() == e_resource_loading_operation_status_success));
            }
        }
        return resource;
    }
    
    cubemap_texture_shared_ptr resource_accessor::get_cubemap_texture(const std::string& xpositive, const std::string& xnegative,
                                                                      const std::string& ypositive, const std::string& ynegative,
                                                                      const std::string& zpositive, const std::string& znegative,
                                                                      bool sync)
    {
        texture_shared_ptr texture_xpositive = resource_accessor::get_texture(xpositive, sync);
        texture_shared_ptr texture_xnegative = resource_accessor::get_texture(xnegative, sync);
        texture_shared_ptr texture_ypositive = resource_accessor::get_texture(ypositive, sync);
        texture_shared_ptr texture_ynegative = resource_accessor::get_texture(ynegative, sync);
        texture_shared_ptr texture_zpositive = resource_accessor::get_texture(zpositive, sync);
        texture_shared_ptr texture_znegative = resource_accessor::get_texture(znegative, sync);
        
        cubemap_texture_shared_ptr cubemap_texture = std::make_shared<gb::cubemap_texture>(xpositive + xnegative + ypositive + ynegative + zpositive + znegative,
                                                                                           texture_xpositive, texture_xnegative,
                                                                                           texture_ypositive, texture_ynegative,
                                                                                           texture_zpositive, texture_znegative);
        texture_xpositive->add_resource_loading_listener(cubemap_texture);
        texture_xnegative->add_resource_loading_listener(cubemap_texture);
        texture_ypositive->add_resource_loading_listener(cubemap_texture);
        texture_ynegative->add_resource_loading_listener(cubemap_texture);
        texture_zpositive->add_resource_loading_listener(cubemap_texture);
        texture_znegative->add_resource_loading_listener(cubemap_texture);
        return cubemap_texture;
    }
    
    mesh_shared_ptr resource_accessor::get_mesh(const std::string& filename, bool sync)
    {
        std::string guid = filename;
        mesh_shared_ptr resource = nullptr;
        if(m_resources.find(guid) != m_resources.end())
        {
            resource = std::static_pointer_cast<mesh>(m_resources.find(guid)->second);
        }
        else
        {
            resource = std::make_shared<mesh>(guid);
            mesh_loading_operation_shared_ptr operation = std::make_shared<mesh_loading_operation>(filename,
                                                                                                   resource);
            m_resources.insert(std::make_pair(guid, resource));
            if(!sync)
            {
                m_operationsQueue.insert(std::make_pair(guid, operation));
            }
            else
            {
                operation->serialize();
                operation->commit();
                
                m_resources_need_to_callback.push(std::make_tuple(resource, operation->get_status() == e_resource_loading_operation_status_success));
            }
        }
        return resource;
    }
    
    sequence_shared_ptr resource_accessor::get_animation(const std::string& filename, bool sync)
    {
        std::string guid = filename;
        sequence_shared_ptr resource = nullptr;
        if(m_resources.find(guid) != m_resources.end())
        {
            resource = std::static_pointer_cast<sequence>(m_resources.find(guid)->second);
        }
        else
        {
            resource = std::make_shared<sequence>(guid);
            animation_loading_operation_shared_ptr operation = std::make_shared<animation_loading_operation>(filename, resource);
            m_resources.insert(std::make_pair(guid, resource));
            if(!sync)
            {
                m_operationsQueue.insert(std::make_pair(guid, operation));
            }
            else
            {
                operation->serialize();
                operation->commit();
                
                m_resources_need_to_callback.push(std::make_tuple(resource, operation->get_status() == e_resource_loading_operation_status_success));
            }
        }
        return resource;
    }
}