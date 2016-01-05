//
//  configuration.h
//  gbCore
//
//  Created by sergey.sergeev on 8/12/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef configuration_h
#define configuration_h

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
    class configuration_attribute
    {
    private:
        
    protected:
        
        std::tuple<std::shared_ptr<bool>,
        std::shared_ptr<i8>,
        std::shared_ptr<ui8>,
        std::shared_ptr<i16>,
        std::shared_ptr<ui16>,
        std::shared_ptr<i32>,
        std::shared_ptr<ui32>,
        std::shared_ptr<f32>,
        std::shared_ptr<std::string>
        > m_container;
        
    public:
        
        configuration_attribute(bool value)
        {
            m_container = std::make_tuple(std::make_shared<bool>(value),
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr);
        };
        
        configuration_attribute(i8 value)
        {
            m_container = std::make_tuple(nullptr,
                                          std::make_shared<i8>(value),
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr);
        };
        
        configuration_attribute(ui8 value)
        {
            m_container = std::make_tuple(nullptr,
                                          nullptr,
                                          std::make_shared<ui8>(value),
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr);
        };
        
        configuration_attribute(i16 value)
        {
            m_container = std::make_tuple(nullptr,
                                          nullptr,
                                          nullptr,
                                          std::make_shared<i16>(value),
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr);
        };
        
        configuration_attribute(ui16 value)
        {
            m_container = std::make_tuple(nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          std::make_shared<ui16>(value),
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr);
        };
        
        configuration_attribute(i32 value)
        {
            m_container = std::make_tuple(nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          std::make_shared<i32>(value),
                                          nullptr,
                                          nullptr,
                                          nullptr);
        };
        
        configuration_attribute(ui32 value)
        {
            m_container = std::make_tuple(nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          std::make_shared<ui32>(value),
                                          nullptr,
                                          nullptr);
        };
        
        configuration_attribute(f32 value)
        {
            m_container = std::make_tuple(nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          std::make_shared<f32>(value),
                                          nullptr);
        };
        
        configuration_attribute(const std::string& value)
        {
            m_container = std::make_tuple(nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          std::make_shared<std::string>(value));
        };
        
        void get(bool *value) const
        {
            (*value) = (*std::get<0>(m_container).get());
        };
        
        void get(i8 *value) const
        {
            (*value) = (*std::get<1>(m_container).get());
        };
        
        void get(ui8 *value) const
        {
            (*value) = (*std::get<2>(m_container).get());
        };
        
        void get(i16 *value) const
        {
            (*value) = (*std::get<3>(m_container).get());
        };
        
        void get(ui16 *value) const
        {
            (*value) = (*std::get<4>(m_container).get());
        };
        
        void get(i32 *value) const
        {
            (*value) = (*std::get<5>(m_container).get());
        };
        
        void get(ui32 *value) const
        {
            (*value) = (*std::get<6>(m_container).get());
        };
        
        void get(f32 *value) const
        {
            (*value) = (*std::get<7>(m_container).get());
        };
        
        void get(std::string *value) const
        {
            (*value) = (*std::get<8>(m_container).get());
        };
    };
    
    extern std::map<std::string, GLenum> g_string_to_glenum;
    extern std::map<GLenum, std::string> g_glenum_to_string;
    
    class configuration
    {
    private:
        
    protected:
        
#if defined(__EDITOR__)
        
        bool m_enabled;
        
#endif
        
        std::string m_filename;
        std::unordered_map<std::string, std::shared_ptr<configuration_attribute>> m_attributes;
        std::unordered_map<std::string, std::vector<std::shared_ptr<configuration>>> m_configurations;
        
        configuration(void);
        
    public:
        
        virtual ~configuration(void);
        
        void set_attribute(const std::string& attribute_name,
                           const std::shared_ptr<configuration_attribute>& attribute);
        void set_configuration(const std::string& configuration_name,
                               const std::shared_ptr<configuration>& configuration,
                               i32 index = -1);
        pugi::xml_parse_result open_xml_document(pugi::xml_document &document,
                                                 const std::string &filename);
        
#if defined(__EDITOR__)
        
        void set_filename(const std::string& filename);
        void set_enabled(bool value);
        bool is_enabled(void) const;
        
#endif
        
        std::string get_filename(void) const;
        
    };
    
    class game_object_configuration : public configuration
    {
    private:
        
    protected:
        
    public:
        
        game_object_configuration() = default;
        virtual ~game_object_configuration(void) = default;
        virtual std::vector<std::shared_ptr<configuration>> get_materials_configurations(void) const = 0;
        virtual i32 get_z_order(void) const;
    };
};

#endif
