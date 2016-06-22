// autogenerated: do not add any changes
#include "ws_technique_configuration.h"
namespace gb
{
std::string ws_technique_configuration::get_guid(void) const
{
const auto& iterator = m_attributes.find("/ws_technique/guid");
assert(iterator != m_attributes.end());
std::string value; iterator->second->get(&value);
return value;
}
#if defined(__EDITOR__)
void ws_technique_configuration::set_guid(std::string guid)
{
configuration::set_attribute("/ws_technique/guid", std::make_shared<configuration_attribute>(guid));
}
#endif
bool ws_technique_configuration::get_is_depth_compare_mode_enabled(void) const
{
const auto& iterator = m_attributes.find("/ws_technique/is_depth_compare_mode_enabled");
assert(iterator != m_attributes.end());
bool value; iterator->second->get(&value);
return value;
}
#if defined(__EDITOR__)
void ws_technique_configuration::set_is_depth_compare_mode_enabled(bool is_depth_compare_mode_enabled)
{
configuration::set_attribute("/ws_technique/is_depth_compare_mode_enabled", std::make_shared<configuration_attribute>(is_depth_compare_mode_enabled));
}
#endif
ui32 ws_technique_configuration::get_num_passes(void) const
{
const auto& iterator = m_attributes.find("/ws_technique/num_passes");
assert(iterator != m_attributes.end());
ui32 value; iterator->second->get(&value);
return value;
}
#if defined(__EDITOR__)
void ws_technique_configuration::set_num_passes(ui32 num_passes)
{
configuration::set_attribute("/ws_technique/num_passes", std::make_shared<configuration_attribute>(num_passes));
}
#endif
ui32 ws_technique_configuration::get_index(void) const
{
const auto& iterator = m_attributes.find("/ws_technique/index");
assert(iterator != m_attributes.end());
ui32 value; iterator->second->get(&value);
return value;
}
#if defined(__EDITOR__)
void ws_technique_configuration::set_index(ui32 index)
{
configuration::set_attribute("/ws_technique/index", std::make_shared<configuration_attribute>(index));
}
#endif
ui32 ws_technique_configuration::get_screen_width(void) const
{
const auto& iterator = m_attributes.find("/ws_technique/screen_width");
assert(iterator != m_attributes.end());
ui32 value; iterator->second->get(&value);
return value;
}
#if defined(__EDITOR__)
void ws_technique_configuration::set_screen_width(ui32 screen_width)
{
configuration::set_attribute("/ws_technique/screen_width", std::make_shared<configuration_attribute>(screen_width));
}
#endif
ui32 ws_technique_configuration::get_screen_height(void) const
{
const auto& iterator = m_attributes.find("/ws_technique/screen_height");
assert(iterator != m_attributes.end());
ui32 value; iterator->second->get(&value);
return value;
}
#if defined(__EDITOR__)
void ws_technique_configuration::set_screen_height(ui32 screen_height)
{
configuration::set_attribute("/ws_technique/screen_height", std::make_shared<configuration_attribute>(screen_height));
}
#endif
f32 ws_technique_configuration::get_clear_color_r(void) const
{
const auto& iterator = m_attributes.find("/ws_technique/clear_color_r");
assert(iterator != m_attributes.end());
f32 value; iterator->second->get(&value);
return value;
}
#if defined(__EDITOR__)
void ws_technique_configuration::set_clear_color_r(f32 clear_color_r)
{
configuration::set_attribute("/ws_technique/clear_color_r", std::make_shared<configuration_attribute>(clear_color_r));
}
#endif
f32 ws_technique_configuration::get_clear_color_g(void) const
{
const auto& iterator = m_attributes.find("/ws_technique/clear_color_g");
assert(iterator != m_attributes.end());
f32 value; iterator->second->get(&value);
return value;
}
#if defined(__EDITOR__)
void ws_technique_configuration::set_clear_color_g(f32 clear_color_g)
{
configuration::set_attribute("/ws_technique/clear_color_g", std::make_shared<configuration_attribute>(clear_color_g));
}
#endif
f32 ws_technique_configuration::get_clear_color_b(void) const
{
const auto& iterator = m_attributes.find("/ws_technique/clear_color_b");
assert(iterator != m_attributes.end());
f32 value; iterator->second->get(&value);
return value;
}
#if defined(__EDITOR__)
void ws_technique_configuration::set_clear_color_b(f32 clear_color_b)
{
configuration::set_attribute("/ws_technique/clear_color_b", std::make_shared<configuration_attribute>(clear_color_b));
}
#endif
f32 ws_technique_configuration::get_clear_color_a(void) const
{
const auto& iterator = m_attributes.find("/ws_technique/clear_color_a");
assert(iterator != m_attributes.end());
f32 value; iterator->second->get(&value);
return value;
}
#if defined(__EDITOR__)
void ws_technique_configuration::set_clear_color_a(f32 clear_color_a)
{
configuration::set_attribute("/ws_technique/clear_color_a", std::make_shared<configuration_attribute>(clear_color_a));
}
#endif
void ws_technique_configuration::serialize(const std::string& filename)
{
pugi::xml_document document;
pugi::xml_parse_result result = configuration::open_xml(document, filename);
assert(result.status == pugi::status_ok);
pugi::xpath_node node;
node = document.select_single_node("/ws_technique");
std::string guid = node.node().attribute("guid").as_string();
configuration::set_attribute("/ws_technique/guid", std::make_shared<configuration_attribute>(guid));
bool is_depth_compare_mode_enabled = node.node().attribute("is_depth_compare_mode_enabled").as_bool();
configuration::set_attribute("/ws_technique/is_depth_compare_mode_enabled", std::make_shared<configuration_attribute>(is_depth_compare_mode_enabled));
ui32 num_passes = node.node().attribute("num_passes").as_uint();
configuration::set_attribute("/ws_technique/num_passes", std::make_shared<configuration_attribute>(num_passes));
ui32 index = node.node().attribute("index").as_uint();
configuration::set_attribute("/ws_technique/index", std::make_shared<configuration_attribute>(index));
ui32 screen_width = node.node().attribute("screen_width").as_uint();
configuration::set_attribute("/ws_technique/screen_width", std::make_shared<configuration_attribute>(screen_width));
ui32 screen_height = node.node().attribute("screen_height").as_uint();
configuration::set_attribute("/ws_technique/screen_height", std::make_shared<configuration_attribute>(screen_height));
f32 clear_color_r = node.node().attribute("clear_color_r").as_float();
configuration::set_attribute("/ws_technique/clear_color_r", std::make_shared<configuration_attribute>(clear_color_r));
f32 clear_color_g = node.node().attribute("clear_color_g").as_float();
configuration::set_attribute("/ws_technique/clear_color_g", std::make_shared<configuration_attribute>(clear_color_g));
f32 clear_color_b = node.node().attribute("clear_color_b").as_float();
configuration::set_attribute("/ws_technique/clear_color_b", std::make_shared<configuration_attribute>(clear_color_b));
f32 clear_color_a = node.node().attribute("clear_color_a").as_float();
configuration::set_attribute("/ws_technique/clear_color_a", std::make_shared<configuration_attribute>(clear_color_a));
}
void ws_technique_configuration::serialize_json(const std::string& filename)
{
Json::Value json;
bool result = configuration::open_json(json, filename);
assert(result);
std::string guid = json.get("guid", 0).asString();
configuration::set_attribute("/ws_technique/guid", std::make_shared<configuration_attribute>(guid));
bool is_depth_compare_mode_enabled = json.get("is_depth_compare_mode_enabled", 0).asBool();
configuration::set_attribute("/ws_technique/is_depth_compare_mode_enabled", std::make_shared<configuration_attribute>(is_depth_compare_mode_enabled));
ui32 num_passes = json.get("num_passes", 0).asUInt();
configuration::set_attribute("/ws_technique/num_passes", std::make_shared<configuration_attribute>(num_passes));
ui32 index = json.get("index", 0).asUInt();
configuration::set_attribute("/ws_technique/index", std::make_shared<configuration_attribute>(index));
ui32 screen_width = json.get("screen_width", 0).asUInt();
configuration::set_attribute("/ws_technique/screen_width", std::make_shared<configuration_attribute>(screen_width));
ui32 screen_height = json.get("screen_height", 0).asUInt();
configuration::set_attribute("/ws_technique/screen_height", std::make_shared<configuration_attribute>(screen_height));
f32 clear_color_r = json.get("clear_color_r", 0).asFloat();
configuration::set_attribute("/ws_technique/clear_color_r", std::make_shared<configuration_attribute>(clear_color_r));
f32 clear_color_g = json.get("clear_color_g", 0).asFloat();
configuration::set_attribute("/ws_technique/clear_color_g", std::make_shared<configuration_attribute>(clear_color_g));
f32 clear_color_b = json.get("clear_color_b", 0).asFloat();
configuration::set_attribute("/ws_technique/clear_color_b", std::make_shared<configuration_attribute>(clear_color_b));
f32 clear_color_a = json.get("clear_color_a", 0).asFloat();
configuration::set_attribute("/ws_technique/clear_color_a", std::make_shared<configuration_attribute>(clear_color_a));
}
#if defined(__EDITOR__)
void ws_technique_configuration::deserialize(const std::string& filename)
{
pugi::xml_document document;
pugi::xml_parse_result result = document.load("");
assert(result.status == pugi::status_ok);
pugi::xml_node node = document.append_child("ws_technique");
pugi::xml_node parent_node = node;
pugi::xml_attribute attribute;
attribute = node.append_attribute("guid");
std::string guid = ws_technique_configuration::get_guid();
attribute.set_value(guid.c_str());
attribute = node.append_attribute("is_depth_compare_mode_enabled");
bool is_depth_compare_mode_enabled = ws_technique_configuration::get_is_depth_compare_mode_enabled();
attribute.set_value(is_depth_compare_mode_enabled);
attribute = node.append_attribute("num_passes");
ui32 num_passes = ws_technique_configuration::get_num_passes();
attribute.set_value(num_passes);
attribute = node.append_attribute("index");
ui32 index = ws_technique_configuration::get_index();
attribute.set_value(index);
attribute = node.append_attribute("screen_width");
ui32 screen_width = ws_technique_configuration::get_screen_width();
attribute.set_value(screen_width);
attribute = node.append_attribute("screen_height");
ui32 screen_height = ws_technique_configuration::get_screen_height();
attribute.set_value(screen_height);
attribute = node.append_attribute("clear_color_r");
f32 clear_color_r = ws_technique_configuration::get_clear_color_r();
attribute.set_value(clear_color_r);
attribute = node.append_attribute("clear_color_g");
f32 clear_color_g = ws_technique_configuration::get_clear_color_g();
attribute.set_value(clear_color_g);
attribute = node.append_attribute("clear_color_b");
f32 clear_color_b = ws_technique_configuration::get_clear_color_b();
attribute.set_value(clear_color_b);
attribute = node.append_attribute("clear_color_a");
f32 clear_color_a = ws_technique_configuration::get_clear_color_a();
attribute.set_value(clear_color_a);
document.save_file(filename.c_str());
}
#endif
}
