// autogenerated: do not add any changes
#ifndef element_configuration_h
#define element_configuration_h
#include "configuration.h"
namespace gb
{
class element_configuration : public configuration
{
public:
element_configuration(void) = default;
~element_configuration(void) = default;
std::string get_red_3d_configuration_filename(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_red_3d_configuration_filename(std::string red_3d_configuration_filename);
#endif
std::string get_green_3d_configuration_filename(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_green_3d_configuration_filename(std::string green_3d_configuration_filename);
#endif
std::string get_blue_3d_configuration_filename(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_blue_3d_configuration_filename(std::string blue_3d_configuration_filename);
#endif
std::string get_yellow_3d_configuration_filename(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_yellow_3d_configuration_filename(std::string yellow_3d_configuration_filename);
#endif
void serialize_xml(const std::string& filename);
void serialize_json(const std::string& filename);
};
};
#endif
