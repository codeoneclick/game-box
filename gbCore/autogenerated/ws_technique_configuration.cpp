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
ui32 ws_technique_configuration::get_order(void) const
{
const auto& iterator = m_attributes.find("/ws_technique/order");
assert(iterator != m_attributes.end());
ui32 value; iterator->second->get(&value);
return value;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void ws_technique_configuration::set_order(ui32 order)
{
configuration::set_attribute("/ws_technique/order", std::make_shared<configuration_attribute>(order));
}
#endif
ui32 ws_technique_configuration::get_frame_width(void) const
{
const auto& iterator = m_attributes.find("/ws_technique/frame_width");
assert(iterator != m_attributes.end());
ui32 value; iterator->second->get(&value);
return value;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void ws_technique_configuration::set_frame_width(ui32 frame_width)
{
configuration::set_attribute("/ws_technique/frame_width", std::make_shared<configuration_attribute>(frame_width));
}
#endif
ui32 ws_technique_configuration::get_frame_height(void) const
{
const auto& iterator = m_attributes.find("/ws_technique/frame_height");
assert(iterator != m_attributes.end());
ui32 value; iterator->second->get(&value);
return value;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void ws_technique_configuration::set_frame_height(ui32 frame_height)
{
configuration::set_attribute("/ws_technique/frame_height", std::make_shared<configuration_attribute>(frame_height));
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
ui32 order = node.node().attribute("order").as_uint();
configuration::set_attribute("/ws_technique/order", std::make_shared<configuration_attribute>(order));
ui32 frame_width = node.node().attribute("frame_width").as_uint();
configuration::set_attribute("/ws_technique/frame_width", std::make_shared<configuration_attribute>(frame_width));
ui32 frame_height = node.node().attribute("frame_height").as_uint();
configuration::set_attribute("/ws_technique/frame_height", std::make_shared<configuration_attribute>(frame_height));
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
ui32 order = json.get("order", 0).asUInt();
configuration::set_attribute("/ws_technique/order", std::make_shared<configuration_attribute>(order));
ui32 frame_width = json.get("frame_width", 0).asUInt();
configuration::set_attribute("/ws_technique/frame_width", std::make_shared<configuration_attribute>(frame_width));
ui32 frame_height = json.get("frame_height", 0).asUInt();
configuration::set_attribute("/ws_technique/frame_height", std::make_shared<configuration_attribute>(frame_height));
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
