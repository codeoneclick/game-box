// autogenerated: do not add any changes
#ifndef attachment_configuration_h
#define attachment_configuration_h
#include "configuration.h"
namespace gb
{
class attachment_configuration : public configuration
{
public:
attachment_configuration(void) = default;
~attachment_configuration(void) = default;
std::string get_name(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_name(std::string name);
#endif
ui32 get_pixel_format(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_pixel_format(ui32 pixel_format);
#endif
void serialize_xml(pugi::xml_document& document, const std::string& path);
void serialize_json(Json::Value& root);
void serialize_xml(pugi::xml_document& document, pugi::xpath_node& node);
};
};
#endif