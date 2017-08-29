// autogenerated: do not add any changes
#include "heightmap_configuration.h"
namespace gb
{
std::string heightmap_configuration::get_heightmap_data_filename(void) const
{
const auto& iterator = m_attributes.find("/heightmap/heightmap_data_filename");
assert(iterator != m_attributes.end());
std::string value; iterator->second->get(&value);
return value;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void heightmap_configuration::set_heightmap_data_filename(std::string heightmap_data_filename)
{
configuration::set_attribute("/heightmap/heightmap_data_filename", std::make_shared<configuration_attribute>(heightmap_data_filename));
}
#endif
i32 heightmap_configuration::get_heightmap_chunk_size_x(void) const
{
const auto& iterator = m_attributes.find("/heightmap/heightmap_chunk_size_x");
assert(iterator != m_attributes.end());
i32 value; iterator->second->get(&value);
return value;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void heightmap_configuration::set_heightmap_chunk_size_x(i32 heightmap_chunk_size_x)
{
configuration::set_attribute("/heightmap/heightmap_chunk_size_x", std::make_shared<configuration_attribute>(heightmap_chunk_size_x));
}
#endif
i32 heightmap_configuration::get_heightmap_chunk_size_y(void) const
{
const auto& iterator = m_attributes.find("/heightmap/heightmap_chunk_size_y");
assert(iterator != m_attributes.end());
i32 value; iterator->second->get(&value);
return value;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void heightmap_configuration::set_heightmap_chunk_size_y(i32 heightmap_chunk_size_y)
{
configuration::set_attribute("/heightmap/heightmap_chunk_size_y", std::make_shared<configuration_attribute>(heightmap_chunk_size_y));
}
#endif
std::vector<std::shared_ptr<configuration>> heightmap_configuration::get_materials_configurations(void) const
{
const auto& iterator = m_configurations.find("/heightmap/materials/material");
if(iterator == m_configurations.end())
{
return std::vector<std::shared_ptr<configuration>>();
}
assert(iterator != m_configurations.end());
return iterator->second;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void heightmap_configuration::add_materials_configurations(const std::shared_ptr<gb::material_configuration>& material)
{
configuration::set_configuration("/heightmap/materials/material", material);
}
#endif
#if defined(__IS_CONFIGURATION_MUTABLE__)
void heightmap_configuration::set_materials_configurations(const std::shared_ptr<gb::material_configuration>& material, i32 index)
{
configuration::set_configuration("/heightmap/materials/material", material, index);
}
#endif
void heightmap_configuration::serialize_xml(const std::string& filename)
{
pugi::xml_document document;
pugi::xml_parse_result result = configuration::open_xml(document, filename);
assert(result.status == pugi::status_ok);
pugi::xpath_node node;
node = document.select_single_node("/heightmap");
std::string heightmap_data_filename = node.node().attribute("heightmap_data_filename").as_string();
configuration::set_attribute("/heightmap/heightmap_data_filename", std::make_shared<configuration_attribute>(heightmap_data_filename));
i32 heightmap_chunk_size_x = node.node().attribute("heightmap_chunk_size_x").as_int();
configuration::set_attribute("/heightmap/heightmap_chunk_size_x", std::make_shared<configuration_attribute>(heightmap_chunk_size_x));
i32 heightmap_chunk_size_y = node.node().attribute("heightmap_chunk_size_y").as_int();
configuration::set_attribute("/heightmap/heightmap_chunk_size_y", std::make_shared<configuration_attribute>(heightmap_chunk_size_y));
pugi::xpath_node_set material_nodes = document.select_nodes("/heightmap/materials/material");
for (pugi::xpath_node_set::const_iterator iterator = material_nodes.begin(); iterator != material_nodes.end(); ++iterator)
{
std::shared_ptr<gb::material_configuration> material = std::make_shared<gb::material_configuration>();
std::string external_filename =(*iterator).node().attribute("filename").as_string();
if(external_filename.find(".xml") != std::string::npos)
{
material->serialize_xml(external_filename);
}
else if(external_filename.find(".json") != std::string::npos)
{
material->serialize_json(external_filename);
}
else
{
assert(false);
}
configuration::set_configuration("/heightmap/materials/material", material);
}
}
void heightmap_configuration::serialize_json(const std::string& filename)
{
Json::Value json;
bool result = configuration::open_json(json, filename);
assert(result);
std::string heightmap_data_filename = json.get("heightmap_data_filename", "unknown").asString();
configuration::set_attribute("/heightmap/heightmap_data_filename", std::make_shared<configuration_attribute>(heightmap_data_filename));
i32 heightmap_chunk_size_x = json.get("heightmap_chunk_size_x", 0).asInt();
configuration::set_attribute("/heightmap/heightmap_chunk_size_x", std::make_shared<configuration_attribute>(heightmap_chunk_size_x));
i32 heightmap_chunk_size_y = json.get("heightmap_chunk_size_y", 0).asInt();
configuration::set_attribute("/heightmap/heightmap_chunk_size_y", std::make_shared<configuration_attribute>(heightmap_chunk_size_y));
Json::Value materials_json_array = json["materials"];
for (Json::ValueIterator iterator = materials_json_array.begin(); iterator != materials_json_array.end(); ++iterator)
{
std::shared_ptr<gb::material_configuration> material = std::make_shared<gb::material_configuration>();
Json::Value json_value = (*iterator);
std::string external_filename =json_value.get("filename", "unknown").asString();
if(external_filename.find(".xml") != std::string::npos)
{
material->serialize_xml(external_filename);
}
else if(external_filename.find(".json") != std::string::npos)
{
material->serialize_json(external_filename);
}
else
{
assert(false);
}
configuration::set_configuration("/heightmap/materials/material", material);
}
}
}