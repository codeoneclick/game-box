// autogenerated: do not add any changes
#ifndef animation_sequence_3d_configuration_h
#define animation_sequence_3d_configuration_h
#include "configuration.h"
namespace gb
{
class animation_sequence_3d_configuration : public configuration
{
public:
animation_sequence_3d_configuration(void) = default;
~animation_sequence_3d_configuration(void) = default;
std::string get_animation_filename(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_animation_filename(std::string animation_filename);
#endif
std::string get_animation_name(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_animation_name(std::string animation_name);
#endif
void serialize_xml(const std::string& filename);
void serialize_json(const std::string& filename);
};
};
#endif
