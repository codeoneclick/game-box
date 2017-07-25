// autogenerated: do not add any changes
#ifndef animated_sprite_configuration_h
#define animated_sprite_configuration_h
#include "configuration.h"
#include "material_configuration.h"
namespace gb
{
class animated_sprite_configuration : public game_object_configuration
{
public:
animated_sprite_configuration(void) = default;
~animated_sprite_configuration(void) = default;
std::string get_animation_filename(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_animation_filename(std::string animation_filename);
#endif
std::vector<std::shared_ptr<configuration>> get_materials_configurations(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void add_materials_configurations(const std::shared_ptr<gb::material_configuration>& material);
#endif
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_materials_configurations(const std::shared_ptr<gb::material_configuration>& material, i32 index);
#endif
void serialize_xml(const std::string& filename);
void serialize_json(const std::string& filename);
};
};
#endif
