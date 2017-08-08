// autogenerated: do not add any changes
#include "quest_dependency_configuration.h"
namespace gb
{
i32 quest_dependency_configuration::get_quest_id(void) const
{
const auto& iterator = m_attributes.find("/quest_dependency/quest_id");
assert(iterator != m_attributes.end());
i32 value; iterator->second->get(&value);
return value;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void quest_dependency_configuration::set_quest_id(i32 quest_id)
{
configuration::set_attribute("/quest_dependency/quest_id", std::make_shared<configuration_attribute>(quest_id));
}
#endif
void quest_dependency_configuration::serialize_xml(pugi::xml_document& document, const std::string& path)
{
pugi::xpath_node node;
node = document.select_single_node((path + "/quest_dependency").c_str());
i32 quest_id = node.node().attribute("quest_id").as_int();
configuration::set_attribute("/quest_dependency/quest_id", std::make_shared<configuration_attribute>(quest_id));
}
void quest_dependency_configuration::serialize_json(Json::Value& json)
{
i32 quest_id = json.get("quest_id", 0).asInt();
configuration::set_attribute("/quest_dependency/quest_id", std::make_shared<configuration_attribute>(quest_id));
}
void quest_dependency_configuration::serialize_xml(pugi::xml_document& document, pugi::xpath_node& node)
{
i32 quest_id = node.node().attribute("quest_id").as_int();
configuration::set_attribute("/quest_dependency/quest_id", std::make_shared<configuration_attribute>(quest_id));
}
}