// autogenerated: do not add any changes
#include "particle_emitter_configuration.h"
namespace gb
{
i32 particle_emitter_configuration::get_z_order(void) const
{
const auto& iterator = m_attributes.find("/particle_emitter/z_order");
assert(iterator != m_attributes.end());
i32 value; iterator->second->get(&value);
return value;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void particle_emitter_configuration::set_z_order(i32 z_order)
{
configuration::set_attribute("/particle_emitter/z_order", std::make_shared<configuration_attribute>(z_order));
}
#endif
ui32 particle_emitter_configuration::get_num_particles(void) const
{
const auto& iterator = m_attributes.find("/particle_emitter/num_particles");
assert(iterator != m_attributes.end());
ui32 value; iterator->second->get(&value);
return value;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void particle_emitter_configuration::set_num_particles(ui32 num_particles)
{
configuration::set_attribute("/particle_emitter/num_particles", std::make_shared<configuration_attribute>(num_particles));
}
#endif
ui32 particle_emitter_configuration::get_duration(void) const
{
const auto& iterator = m_attributes.find("/particle_emitter/duration");
assert(iterator != m_attributes.end());
ui32 value; iterator->second->get(&value);
return value;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void particle_emitter_configuration::set_duration(ui32 duration)
{
configuration::set_attribute("/particle_emitter/duration", std::make_shared<configuration_attribute>(duration));
}
#endif
f32 particle_emitter_configuration::get_duration_randomess(void) const
{
const auto& iterator = m_attributes.find("/particle_emitter/duration_randomness");
assert(iterator != m_attributes.end());
f32 value; iterator->second->get(&value);
return value;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void particle_emitter_configuration::set_duration_randomess(f32 duration_randomness)
{
configuration::set_attribute("/particle_emitter/duration_randomness", std::make_shared<configuration_attribute>(duration_randomness));
}
#endif
f32 particle_emitter_configuration::get_velocity_sensitivity(void) const
{
const auto& iterator = m_attributes.find("/particle_emitter/velocity_sensitivity");
assert(iterator != m_attributes.end());
f32 value; iterator->second->get(&value);
return value;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void particle_emitter_configuration::set_velocity_sensitivity(f32 velocity_sensitivity)
{
configuration::set_attribute("/particle_emitter/velocity_sensitivity", std::make_shared<configuration_attribute>(velocity_sensitivity));
}
#endif
f32 particle_emitter_configuration::get_min_horizontal_velocity(void) const
{
const auto& iterator = m_attributes.find("/particle_emitter/min_horizontal_velocity");
assert(iterator != m_attributes.end());
f32 value; iterator->second->get(&value);
return value;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void particle_emitter_configuration::set_min_horizontal_velocity(f32 min_horizontal_velocity)
{
configuration::set_attribute("/particle_emitter/min_horizontal_velocity", std::make_shared<configuration_attribute>(min_horizontal_velocity));
}
#endif
f32 particle_emitter_configuration::get_max_horizontal_velocity(void) const
{
const auto& iterator = m_attributes.find("/particle_emitter/max_horizontal_velocity");
assert(iterator != m_attributes.end());
f32 value; iterator->second->get(&value);
return value;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void particle_emitter_configuration::set_max_horizontal_velocity(f32 max_horizontal_velocity)
{
configuration::set_attribute("/particle_emitter/max_horizontal_velocity", std::make_shared<configuration_attribute>(max_horizontal_velocity));
}
#endif
f32 particle_emitter_configuration::get_min_vertical_velocity(void) const
{
const auto& iterator = m_attributes.find("/particle_emitter/min_vertical_velocity");
assert(iterator != m_attributes.end());
f32 value; iterator->second->get(&value);
return value;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void particle_emitter_configuration::set_min_vertical_velocity(f32 min_vertical_velocity)
{
configuration::set_attribute("/particle_emitter/min_vertical_velocity", std::make_shared<configuration_attribute>(min_vertical_velocity));
}
#endif
f32 particle_emitter_configuration::get_max_vertical_velocity(void) const
{
const auto& iterator = m_attributes.find("/particle_emitter/max_vertical_velocity");
assert(iterator != m_attributes.end());
f32 value; iterator->second->get(&value);
return value;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void particle_emitter_configuration::set_max_vertical_velocity(f32 max_vertical_velocity)
{
configuration::set_attribute("/particle_emitter/max_vertical_velocity", std::make_shared<configuration_attribute>(max_vertical_velocity));
}
#endif
f32 particle_emitter_configuration::get_end_velocity(void) const
{
const auto& iterator = m_attributes.find("/particle_emitter/end_velocity");
assert(iterator != m_attributes.end());
f32 value; iterator->second->get(&value);
return value;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void particle_emitter_configuration::set_end_velocity(f32 end_velocity)
{
configuration::set_attribute("/particle_emitter/end_velocity", std::make_shared<configuration_attribute>(end_velocity));
}
#endif
f32 particle_emitter_configuration::get_gravity_x(void) const
{
const auto& iterator = m_attributes.find("/particle_emitter/gravity_x");
assert(iterator != m_attributes.end());
f32 value; iterator->second->get(&value);
return value;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void particle_emitter_configuration::set_gravity_x(f32 gravity_x)
{
configuration::set_attribute("/particle_emitter/gravity_x", std::make_shared<configuration_attribute>(gravity_x));
}
#endif
f32 particle_emitter_configuration::get_gravity_y(void) const
{
const auto& iterator = m_attributes.find("/particle_emitter/gravity_y");
assert(iterator != m_attributes.end());
f32 value; iterator->second->get(&value);
return value;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void particle_emitter_configuration::set_gravity_y(f32 gravity_y)
{
configuration::set_attribute("/particle_emitter/gravity_y", std::make_shared<configuration_attribute>(gravity_y));
}
#endif
f32 particle_emitter_configuration::get_gravity_z(void) const
{
const auto& iterator = m_attributes.find("/particle_emitter/gravity_z");
assert(iterator != m_attributes.end());
f32 value; iterator->second->get(&value);
return value;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void particle_emitter_configuration::set_gravity_z(f32 gravity_z)
{
configuration::set_attribute("/particle_emitter/gravity_z", std::make_shared<configuration_attribute>(gravity_z));
}
#endif
ui8 particle_emitter_configuration::get_source_color_r(void) const
{
const auto& iterator = m_attributes.find("/particle_emitter/source_color_r");
assert(iterator != m_attributes.end());
ui8 value; iterator->second->get(&value);
return value;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void particle_emitter_configuration::set_source_color_r(ui8 source_color_r)
{
configuration::set_attribute("/particle_emitter/source_color_r", std::make_shared<configuration_attribute>(source_color_r));
}
#endif
ui8 particle_emitter_configuration::get_source_color_g(void) const
{
const auto& iterator = m_attributes.find("/particle_emitter/source_color_g");
assert(iterator != m_attributes.end());
ui8 value; iterator->second->get(&value);
return value;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void particle_emitter_configuration::set_source_color_g(ui8 source_color_g)
{
configuration::set_attribute("/particle_emitter/source_color_g", std::make_shared<configuration_attribute>(source_color_g));
}
#endif
ui8 particle_emitter_configuration::get_source_color_b(void) const
{
const auto& iterator = m_attributes.find("/particle_emitter/source_color_b");
assert(iterator != m_attributes.end());
ui8 value; iterator->second->get(&value);
return value;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void particle_emitter_configuration::set_source_color_b(ui8 source_color_b)
{
configuration::set_attribute("/particle_emitter/source_color_b", std::make_shared<configuration_attribute>(source_color_b));
}
#endif
ui8 particle_emitter_configuration::get_source_color_a(void) const
{
const auto& iterator = m_attributes.find("/particle_emitter/source_color_a");
assert(iterator != m_attributes.end());
ui8 value; iterator->second->get(&value);
return value;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void particle_emitter_configuration::set_source_color_a(ui8 source_color_a)
{
configuration::set_attribute("/particle_emitter/source_color_a", std::make_shared<configuration_attribute>(source_color_a));
}
#endif
ui8 particle_emitter_configuration::get_destination_color_r(void) const
{
const auto& iterator = m_attributes.find("/particle_emitter/destination_color_r");
assert(iterator != m_attributes.end());
ui8 value; iterator->second->get(&value);
return value;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void particle_emitter_configuration::set_destination_color_r(ui8 destination_color_r)
{
configuration::set_attribute("/particle_emitter/destination_color_r", std::make_shared<configuration_attribute>(destination_color_r));
}
#endif
ui8 particle_emitter_configuration::get_destination_color_g(void) const
{
const auto& iterator = m_attributes.find("/particle_emitter/destination_color_g");
assert(iterator != m_attributes.end());
ui8 value; iterator->second->get(&value);
return value;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void particle_emitter_configuration::set_destination_color_g(ui8 destination_color_g)
{
configuration::set_attribute("/particle_emitter/destination_color_g", std::make_shared<configuration_attribute>(destination_color_g));
}
#endif
ui8 particle_emitter_configuration::get_destination_color_b(void) const
{
const auto& iterator = m_attributes.find("/particle_emitter/destination_color_b");
assert(iterator != m_attributes.end());
ui8 value; iterator->second->get(&value);
return value;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void particle_emitter_configuration::set_destination_color_b(ui8 destination_color_b)
{
configuration::set_attribute("/particle_emitter/destination_color_b", std::make_shared<configuration_attribute>(destination_color_b));
}
#endif
ui8 particle_emitter_configuration::get_destination_color_a(void) const
{
const auto& iterator = m_attributes.find("/particle_emitter/destination_color_a");
assert(iterator != m_attributes.end());
ui8 value; iterator->second->get(&value);
return value;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void particle_emitter_configuration::set_destination_color_a(ui8 destination_color_a)
{
configuration::set_attribute("/particle_emitter/destination_color_a", std::make_shared<configuration_attribute>(destination_color_a));
}
#endif
f32 particle_emitter_configuration::get_source_size_x(void) const
{
const auto& iterator = m_attributes.find("/particle_emitter/source_size_x");
assert(iterator != m_attributes.end());
f32 value; iterator->second->get(&value);
return value;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void particle_emitter_configuration::set_source_size_x(f32 source_size_x)
{
configuration::set_attribute("/particle_emitter/source_size_x", std::make_shared<configuration_attribute>(source_size_x));
}
#endif
f32 particle_emitter_configuration::get_source_size_y(void) const
{
const auto& iterator = m_attributes.find("/particle_emitter/source_size_y");
assert(iterator != m_attributes.end());
f32 value; iterator->second->get(&value);
return value;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void particle_emitter_configuration::set_source_size_y(f32 source_size_y)
{
configuration::set_attribute("/particle_emitter/source_size_y", std::make_shared<configuration_attribute>(source_size_y));
}
#endif
f32 particle_emitter_configuration::get_destination_size_x(void) const
{
const auto& iterator = m_attributes.find("/particle_emitter/destination_size_x");
assert(iterator != m_attributes.end());
f32 value; iterator->second->get(&value);
return value;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void particle_emitter_configuration::set_destination_size_x(f32 destination_size_x)
{
configuration::set_attribute("/particle_emitter/destination_size_x", std::make_shared<configuration_attribute>(destination_size_x));
}
#endif
f32 particle_emitter_configuration::get_destination_size_y(void) const
{
const auto& iterator = m_attributes.find("/particle_emitter/destination_size_y");
assert(iterator != m_attributes.end());
f32 value; iterator->second->get(&value);
return value;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void particle_emitter_configuration::set_destination_size_y(f32 destination_size_y)
{
configuration::set_attribute("/particle_emitter/destination_size_y", std::make_shared<configuration_attribute>(destination_size_y));
}
#endif
ui32 particle_emitter_configuration::get_min_emitt_interval(void) const
{
const auto& iterator = m_attributes.find("/particle_emitter/min_emitt_interval");
assert(iterator != m_attributes.end());
ui32 value; iterator->second->get(&value);
return value;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void particle_emitter_configuration::set_min_emitt_interval(ui32 min_emitt_interval)
{
configuration::set_attribute("/particle_emitter/min_emitt_interval", std::make_shared<configuration_attribute>(min_emitt_interval));
}
#endif
ui32 particle_emitter_configuration::get_max_emitt_interval(void) const
{
const auto& iterator = m_attributes.find("/particle_emitter/max_emitt_interval");
assert(iterator != m_attributes.end());
ui32 value; iterator->second->get(&value);
return value;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void particle_emitter_configuration::set_max_emitt_interval(ui32 max_emitt_interval)
{
configuration::set_attribute("/particle_emitter/max_emitt_interval", std::make_shared<configuration_attribute>(max_emitt_interval));
}
#endif
std::vector<std::shared_ptr<configuration>> particle_emitter_configuration::get_materials_configurations(void) const
{
const auto& iterator = m_configurations.find("/particle_emitter/materials/material");
if(iterator == m_configurations.end())
{
return std::vector<std::shared_ptr<configuration>>();
}
assert(iterator != m_configurations.end());
return iterator->second;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void particle_emitter_configuration::add_materials_configurations(const std::shared_ptr<gb::material_configuration>& material)
{
configuration::set_configuration("/particle_emitter/materials/material", material);
}
#endif
#if defined(__IS_CONFIGURATION_MUTABLE__)
void particle_emitter_configuration::set_materials_configurations(const std::shared_ptr<gb::material_configuration>& material, i32 index)
{
configuration::set_configuration("/particle_emitter/materials/material", material, index);
}
#endif
void particle_emitter_configuration::serialize_xml(const std::string& filename)
{
pugi::xml_document document;
pugi::xml_parse_result result = configuration::open_xml(document, filename);
assert(result.status == pugi::status_ok);
pugi::xpath_node node;
node = document.select_single_node("/particle_emitter");
i32 z_order = node.node().attribute("z_order").as_int();
configuration::set_attribute("/particle_emitter/z_order", std::make_shared<configuration_attribute>(z_order));
ui32 num_particles = node.node().attribute("num_particles").as_uint();
configuration::set_attribute("/particle_emitter/num_particles", std::make_shared<configuration_attribute>(num_particles));
ui32 duration = node.node().attribute("duration").as_uint();
configuration::set_attribute("/particle_emitter/duration", std::make_shared<configuration_attribute>(duration));
f32 duration_randomness = node.node().attribute("duration_randomness").as_float();
configuration::set_attribute("/particle_emitter/duration_randomness", std::make_shared<configuration_attribute>(duration_randomness));
f32 velocity_sensitivity = node.node().attribute("velocity_sensitivity").as_float();
configuration::set_attribute("/particle_emitter/velocity_sensitivity", std::make_shared<configuration_attribute>(velocity_sensitivity));
f32 min_horizontal_velocity = node.node().attribute("min_horizontal_velocity").as_float();
configuration::set_attribute("/particle_emitter/min_horizontal_velocity", std::make_shared<configuration_attribute>(min_horizontal_velocity));
f32 max_horizontal_velocity = node.node().attribute("max_horizontal_velocity").as_float();
configuration::set_attribute("/particle_emitter/max_horizontal_velocity", std::make_shared<configuration_attribute>(max_horizontal_velocity));
f32 min_vertical_velocity = node.node().attribute("min_vertical_velocity").as_float();
configuration::set_attribute("/particle_emitter/min_vertical_velocity", std::make_shared<configuration_attribute>(min_vertical_velocity));
f32 max_vertical_velocity = node.node().attribute("max_vertical_velocity").as_float();
configuration::set_attribute("/particle_emitter/max_vertical_velocity", std::make_shared<configuration_attribute>(max_vertical_velocity));
f32 end_velocity = node.node().attribute("end_velocity").as_float();
configuration::set_attribute("/particle_emitter/end_velocity", std::make_shared<configuration_attribute>(end_velocity));
f32 gravity_x = node.node().attribute("gravity_x").as_float();
configuration::set_attribute("/particle_emitter/gravity_x", std::make_shared<configuration_attribute>(gravity_x));
f32 gravity_y = node.node().attribute("gravity_y").as_float();
configuration::set_attribute("/particle_emitter/gravity_y", std::make_shared<configuration_attribute>(gravity_y));
f32 gravity_z = node.node().attribute("gravity_z").as_float();
configuration::set_attribute("/particle_emitter/gravity_z", std::make_shared<configuration_attribute>(gravity_z));
ui8 source_color_r = node.node().attribute("source_color_r").as_uint();
configuration::set_attribute("/particle_emitter/source_color_r", std::make_shared<configuration_attribute>(source_color_r));
ui8 source_color_g = node.node().attribute("source_color_g").as_uint();
configuration::set_attribute("/particle_emitter/source_color_g", std::make_shared<configuration_attribute>(source_color_g));
ui8 source_color_b = node.node().attribute("source_color_b").as_uint();
configuration::set_attribute("/particle_emitter/source_color_b", std::make_shared<configuration_attribute>(source_color_b));
ui8 source_color_a = node.node().attribute("source_color_a").as_uint();
configuration::set_attribute("/particle_emitter/source_color_a", std::make_shared<configuration_attribute>(source_color_a));
ui8 destination_color_r = node.node().attribute("destination_color_r").as_uint();
configuration::set_attribute("/particle_emitter/destination_color_r", std::make_shared<configuration_attribute>(destination_color_r));
ui8 destination_color_g = node.node().attribute("destination_color_g").as_uint();
configuration::set_attribute("/particle_emitter/destination_color_g", std::make_shared<configuration_attribute>(destination_color_g));
ui8 destination_color_b = node.node().attribute("destination_color_b").as_uint();
configuration::set_attribute("/particle_emitter/destination_color_b", std::make_shared<configuration_attribute>(destination_color_b));
ui8 destination_color_a = node.node().attribute("destination_color_a").as_uint();
configuration::set_attribute("/particle_emitter/destination_color_a", std::make_shared<configuration_attribute>(destination_color_a));
f32 source_size_x = node.node().attribute("source_size_x").as_float();
configuration::set_attribute("/particle_emitter/source_size_x", std::make_shared<configuration_attribute>(source_size_x));
f32 source_size_y = node.node().attribute("source_size_y").as_float();
configuration::set_attribute("/particle_emitter/source_size_y", std::make_shared<configuration_attribute>(source_size_y));
f32 destination_size_x = node.node().attribute("destination_size_x").as_float();
configuration::set_attribute("/particle_emitter/destination_size_x", std::make_shared<configuration_attribute>(destination_size_x));
f32 destination_size_y = node.node().attribute("destination_size_y").as_float();
configuration::set_attribute("/particle_emitter/destination_size_y", std::make_shared<configuration_attribute>(destination_size_y));
ui32 min_emitt_interval = node.node().attribute("min_emitt_interval").as_uint();
configuration::set_attribute("/particle_emitter/min_emitt_interval", std::make_shared<configuration_attribute>(min_emitt_interval));
ui32 max_emitt_interval = node.node().attribute("max_emitt_interval").as_uint();
configuration::set_attribute("/particle_emitter/max_emitt_interval", std::make_shared<configuration_attribute>(max_emitt_interval));
pugi::xpath_node_set material_nodes = document.select_nodes("/particle_emitter/materials/material");
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
configuration::set_configuration("/particle_emitter/materials/material", material);
}
}
void particle_emitter_configuration::serialize_json(const std::string& filename)
{
Json::Value json;
bool result = configuration::open_json(json, filename);
assert(result);
i32 z_order = json.get("z_order", 0).asInt();
configuration::set_attribute("/particle_emitter/z_order", std::make_shared<configuration_attribute>(z_order));
ui32 num_particles = json.get("num_particles", 0).asUInt();
configuration::set_attribute("/particle_emitter/num_particles", std::make_shared<configuration_attribute>(num_particles));
ui32 duration = json.get("duration", 0).asUInt();
configuration::set_attribute("/particle_emitter/duration", std::make_shared<configuration_attribute>(duration));
f32 duration_randomness = json.get("duration_randomness", 0.f).asFloat();
configuration::set_attribute("/particle_emitter/duration_randomness", std::make_shared<configuration_attribute>(duration_randomness));
f32 velocity_sensitivity = json.get("velocity_sensitivity", 0.f).asFloat();
configuration::set_attribute("/particle_emitter/velocity_sensitivity", std::make_shared<configuration_attribute>(velocity_sensitivity));
f32 min_horizontal_velocity = json.get("min_horizontal_velocity", 0.f).asFloat();
configuration::set_attribute("/particle_emitter/min_horizontal_velocity", std::make_shared<configuration_attribute>(min_horizontal_velocity));
f32 max_horizontal_velocity = json.get("max_horizontal_velocity", 0.f).asFloat();
configuration::set_attribute("/particle_emitter/max_horizontal_velocity", std::make_shared<configuration_attribute>(max_horizontal_velocity));
f32 min_vertical_velocity = json.get("min_vertical_velocity", 0.f).asFloat();
configuration::set_attribute("/particle_emitter/min_vertical_velocity", std::make_shared<configuration_attribute>(min_vertical_velocity));
f32 max_vertical_velocity = json.get("max_vertical_velocity", 0.f).asFloat();
configuration::set_attribute("/particle_emitter/max_vertical_velocity", std::make_shared<configuration_attribute>(max_vertical_velocity));
f32 end_velocity = json.get("end_velocity", 0.f).asFloat();
configuration::set_attribute("/particle_emitter/end_velocity", std::make_shared<configuration_attribute>(end_velocity));
f32 gravity_x = json.get("gravity_x", 0.f).asFloat();
configuration::set_attribute("/particle_emitter/gravity_x", std::make_shared<configuration_attribute>(gravity_x));
f32 gravity_y = json.get("gravity_y", 0.f).asFloat();
configuration::set_attribute("/particle_emitter/gravity_y", std::make_shared<configuration_attribute>(gravity_y));
f32 gravity_z = json.get("gravity_z", 0.f).asFloat();
configuration::set_attribute("/particle_emitter/gravity_z", std::make_shared<configuration_attribute>(gravity_z));
ui8 source_color_r = json.get("source_color_r", 0).asUInt();
configuration::set_attribute("/particle_emitter/source_color_r", std::make_shared<configuration_attribute>(source_color_r));
ui8 source_color_g = json.get("source_color_g", 0).asUInt();
configuration::set_attribute("/particle_emitter/source_color_g", std::make_shared<configuration_attribute>(source_color_g));
ui8 source_color_b = json.get("source_color_b", 0).asUInt();
configuration::set_attribute("/particle_emitter/source_color_b", std::make_shared<configuration_attribute>(source_color_b));
ui8 source_color_a = json.get("source_color_a", 0).asUInt();
configuration::set_attribute("/particle_emitter/source_color_a", std::make_shared<configuration_attribute>(source_color_a));
ui8 destination_color_r = json.get("destination_color_r", 0).asUInt();
configuration::set_attribute("/particle_emitter/destination_color_r", std::make_shared<configuration_attribute>(destination_color_r));
ui8 destination_color_g = json.get("destination_color_g", 0).asUInt();
configuration::set_attribute("/particle_emitter/destination_color_g", std::make_shared<configuration_attribute>(destination_color_g));
ui8 destination_color_b = json.get("destination_color_b", 0).asUInt();
configuration::set_attribute("/particle_emitter/destination_color_b", std::make_shared<configuration_attribute>(destination_color_b));
ui8 destination_color_a = json.get("destination_color_a", 0).asUInt();
configuration::set_attribute("/particle_emitter/destination_color_a", std::make_shared<configuration_attribute>(destination_color_a));
f32 source_size_x = json.get("source_size_x", 0.f).asFloat();
configuration::set_attribute("/particle_emitter/source_size_x", std::make_shared<configuration_attribute>(source_size_x));
f32 source_size_y = json.get("source_size_y", 0.f).asFloat();
configuration::set_attribute("/particle_emitter/source_size_y", std::make_shared<configuration_attribute>(source_size_y));
f32 destination_size_x = json.get("destination_size_x", 0.f).asFloat();
configuration::set_attribute("/particle_emitter/destination_size_x", std::make_shared<configuration_attribute>(destination_size_x));
f32 destination_size_y = json.get("destination_size_y", 0.f).asFloat();
configuration::set_attribute("/particle_emitter/destination_size_y", std::make_shared<configuration_attribute>(destination_size_y));
ui32 min_emitt_interval = json.get("min_emitt_interval", 0).asUInt();
configuration::set_attribute("/particle_emitter/min_emitt_interval", std::make_shared<configuration_attribute>(min_emitt_interval));
ui32 max_emitt_interval = json.get("max_emitt_interval", 0).asUInt();
configuration::set_attribute("/particle_emitter/max_emitt_interval", std::make_shared<configuration_attribute>(max_emitt_interval));
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
configuration::set_configuration("/particle_emitter/materials/material", material);
}
}
}
