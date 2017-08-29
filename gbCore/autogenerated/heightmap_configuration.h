// autogenerated: do not add any changes
#ifndef heightmap_configuration_h
#define heightmap_configuration_h
#include "configuration.h"
namespace gb
{
class heightmap_configuration : public configuration
{
public:
heightmap_configuration(void) = default;
~heightmap_configuration(void) = default;
std::string get_heightmap_data_filename(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_heightmap_data_filename(std::string heightmap_data_filename);
#endif
i32 get_heightmap_chunk_size_x(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_heightmap_chunk_size_x(i32 heightmap_chunk_size_x);
#endif
i32 get_heightmap_chunk_size_y(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_heightmap_chunk_size_y(i32 heightmap_chunk_size_y);
#endif
std::string get_splatting_diffuse_texture_preprocessing_material_filename(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_splatting_diffuse_texture_preprocessing_material_filename(std::string splatting_diffuse_texture_preprocessing_material_filename);
#endif
std::string get_splatting_normal_texture_preprocessing_material_filename(void) const;
#if defined(__IS_CONFIGURATION_MUTABLE__)
void set_splatting_normal_texture_preprocessing_material_filename(std::string splatting_normal_texture_preprocessing_material_filename);
#endif
void serialize_xml(const std::string& filename);
void serialize_json(const std::string& filename);
};
};
#endif
