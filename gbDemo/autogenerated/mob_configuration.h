// autogenerated: do not add any changes
#ifndef mob_configuration_h
#define mob_configuration_h
#include "configuration.h"
namespace gb
{
class mob_configuration : public configuration
{
public:
mob_configuration(void) = default;
~mob_configuration(void) = default;
i32 get_id(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_id(i32 id);
#endif
std::string get_character_configuration_filename(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_character_configuration_filename(std::string character_configuration_filename);
#endif
f32 get_max_distance_to_chase(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_max_distance_to_chase(f32 max_distance_to_chase);
#endif
f32 get_max_distance_away_from_spawner(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_max_distance_away_from_spawner(f32 max_distance_away_from_spawner);
#endif
f32 get_spawner_position_x(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_spawner_position_x(f32 spawner_position_x);
#endif
f32 get_spawner_position_y(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_spawner_position_y(f32 spawner_position_y);
#endif
f32 get_max_distance_delta_to_spawn(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_max_distance_delta_to_spawn(f32 max_distance_delta_to_spawn);
#endif
void serialize_xml(const std::string& filename);
void serialize_json(const std::string& filename);
};
};
#endif
