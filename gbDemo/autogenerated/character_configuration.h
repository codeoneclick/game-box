// autogenerated: do not add any changes
#ifndef character_configuration_h
#define character_configuration_h
#include "configuration.h"
namespace gb
{
class character_configuration : public configuration
{
public:
character_configuration(void) = default;
~character_configuration(void) = default;
std::string get_main_3d_configuration_filename(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_main_3d_configuration_filename(std::string main_3d_configuration_filename);
#endif
std::string get_avatar_3d_configuration_filename(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_avatar_3d_configuration_filename(std::string avatar_3d_configuration_filename);
#endif
std::string get_main_2d_configuration_filename(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_main_2d_configuration_filename(std::string main_2d_configuration_filename);
#endif
std::string get_avatar_2d_configuration_filename(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_avatar_2d_configuration_filename(std::string avatar_2d_configuration_filename);
#endif
std::string get_light_source_configuration_filename(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_light_source_configuration_filename(std::string light_source_configuration_filename);
#endif
std::string get_bounds_configuration_filename(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_bounds_configuration_filename(std::string bounds_configuration_filename);
#endif
i32 get_visual_size(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_visual_size(i32 visual_size);
#endif
i32 get_bounds_size(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_bounds_size(i32 bounds_size);
#endif
f32 get_hp(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_hp(f32 hp);
#endif
f32 get_damage(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_damage(f32 damage);
#endif
f32 get_move_speed(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_move_speed(f32 move_speed);
#endif
f32 get_attack_speed(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_attack_speed(f32 attack_speed);
#endif
f32 get_attack_distance(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_attack_distance(f32 attack_distance);
#endif
f32 get_reviving_time(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_reviving_time(f32 reviving_time);
#endif
void serialize_xml(const std::string& filename);
void serialize_json(const std::string& filename);
};
};
#endif
