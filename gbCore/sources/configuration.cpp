//
//  configuration.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/12/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "configuration.h"
#include <fstream>
#include <mutex>
#include <iterator>
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
        {"GL_FUNC_ADD", gl::constant::func_add},
        {"GL_FUNC_MIN", gl::constant::func_min},
        {"GL_FUNC_MAX", gl::constant::func_max},
        {"KEEP", gl::constant::stencil_op_keep},
        {"ZERO", gl::constant::stencil_op_zero},
        {"REPLACE", gl::constant::stencil_op_replace},
        {"INC_CLAMP", gl::constant::stencil_op_inc_clamp},
        {"DEC_CLAMP", gl::constant::stencil_op_dec_clamp},
        {"INV", gl::constant::stencil_op_inv},
        {"INC_WRAP", gl::constant::stencil_op_inc_wrap},
        {"DEC_WRAP", gl::constant::stencil_op_dec_wrap},
        {"LESS", gl::constant::less},
        {"LESS_EQUAL", gl::constant::less_equal},
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
        {gl::constant::func_add, "GL_FUNC_ADD"},
        {gl::constant::func_min, "GL_FUNC_MIN"},
        {gl::constant::func_max, "GL_FUNC_MAX"},
        {gl::constant::stencil_op_keep, "KEEP"},
        {gl::constant::stencil_op_zero, "ZERO"},
        {gl::constant::stencil_op_replace, "REPLACE"},
        {gl::constant::stencil_op_inc_clamp, "INC_CLAMP"},
        {gl::constant::stencil_op_dec_clamp, "DEC_CLAMP"},
        {gl::constant::stencil_op_inv, "INV"},
        {gl::constant::stencil_op_inc_wrap, "INC_WRAP"},
        {gl::constant::stencil_op_dec_wrap, "DEC_WRAP"},
        {gl::constant::less, "LESS"},
        {gl::constant::less_equal, "LESS_EQUAL"},
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
		{ "GL_FUNC_ADD", 22 },
        { "GL_FUNC_MIN", 23 },
        { "GL_FUNC_MAX", 24 }
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
		{ 22, "GL_FUNC_ADD" },
        { 23, "GL_FUNC_MIN" },
        { 24, "GL_FUNC_MAX" }
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
        struct parsed_xml
        {
            std::string contents;
            std::shared_ptr<pugi::xml_document> document;
            pugi::xml_parse_result result;
        };
        static std::mutex cache_mutex;
        static std::map<std::string, parsed_xml> cache;
        static size_t source_bytes = 0;
        const size_t max_source_bytes = 4 * 1024 * 1024;
        const size_t max_documents = 64;
        pugi::xml_parse_result result;
        result.status = pugi::status_file_not_found;
        const std::array<std::string, 2> paths = {filename, bundlepath() + filename};
        for(const auto& path : paths)
        {
            std::ifstream stream(path, std::ios::binary);
            if(!stream) continue;
            const std::string contents((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
            if(stream.bad()) { result.status = pugi::status_io_error; continue; }
            {
                std::lock_guard<std::mutex> guard(cache_mutex);
                const auto found = cache.find(path);
                // Compare actual bytes, preserving reload behavior even for
                // same-size edits within a filesystem timestamp tick.
                if(found != cache.end() && found->second.contents == contents)
                {
                    xml.reset(*found->second.document);
                    m_filename = path;
                    return found->second.result;
                }
            }
            const auto document = std::make_shared<pugi::xml_document>();
            result = document->load_buffer(contents.data(), contents.size());
            if(result.status != pugi::status_ok) continue;
            xml.reset(*document);
            m_filename = path;
            if(contents.size() <= max_source_bytes)
            {
                std::lock_guard<std::mutex> guard(cache_mutex);
                if(cache.size() >= max_documents || source_bytes + contents.size() > max_source_bytes)
                {
                    cache.clear();
                    source_bytes = 0;
                }
                const auto found = cache.find(path);
                if(found != cache.end()) source_bytes -= found->second.contents.size();
                cache[path] = parsed_xml{contents, document, result};
                source_bytes += contents.size();
            }
            return result;
        }
        assert(false);
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
