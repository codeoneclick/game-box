// autogenerated: do not add any changes, added changes will be lost.
#ifndef configuration_accessor_h
#define configuration_accessor_h
#include "material_configuration.h"
#include "output_technique_configuration.h"
#include "sprite_configuration.h"
#include "ss_technique_configuration.h"
#include "transition_configuration.h"
#include "ws_technique_configuration.h"
namespace gb
{
class configuration_accessor
{
public:
configuration_accessor(void) = default;
~configuration_accessor(void) = default;
std::shared_ptr<configuration> get_material_configuration(const std::string& filename) const;
std::shared_ptr<configuration> get_output_technique_configuration(const std::string& filename) const;
std::shared_ptr<configuration> get_sprite_configuration(const std::string& filename) const;
std::shared_ptr<configuration> get_ss_technique_configuration(const std::string& filename) const;
std::shared_ptr<configuration> get_transition_configuration(const std::string& filename) const;
std::shared_ptr<configuration> get_ws_technique_configuration(const std::string& filename) const;
};
};
#endif
