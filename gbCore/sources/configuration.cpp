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
#if USED_GRAPHICS_API != NO_GRAPHICS_API

    std::map<std::string, ui32> g_string_to_glenum = {
        {"GL_FRONT", gl::constant::front},
        {"GL_BACK", gl::constant::back},
        {"GL_SRC_COLOR", gl::constant::src_color},
        {"GL_SRC_ALPHA", gl::constant::src_alpha},
        {"GL_ONE", gl::constant::one},
        {"GL_ZERO", gl::constant::zero},
        {"GL_ONE_MINUS_SRC_COLOR", gl::constant::one_minus_src_color},
        {"GL_ONE_MINUS_DST_COLOR", gl::constant::one_minus_dst_color},
        {"GL_ONE_MINUS_SRC_ALPHA", gl::constant::one_minus_src_alpha},
        {"GL_ONE_MINUS_DST_ALPHA", gl::constant::one_minus_dst_alpha},
        {"GL_DST_ALPHA", gl::constant::dst_alpha},
        {"GL_CONSTANT_ALPHA", gl::constant::constant_alpha},
        {"GL_REPEAT", gl::constant::repeat},
        {"GL_CLAMP_TO_EDGE", gl::constant::clamp_to_edge},
        {"GL_MIRRORED_REPEAT", gl::constant::mirrored_repeat},
        {"GL_NEAREST", gl::constant::nearest},
        {"GL_LINEAR", gl::constant::linear},
        {"GL_MIPMAP", gl::constant::linear_mipmap_nearest},
        {"GL_ALWAYS", gl::constant::always},
        {"GL_EQUAL", gl::constant::equal},
        {"GL_NOTEQUAL", gl::constant::notequal},
        {"GL_FUNC_ADD", gl::constant::func_add}
    };
    
    std::map<ui32, std::string> g_glenum_to_string = {
        {gl::constant::front, "GL_FRONT"},
        {gl::constant::back, "GL_BACK"},
        {gl::constant::src_color, "GL_SRC_COLOR"},
        {gl::constant::src_alpha, "GL_SRC_ALPHA"},
        {gl::constant::one, "GL_ONE"},
        {gl::constant::zero, "GL_ZERO"},
        {gl::constant::one_minus_src_color, "GL_ONE_MINUS_SRC_COLOR"},
        {gl::constant::one_minus_dst_color, "GL_ONE_MINUS_DST_COLOR"},
        {gl::constant::one_minus_src_alpha, "GL_ONE_MINUS_SRC_ALPHA"},
        {gl::constant::one_minus_dst_alpha, "GL_ONE_MINUS_DST_ALPHA"},
        {gl::constant::dst_alpha, "GL_DST_ALPHA"},
        {gl::constant::constant_alpha, "GL_CONSTANT_ALPHA"},
        {gl::constant::repeat, "GL_REPEAT"},
        {gl::constant::clamp_to_edge, "GL_CLAMP_TO_EDGE"},
        {gl::constant::mirrored_repeat, "GL_MIRRORED_REPEAT"},
        {gl::constant::nearest, "GL_NEAREST"},
        {gl::constant::linear, "GL_LINEAR"},
        {gl::constant::linear_mipmap_nearest, "GL_MIPMAP"},
        {gl::constant::always, "GL_ALWAYS" },
        {gl::constant::equal, "GL_EQUAL"},
        {gl::constant::notequal, "GL_NOTEQUAL"},
        {gl::constant::func_add, "GL_FUNC_ADD"}
    };

#else

	std::map<std::string, ui32> g_string_to_glenum = {
		{ "GL_FRONT", 1 },
		{ "GL_BACK", 2 },
		{ "GL_SRC_COLOR", 3 },
		{ "GL_SRC_ALPHA", 4 },
		{ "GL_ONE", 5 },
		{ "GL_ZERO", 6 },
		{ "GL_ONE_MINUS_SRC_COLOR", 7 },
		{ "GL_ONE_MINUS_DST_COLOR", 8 },
		{ "GL_ONE_MINUS_SRC_ALPHA", 9 },
		{ "GL_ONE_MINUS_DST_ALPHA", 10 },
		{ "GL_DST_ALPHA", 11 },
		{ "GL_CONSTANT_ALPHA", 12 },
		{ "GL_REPEAT", 13 },
		{ "GL_CLAMP_TO_EDGE", 14 },
		{ "GL_MIRRORED_REPEAT", 15 },
		{ "GL_NEAREST", 16 },
		{ "GL_LINEAR", 17 },
		{ "GL_MIPMAP", 18 },
		{ "GL_ALWAYS", 19 },
		{ "GL_EQUAL", 20 },
		{ "GL_NOTEQUAL", 21 },
		{ "GL_FUNC_ADD", 22 }
	};

	std::map<ui32, std::string> g_glenum_to_string = {
		{ 1, "GL_FRONT" },
		{ 2, "GL_BACK" },
		{ 3, "GL_SRC_COLOR" },
		{ 4, "GL_SRC_ALPHA" },
		{ 5, "GL_ONE" },
		{ 6, "GL_ZERO" },
		{ 7, "GL_ONE_MINUS_SRC_COLOR" },
		{ 8, "GL_ONE_MINUS_DST_COLOR" },
		{ 9, "GL_ONE_MINUS_SRC_ALPHA" },
		{ 10, "GL_ONE_MINUS_DST_ALPHA" },
		{ 11, "GL_DST_ALPHA" },
		{ 12, "GL_CONSTANT_ALPHA" },
		{ 13, "GL_REPEAT" },
		{ 14, "GL_CLAMP_TO_EDGE" },
		{ 15, "GL_MIRRORED_REPEAT" },
		{ 16, "GL_NEAREST" },
		{ 17, "GL_LINEAR" },
		{ 18, "GL_MIPMAP" },
		{ 19, "GL_ALWAYS" },
		{ 20, "GL_EQUAL" },
		{ 21, "GL_NOTEQUAL" },
		{ 22, "GL_FUNC_ADD" }
	};

#endif

    
    configuration::configuration(void)
    
#if defined(__IS_CONFIGURATION_MUTABLE__)
    
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
    
    pugi::xml_parse_result configuration::open_xml(pugi::xml_document &xml, const std::string &filename)
    {
        std::string search_filename = filename;
        pugi::xml_parse_result result = xml.load_file(search_filename.c_str());
        if(result.status != pugi::status_ok)
        {
            search_filename = bundlepath().append(search_filename);
            result = xml.load_file(search_filename.c_str());
        }
        
        if(result.status == pugi::status_ok)
        {
            m_filename = search_filename;
        }
        else
        {
            assert(false);
        }
        
        return result;
    };
    
    bool configuration::open_json(Json::Value &json, const std::string& filename)
    {
        std::string search_filename = filename;
        Json::Reader reader;
        std::ifstream stream(search_filename, std::ifstream::binary);
        bool result = reader.parse(stream, json, false);
        if(!result)
        {
            search_filename = bundlepath().append(search_filename);
            std::ifstream stream(search_filename, std::ifstream::binary);
            result = reader.parse(stream, json, false);
        }
        
        if(result)
        {
            m_filename = search_filename;
        }
        else
        {
            assert(false);
        }

        return result;
    }
    
#if defined(__IS_CONFIGURATION_MUTABLE__)
    
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
