// autogenerated: do not add any changes, added changes will be lost.
#ifndef gameplay_configuration_accessor_h
#define gameplay_configuration_accessor_h
#include "level_configuration.h"
#include "car_configuration.h"
#include "levels_set_configuration.h"
#include "car_progression_configuration.h"
#include "cars_progression_configuration.h"
namespace gb
{
class gameplay_configuration_accessor
{
protected:
std::unordered_map<std::string, std::shared_ptr<configuration>> m_configurations_pool;
public:
gameplay_configuration_accessor(void) = default;
~gameplay_configuration_accessor(void) = default;
std::shared_ptr<configuration> get_level_configuration(const std::string& filename);
std::shared_ptr<configuration> get_car_configuration(const std::string& filename);
std::shared_ptr<configuration> get_levels_set_configuration(const std::string& filename);
std::shared_ptr<configuration> get_car_progression_configuration(const std::string& filename);
std::shared_ptr<configuration> get_cars_progression_configuration(const std::string& filename);
};
};
#endif
