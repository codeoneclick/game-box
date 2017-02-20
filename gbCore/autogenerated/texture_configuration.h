// autogenerated: do not add any changes
#ifndef texture_configuration_h
#define texture_configuration_h
#include "configuration.h"
namespace gb
{
class texture_configuration : public configuration
{
public:
texture_configuration(void) = default;
~texture_configuration(void) = default;
std::string get_texture_filename(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_texture_filename(std::string filename);
#endif
std::string get_render_technique_name(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_render_technique_name(std::string render_operation_name);
#endif
ui32 get_sampler_index(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_sampler_index(ui32 sampler_index);
#endif
ui32 get_wrap_mode(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_wrap_mode(ui32 wrap_mode);
#endif
ui32 get_mag_filter(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_mag_filter(ui32 mag_filter);
#endif
ui32 get_min_filter(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_min_filter(ui32 min_filter);
#endif
void serialize_xml(pugi::xml_document& document, const std::string& path);
void serialize_json(Json::Value& root);
void serialize_xml(pugi::xml_document& document, pugi::xpath_node& node);
};
};
#endif
