// autogenerated: do not add any changes, added changes will be lost.
#include "configuration_accessor.h"
namespace gb
{
std::shared_ptr<configuration> configuration_accessor::get_material_configuration(const std::string& filename) const
{
std::shared_ptr<material_configuration> configuration = std::make_shared<material_configuration>();
configuration->serialize(filename);
assert(configuration);
return configuration;
}
std::shared_ptr<configuration> configuration_accessor::get_output_technique_configuration(const std::string& filename) const
{
std::shared_ptr<output_technique_configuration> configuration = std::make_shared<output_technique_configuration>();
configuration->serialize(filename);
assert(configuration);
return configuration;
}
std::shared_ptr<configuration> configuration_accessor::get_shape_configuration(const std::string& filename) const
{
std::shared_ptr<shape_configuration> configuration = std::make_shared<shape_configuration>();
configuration->serialize(filename);
assert(configuration);
return configuration;
}
std::shared_ptr<configuration> configuration_accessor::get_ss_technique_configuration(const std::string& filename) const
{
std::shared_ptr<ss_technique_configuration> configuration = std::make_shared<ss_technique_configuration>();
configuration->serialize(filename);
assert(configuration);
return configuration;
}
std::shared_ptr<configuration> configuration_accessor::get_transition_configuration(const std::string& filename) const
{
std::shared_ptr<transition_configuration> configuration = std::make_shared<transition_configuration>();
configuration->serialize(filename);
assert(configuration);
return configuration;
}
std::shared_ptr<configuration> configuration_accessor::get_ws_technique_configuration(const std::string& filename) const
{
std::shared_ptr<ws_technique_configuration> configuration = std::make_shared<ws_technique_configuration>();
configuration->serialize(filename);
assert(configuration);
return configuration;
}
}
