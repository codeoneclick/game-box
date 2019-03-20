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
#if defined(__IS_CONFIGURATION_MUTABLE__)
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
#if defined(__IS_CONFIGURATION_MUTABLE__)
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
#if defined(__IS_CONFIGURATION_MUTABLE__)
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
#if defined(__IS_CONFIGURATION_MUTABLE__)
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
#if defined(__IS_CONFIGURATION_MUTABLE__)
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
#if defined(__IS_CONFIGURATION_MUTABLE__)
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
#if defined(__IS_CONFIGURATION_MUTABLE__)
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
#if defined(__IS_CONFIGURATION_MUTABLE__)
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
#if defined(__IS_CONFIGURATION_MUTABLE__)
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
#if defined(__IS_CONFIGURATION_MUTABLE__)
void ws_technique_configuration::set_clear_color_a(f32 clear_color_a)
{
configuration::set_attribute("/ws_technique/clear_color_a", std::make_shared<configuration_attribute>(clear_color_a));
}
#endif
std::vector<std::shared_ptr<configuration>> ws_technique_configuration::get_attachments_configurations(void) const
{
const auto& iterator = m_configurations.find("/ws_technique/attachments/attachment");
if(iterator == m_configurations.end())
{
return std::vector<std::shared_ptr<configuration>>();
}
assert(iterator != m_configurations.end());
return iterator->second;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void ws_technique_configuration::add_attachments_configurations(const std::shared_ptr<gb::attachment_configuration>& attachment)
{
configuration::set_configuration("/ws_technique/attachments/attachment", attachment);
}
#endif
#if defined(__IS_CONFIGURATION_MUTABLE__)
void ws_technique_configuration::set_attachments_configurations(const std::shared_ptr<gb::attachment_configuration>& attachment, i32 index)
{
configuration::set_configuration("/ws_technique/attachments/attachment", attachment, index);
}
#endif
void ws_technique_configuration::serialize_xml(const std::string& filename)
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
pugi::xpath_node_set attachment_nodes = document.select_nodes("/ws_technique/attachments/attachment");
for (pugi::xpath_node_set::const_iterator iterator = attachment_nodes.begin(); iterator != attachment_nodes.end(); ++iterator)
{
std::shared_ptr<gb::attachment_configuration> attachment = std::make_shared<gb::attachment_configuration>();
pugi::xpath_node node = (*iterator);
attachment->serialize_xml(document, node);
configuration::set_configuration("/ws_technique/attachments/attachment", attachment);
}
}
void ws_technique_configuration::serialize_json(const std::string& filename)
{
Json::Value json;
bool result = configuration::open_json(json, filename);
assert(result);
std::string guid = json.get("guid", "unknown").asString();
configuration::set_attribute("/ws_technique/guid", std::make_shared<configuration_attribute>(guid));
bool is_depth_compare_mode_enabled = json.get("is_depth_compare_mode_enabled", false).asBool();
configuration::set_attribute("/ws_technique/is_depth_compare_mode_enabled", std::make_shared<configuration_attribute>(is_depth_compare_mode_enabled));
ui32 num_passes = json.get("num_passes", 0).asUInt();
configuration::set_attribute("/ws_technique/num_passes", std::make_shared<configuration_attribute>(num_passes));
ui32 index = json.get("index", 0).asUInt();
configuration::set_attribute("/ws_technique/index", std::make_shared<configuration_attribute>(index));
ui32 screen_width = json.get("screen_width", 0).asUInt();
configuration::set_attribute("/ws_technique/screen_width", std::make_shared<configuration_attribute>(screen_width));
ui32 screen_height = json.get("screen_height", 0).asUInt();
configuration::set_attribute("/ws_technique/screen_height", std::make_shared<configuration_attribute>(screen_height));
f32 clear_color_r = json.get("clear_color_r", 0.f).asFloat();
configuration::set_attribute("/ws_technique/clear_color_r", std::make_shared<configuration_attribute>(clear_color_r));
f32 clear_color_g = json.get("clear_color_g", 0.f).asFloat();
configuration::set_attribute("/ws_technique/clear_color_g", std::make_shared<configuration_attribute>(clear_color_g));
f32 clear_color_b = json.get("clear_color_b", 0.f).asFloat();
configuration::set_attribute("/ws_technique/clear_color_b", std::make_shared<configuration_attribute>(clear_color_b));
f32 clear_color_a = json.get("clear_color_a", 0.f).asFloat();
configuration::set_attribute("/ws_technique/clear_color_a", std::make_shared<configuration_attribute>(clear_color_a));
Json::Value attachments_json_array = json["attachments"];
for (Json::ValueIterator iterator = attachments_json_array.begin(); iterator != attachments_json_array.end(); ++iterator)
{
std::shared_ptr<gb::attachment_configuration> attachment = std::make_shared<gb::attachment_configuration>();
Json::Value json_value = (*iterator);
attachment->serialize_json(json_value);
configuration::set_configuration("/ws_technique/attachments/attachment", attachment);
}
}
}
