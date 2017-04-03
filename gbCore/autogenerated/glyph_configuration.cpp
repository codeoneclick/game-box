// autogenerated: do not add any changes
#include "glyph_configuration.h"
namespace gb
{
i32 glyph_configuration::get_id(void) const
{
const auto& iterator = m_attributes.find("/glyph/id");
assert(iterator != m_attributes.end());
i32 value; iterator->second->get(&value);
return value;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void glyph_configuration::set_id(i32 id)
{
configuration::set_attribute("/glyph/id", std::make_shared<configuration_attribute>(id));
}
#endif
i32 glyph_configuration::get_x(void) const
{
const auto& iterator = m_attributes.find("/glyph/x");
assert(iterator != m_attributes.end());
i32 value; iterator->second->get(&value);
return value;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void glyph_configuration::set_x(i32 x)
{
configuration::set_attribute("/glyph/x", std::make_shared<configuration_attribute>(x));
}
#endif
i32 glyph_configuration::get_y(void) const
{
const auto& iterator = m_attributes.find("/glyph/y");
assert(iterator != m_attributes.end());
i32 value; iterator->second->get(&value);
return value;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void glyph_configuration::set_y(i32 y)
{
configuration::set_attribute("/glyph/y", std::make_shared<configuration_attribute>(y));
}
#endif
i32 glyph_configuration::get_width(void) const
{
const auto& iterator = m_attributes.find("/glyph/width");
assert(iterator != m_attributes.end());
i32 value; iterator->second->get(&value);
return value;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void glyph_configuration::set_width(i32 width)
{
configuration::set_attribute("/glyph/width", std::make_shared<configuration_attribute>(width));
}
#endif
i32 glyph_configuration::get_height(void) const
{
const auto& iterator = m_attributes.find("/glyph/height");
assert(iterator != m_attributes.end());
i32 value; iterator->second->get(&value);
return value;
}
#if defined(__IS_CONFIGURATION_MUTABLE__)
void glyph_configuration::set_height(i32 height)
{
configuration::set_attribute("/glyph/height", std::make_shared<configuration_attribute>(height));
}
#endif
void glyph_configuration::serialize_xml(pugi::xml_document& document, const std::string& path)
{
pugi::xpath_node node;
node = document.select_single_node((path + "/glyph").c_str());
i32 id = node.node().attribute("id").as_int();
configuration::set_attribute("/glyph/id", std::make_shared<configuration_attribute>(id));
i32 x = node.node().attribute("x").as_int();
configuration::set_attribute("/glyph/x", std::make_shared<configuration_attribute>(x));
i32 y = node.node().attribute("y").as_int();
configuration::set_attribute("/glyph/y", std::make_shared<configuration_attribute>(y));
i32 width = node.node().attribute("width").as_int();
configuration::set_attribute("/glyph/width", std::make_shared<configuration_attribute>(width));
i32 height = node.node().attribute("height").as_int();
configuration::set_attribute("/glyph/height", std::make_shared<configuration_attribute>(height));
}
void glyph_configuration::serialize_json(Json::Value& json)
{
i32 id = json.get("id", 0).asInt();
configuration::set_attribute("/glyph/id", std::make_shared<configuration_attribute>(id));
i32 x = json.get("x", 0).asInt();
configuration::set_attribute("/glyph/x", std::make_shared<configuration_attribute>(x));
i32 y = json.get("y", 0).asInt();
configuration::set_attribute("/glyph/y", std::make_shared<configuration_attribute>(y));
i32 width = json.get("width", 0).asInt();
configuration::set_attribute("/glyph/width", std::make_shared<configuration_attribute>(width));
i32 height = json.get("height", 0).asInt();
configuration::set_attribute("/glyph/height", std::make_shared<configuration_attribute>(height));
}
void glyph_configuration::serialize_xml(pugi::xml_document& document, pugi::xpath_node& node)
{
i32 id = node.node().attribute("id").as_int();
configuration::set_attribute("/glyph/id", std::make_shared<configuration_attribute>(id));
i32 x = node.node().attribute("x").as_int();
configuration::set_attribute("/glyph/x", std::make_shared<configuration_attribute>(x));
i32 y = node.node().attribute("y").as_int();
configuration::set_attribute("/glyph/y", std::make_shared<configuration_attribute>(y));
i32 width = node.node().attribute("width").as_int();
configuration::set_attribute("/glyph/width", std::make_shared<configuration_attribute>(width));
i32 height = node.node().attribute("height").as_int();
configuration::set_attribute("/glyph/height", std::make_shared<configuration_attribute>(height));
}
}
