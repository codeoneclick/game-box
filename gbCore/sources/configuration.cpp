//
//  configuration.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/12/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "configuration.h"
#include "common.h"

namespace gb
{
    std::map<std::string, GLenum> g_string_to_glenum = {
        {"GL_FRONT", GL_FRONT},
        {"GL_BACK", GL_BACK},
        {"GL_SRC_ALPHA", GL_SRC_ALPHA},
        {"GL_ONE", GL_ONE},
        {"GL_ONE_MINUS_SRC_ALPHA", GL_ONE_MINUS_SRC_ALPHA},
        {"GL_REPEAT", GL_REPEAT},
        {"GL_CLAMP_TO_EDGE", GL_CLAMP_TO_EDGE},
        {"GL_MIRRORED_REPEAT", GL_MIRRORED_REPEAT},
        {"GL_NEAREST", GL_NEAREST},
        {"GL_LINEAR", GL_LINEAR},
        {"GL_MIPMAP", GL_LINEAR_MIPMAP_NEAREST},
        {"GL_ALWAYS", GL_ALWAYS},
        {"GL_EQUAL", GL_EQUAL},
        {"GL_NOTEQUAL", GL_NOTEQUAL}
    };
    
    std::map<GLenum, std::string> g_glenum_to_string = {
        {GL_FRONT, "GL_FRONT"},
        {GL_BACK, "GL_BACK"},
        {GL_SRC_ALPHA, "GL_SRC_ALPHA"},
        {GL_ONE, "GL_ONE"},
        {GL_ONE_MINUS_SRC_ALPHA, "GL_ONE_MINUS_SRC_ALPHA"},
        {GL_REPEAT, "GL_REPEAT"},
        {GL_CLAMP_TO_EDGE, "GL_CLAMP_TO_EDGE"},
        {GL_MIRRORED_REPEAT, "GL_MIRRORED_REPEAT"},
        {GL_NEAREST, "GL_NEAREST"},
        {GL_LINEAR, "GL_LINEAR"},
        {GL_LINEAR_MIPMAP_NEAREST, "GL_MIPMAP"},
        {GL_ALWAYS, "GL_ALWAYS" },
        {GL_EQUAL, "GL_EQUAL"},
        {GL_NOTEQUAL, "GL_NOTEQUAL"}
    };
    
    configuration::configuration(void)
#if defined(__EDITOR__)
    
    : m_enabled(true)
    
#endif
    {
        
    }
    
    configuration::~configuration(void)
    {
        m_attributes.clear();
        m_configurations.clear();
    }
    
    void configuration::set_attribute(const std::string &attribute_name, const std::shared_ptr<configuration_attribute> &attribute)
    {
        m_attributes[attribute_name] = attribute;
    }
    
    void configuration::set_configuration(const std::string &configuration_name, const std::shared_ptr<configuration> &configuration, i32 index)
    {
        const auto& iterator = m_configurations.find(configuration_name);
        if(iterator != m_configurations.end())
        {
            if(index >= 0 && index < iterator->second.size())
            {
                iterator->second[index] = configuration;
            }
            else
            {
                iterator->second.push_back(configuration);
            }
        }
        else
        {
            std::vector<std::shared_ptr<gb::configuration>> configurations;
            configurations.push_back(configuration);
            m_configurations[configuration_name] = configurations;
        }
    }
    
    pugi::xml_parse_result configuration::open_xml_document(pugi::xml_document &document, const std::string &filename)
    {
        std::string search_filename = filename;
        pugi::xml_parse_result result;
        result = document.load_file(search_filename.c_str());
        if(result.status != pugi::status_ok)
        {
            search_filename = bundlepath().append(search_filename);
            result = document.load_file(search_filename.c_str());
        }
#if defined(__EDITOR__)
        
        if(result.status == pugi::status_ok)
        {
            configuration::set_filename(search_filename);
        }
        
#endif
        return result;
    };
    
#if defined(__EDITOR__)
    
    void configuration::set_filename(const std::string& filename)
    {
        m_filename = filename;
    }
    
#endif
    
    std::string configuration::get_filename(void) const
    {
        return m_filename;
    }
    
    i32 game_object_configuration::get_z_order(void) const
    {
        return -1;
    }
}
