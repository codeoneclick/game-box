// autogenerated: do not add any changes
#include "material_configuration.h"
namespace gb
{
std::string material_configuration::get_technique_name(void) const
{
const auto& iterator = m_attributes.find("/material/technique_name");
assert(iterator != m_attributes.end());
std::string value; iterator->second->get(&value);
return value;
}
#if defined(__EDITOR__)
void material_configuration::set_technique_name(std::string technique_name)
{
configuration::set_attribute("/material/technique_name", std::make_shared<configuration_attribute>(technique_name));
}
#endif
i32 material_configuration::get_technique_pass(void) const
{
const auto& iterator = m_attributes.find("/material/technique_pass");
assert(iterator != m_attributes.end());
i32 value; iterator->second->get(&value);
return value;
}
#if defined(__EDITOR__)
void material_configuration::set_technique_pass(i32 technique_pass)
{
configuration::set_attribute("/material/technique_pass", std::make_shared<configuration_attribute>(technique_pass));
}
#endif
bool material_configuration::get_depth_test(void) const
{
const auto& iterator = m_attributes.find("/material/is_depth_test");
assert(iterator != m_attributes.end());
bool value; iterator->second->get(&value);
return value;
}
#if defined(__EDITOR__)
void material_configuration::set_depth_test(bool is_depth_test)
{
configuration::set_attribute("/material/is_depth_test", std::make_shared<configuration_attribute>(is_depth_test));
}
#endif
bool material_configuration::get_depth_mask(void) const
{
const auto& iterator = m_attributes.find("/material/is_depth_mask");
assert(iterator != m_attributes.end());
bool value; iterator->second->get(&value);
return value;
}
#if defined(__EDITOR__)
void material_configuration::set_depth_mask(bool is_depth_mask)
{
configuration::set_attribute("/material/is_depth_mask", std::make_shared<configuration_attribute>(is_depth_mask));
}
#endif
bool material_configuration::get_culling(void) const
{
const auto& iterator = m_attributes.find("/material/is_cull_face");
assert(iterator != m_attributes.end());
bool value; iterator->second->get(&value);
return value;
}
#if defined(__EDITOR__)
void material_configuration::set_culling(bool is_cull_face)
{
configuration::set_attribute("/material/is_cull_face", std::make_shared<configuration_attribute>(is_cull_face));
}
#endif
GLenum material_configuration::get_culling_mode(void) const
{
const auto& iterator = m_attributes.find("/material/cull_face_mode");
assert(iterator != m_attributes.end());
GLenum value; iterator->second->get(&value);
return value;
}
#if defined(__EDITOR__)
void material_configuration::set_culling_mode(GLenum cull_face_mode)
{
configuration::set_attribute("/material/cull_face_mode", std::make_shared<configuration_attribute>(cull_face_mode));
}
#endif
bool material_configuration::get_blending(void) const
{
const auto& iterator = m_attributes.find("/material/is_blending");
assert(iterator != m_attributes.end());
bool value; iterator->second->get(&value);
return value;
}
#if defined(__EDITOR__)
void material_configuration::set_blending(bool is_blending)
{
configuration::set_attribute("/material/is_blending", std::make_shared<configuration_attribute>(is_blending));
}
#endif
GLenum material_configuration::get_blending_function_source(void) const
{
const auto& iterator = m_attributes.find("/material/blending_function_source");
assert(iterator != m_attributes.end());
GLenum value; iterator->second->get(&value);
return value;
}
#if defined(__EDITOR__)
void material_configuration::set_blending_function_source(GLenum blending_function_source)
{
configuration::set_attribute("/material/blending_function_source", std::make_shared<configuration_attribute>(blending_function_source));
}
#endif
GLenum material_configuration::get_blending_function_destination(void) const
{
const auto& iterator = m_attributes.find("/material/blending_function_destination");
assert(iterator != m_attributes.end());
GLenum value; iterator->second->get(&value);
return value;
}
#if defined(__EDITOR__)
void material_configuration::set_blending_function_destination(GLenum blending_function_destination)
{
configuration::set_attribute("/material/blending_function_destination", std::make_shared<configuration_attribute>(blending_function_destination));
}
#endif
GLenum material_configuration::get_blending_equation(void) const
{
const auto& iterator = m_attributes.find("/material/blending_equation");
assert(iterator != m_attributes.end());
GLenum value; iterator->second->get(&value);
return value;
}
#if defined(__EDITOR__)
void material_configuration::set_blending_equation(GLenum blending_equation)
{
configuration::set_attribute("/material/blending_equation", std::make_shared<configuration_attribute>(blending_equation));
}
#endif
bool material_configuration::get_stencil_test(void) const
{
const auto& iterator = m_attributes.find("/material/is_stencil_test");
assert(iterator != m_attributes.end());
bool value; iterator->second->get(&value);
return value;
}
#if defined(__EDITOR__)
void material_configuration::set_stencil_test(bool is_stencil_test)
{
configuration::set_attribute("/material/is_stencil_test", std::make_shared<configuration_attribute>(is_stencil_test));
}
#endif
GLenum material_configuration::get_stencil_function(void) const
{
const auto& iterator = m_attributes.find("/material/stencil_function");
assert(iterator != m_attributes.end());
GLenum value; iterator->second->get(&value);
return value;
}
#if defined(__EDITOR__)
void material_configuration::set_stencil_function(GLenum stencil_function)
{
configuration::set_attribute("/material/stencil_function", std::make_shared<configuration_attribute>(stencil_function));
}
#endif
i32 material_configuration::get_stencil_function_parameter_1(void) const
{
const auto& iterator = m_attributes.find("/material/stencil_function_parameter_1");
assert(iterator != m_attributes.end());
i32 value; iterator->second->get(&value);
return value;
}
#if defined(__EDITOR__)
void material_configuration::set_stencil_function_parameter_1(i32 stencil_function_parameter_1)
{
configuration::set_attribute("/material/stencil_function_parameter_1", std::make_shared<configuration_attribute>(stencil_function_parameter_1));
}
#endif
i32 material_configuration::get_stencil_function_parameter_2(void) const
{
const auto& iterator = m_attributes.find("/material/stencil_function_parameter_2");
assert(iterator != m_attributes.end());
i32 value; iterator->second->get(&value);
return value;
}
#if defined(__EDITOR__)
void material_configuration::set_stencil_function_parameter_2(i32 stencil_function_parameter_2)
{
configuration::set_attribute("/material/stencil_function_parameter_2", std::make_shared<configuration_attribute>(stencil_function_parameter_2));
}
#endif
i32 material_configuration::get_stencil_mask_parameter(void) const
{
const auto& iterator = m_attributes.find("/material/stencil_mask_parameter");
assert(iterator != m_attributes.end());
i32 value; iterator->second->get(&value);
return value;
}
#if defined(__EDITOR__)
void material_configuration::set_stencil_mask_parameter(i32 stencil_mask_parameter)
{
configuration::set_attribute("/material/stencil_mask_parameter", std::make_shared<configuration_attribute>(stencil_mask_parameter));
}
#endif
bool material_configuration::get_debugging(void) const
{
const auto& iterator = m_attributes.find("/material/is_debugging");
assert(iterator != m_attributes.end());
bool value; iterator->second->get(&value);
return value;
}
#if defined(__EDITOR__)
void material_configuration::set_debugging(bool is_debugging)
{
configuration::set_attribute("/material/is_debugging", std::make_shared<configuration_attribute>(is_debugging));
}
#endif
std::shared_ptr<gb::shader_configuration> material_configuration::get_shader_configuration(void) const
{
const auto& iterator = m_configurations.find("/material/shader");
if(iterator == m_configurations.end())
{
return nullptr;
}
assert(iterator != m_configurations.end());
assert(iterator->second.size() != 0);
return std::static_pointer_cast<gb::shader_configuration>(iterator->second.at(0));
}
#if defined(__EDITOR__)
void material_configuration::set_shader_configuration(const std::shared_ptr<gb::shader_configuration>& shader)
{
configuration::set_configuration("/material/shader", shader, 0);
}
#endif
std::vector<std::shared_ptr<configuration>> material_configuration::get_textures_configurations(void) const
{
const auto& iterator = m_configurations.find("/material/textures/texture");
if(iterator == m_configurations.end())
{
return std::vector<std::shared_ptr<configuration>>();
}
assert(iterator != m_configurations.end());
return iterator->second;
}
#if defined(__EDITOR__)
void material_configuration::add_textures_configurations(const std::shared_ptr<gb::texture_configuration>& texture)
{
configuration::set_configuration("/material/textures/texture", texture);
}
#endif
#if defined(__EDITOR__)
void material_configuration::set_textures_configurations(const std::shared_ptr<gb::texture_configuration>& texture, i32 index)
{
configuration::set_configuration("/material/textures/texture", texture, index);
}
#endif
void material_configuration::serialize(const std::string& filename)
{
pugi::xml_document document;
pugi::xml_parse_result result = configuration::open_xml_document(document, filename);
assert(result.status == pugi::status_ok);
pugi::xpath_node node;
node = document.select_single_node("/material");
std::string technique_name = node.node().attribute("technique_name").as_string();
configuration::set_attribute("/material/technique_name", std::make_shared<configuration_attribute>(technique_name));
i32 technique_pass = node.node().attribute("technique_pass").as_int();
configuration::set_attribute("/material/technique_pass", std::make_shared<configuration_attribute>(technique_pass));
bool is_depth_test = node.node().attribute("is_depth_test").as_bool();
configuration::set_attribute("/material/is_depth_test", std::make_shared<configuration_attribute>(is_depth_test));
bool is_depth_mask = node.node().attribute("is_depth_mask").as_bool();
configuration::set_attribute("/material/is_depth_mask", std::make_shared<configuration_attribute>(is_depth_mask));
bool is_cull_face = node.node().attribute("is_cull_face").as_bool();
configuration::set_attribute("/material/is_cull_face", std::make_shared<configuration_attribute>(is_cull_face));
std::string cull_face_mode = node.node().attribute("cull_face_mode").as_string();
assert(g_string_to_glenum.find(cull_face_mode) != g_string_to_glenum.end());
GLenum cull_face_mode_enum = g_string_to_glenum.find(cull_face_mode)->second;
configuration::set_attribute("/material/cull_face_mode", std::make_shared<configuration_attribute>(cull_face_mode_enum));
bool is_blending = node.node().attribute("is_blending").as_bool();
configuration::set_attribute("/material/is_blending", std::make_shared<configuration_attribute>(is_blending));
std::string blending_function_source = node.node().attribute("blending_function_source").as_string();
assert(g_string_to_glenum.find(blending_function_source) != g_string_to_glenum.end());
GLenum blending_function_source_enum = g_string_to_glenum.find(blending_function_source)->second;
configuration::set_attribute("/material/blending_function_source", std::make_shared<configuration_attribute>(blending_function_source_enum));
std::string blending_function_destination = node.node().attribute("blending_function_destination").as_string();
assert(g_string_to_glenum.find(blending_function_destination) != g_string_to_glenum.end());
GLenum blending_function_destination_enum = g_string_to_glenum.find(blending_function_destination)->second;
configuration::set_attribute("/material/blending_function_destination", std::make_shared<configuration_attribute>(blending_function_destination_enum));
std::string blending_equation = node.node().attribute("blending_equation").as_string();
assert(g_string_to_glenum.find(blending_equation) != g_string_to_glenum.end());
GLenum blending_equation_enum = g_string_to_glenum.find(blending_equation)->second;
configuration::set_attribute("/material/blending_equation", std::make_shared<configuration_attribute>(blending_equation_enum));
bool is_stencil_test = node.node().attribute("is_stencil_test").as_bool();
configuration::set_attribute("/material/is_stencil_test", std::make_shared<configuration_attribute>(is_stencil_test));
std::string stencil_function = node.node().attribute("stencil_function").as_string();
assert(g_string_to_glenum.find(stencil_function) != g_string_to_glenum.end());
GLenum stencil_function_enum = g_string_to_glenum.find(stencil_function)->second;
configuration::set_attribute("/material/stencil_function", std::make_shared<configuration_attribute>(stencil_function_enum));
i32 stencil_function_parameter_1 = node.node().attribute("stencil_function_parameter_1").as_int();
configuration::set_attribute("/material/stencil_function_parameter_1", std::make_shared<configuration_attribute>(stencil_function_parameter_1));
i32 stencil_function_parameter_2 = node.node().attribute("stencil_function_parameter_2").as_int();
configuration::set_attribute("/material/stencil_function_parameter_2", std::make_shared<configuration_attribute>(stencil_function_parameter_2));
i32 stencil_mask_parameter = node.node().attribute("stencil_mask_parameter").as_int();
configuration::set_attribute("/material/stencil_mask_parameter", std::make_shared<configuration_attribute>(stencil_mask_parameter));
bool is_debugging = node.node().attribute("is_debugging").as_bool();
configuration::set_attribute("/material/is_debugging", std::make_shared<configuration_attribute>(is_debugging));
std::shared_ptr<gb::shader_configuration> shader = std::make_shared<gb::shader_configuration>();
shader->serialize(document, "/material");
configuration::set_configuration("/material/shader", shader);
pugi::xpath_node_set texture_nodes = document.select_nodes("/material/textures/texture");
for (pugi::xpath_node_set::const_iterator iterator = texture_nodes.begin(); iterator != texture_nodes.end(); ++iterator)
{
std::shared_ptr<gb::texture_configuration> texture = std::make_shared<gb::texture_configuration>();
pugi::xpath_node node = (*iterator);
texture->serialize(document, node);
configuration::set_configuration("/material/textures/texture", texture);
}
}
#if defined(__EDITOR__)
void material_configuration::deserialize(const std::string& filename)
{
pugi::xml_document document;
pugi::xml_parse_result result = document.load("");
assert(result.status == pugi::status_ok);
pugi::xml_node node = document.append_child("material");
pugi::xml_node parent_node = node;
pugi::xml_attribute attribute;
attribute = node.append_attribute("technique_name");
std::string technique_name = material_configuration::get_technique_name();
attribute.set_value(technique_name.c_str());
attribute = node.append_attribute("technique_pass");
i32 technique_pass = material_configuration::get_technique_pass();
attribute.set_value(technique_pass);
attribute = node.append_attribute("is_depth_test");
bool is_depth_test = material_configuration::get_depth_test();
attribute.set_value(is_depth_test);
attribute = node.append_attribute("is_depth_mask");
bool is_depth_mask = material_configuration::get_depth_mask();
attribute.set_value(is_depth_mask);
attribute = node.append_attribute("is_cull_face");
bool is_cull_face = material_configuration::get_culling();
attribute.set_value(is_cull_face);
attribute = node.append_attribute("cull_face_mode");
GLenum cull_face_mode_enum = material_configuration::get_culling_mode();
assert(g_glenum_to_string.find(cull_face_mode_enum) != g_glenum_to_string.end());
std::string cull_face_mode = g_glenum_to_string.find(cull_face_mode_enum)->second;
attribute.set_value(cull_face_mode.c_str());
attribute = node.append_attribute("is_blending");
bool is_blending = material_configuration::get_blending();
attribute.set_value(is_blending);
attribute = node.append_attribute("blending_function_source");
GLenum blending_function_source_enum = material_configuration::get_blending_function_source();
assert(g_glenum_to_string.find(blending_function_source_enum) != g_glenum_to_string.end());
std::string blending_function_source = g_glenum_to_string.find(blending_function_source_enum)->second;
attribute.set_value(blending_function_source.c_str());
attribute = node.append_attribute("blending_function_destination");
GLenum blending_function_destination_enum = material_configuration::get_blending_function_destination();
assert(g_glenum_to_string.find(blending_function_destination_enum) != g_glenum_to_string.end());
std::string blending_function_destination = g_glenum_to_string.find(blending_function_destination_enum)->second;
attribute.set_value(blending_function_destination.c_str());
attribute = node.append_attribute("blending_equation");
GLenum blending_equation_enum = material_configuration::get_blending_equation();
assert(g_glenum_to_string.find(blending_equation_enum) != g_glenum_to_string.end());
std::string blending_equation = g_glenum_to_string.find(blending_equation_enum)->second;
attribute.set_value(blending_equation.c_str());
attribute = node.append_attribute("is_stencil_test");
bool is_stencil_test = material_configuration::get_stencil_test();
attribute.set_value(is_stencil_test);
attribute = node.append_attribute("stencil_function");
GLenum stencil_function_enum = material_configuration::get_stencil_function();
assert(g_glenum_to_string.find(stencil_function_enum) != g_glenum_to_string.end());
std::string stencil_function = g_glenum_to_string.find(stencil_function_enum)->second;
attribute.set_value(stencil_function.c_str());
attribute = node.append_attribute("stencil_function_parameter_1");
i32 stencil_function_parameter_1 = material_configuration::get_stencil_function_parameter_1();
attribute.set_value(stencil_function_parameter_1);
attribute = node.append_attribute("stencil_function_parameter_2");
i32 stencil_function_parameter_2 = material_configuration::get_stencil_function_parameter_2();
attribute.set_value(stencil_function_parameter_2);
attribute = node.append_attribute("stencil_mask_parameter");
i32 stencil_mask_parameter = material_configuration::get_stencil_mask_parameter();
attribute.set_value(stencil_mask_parameter);
attribute = node.append_attribute("is_debugging");
bool is_debugging = material_configuration::get_debugging();
attribute.set_value(is_debugging);
node = parent_node.append_child("shader");
material_configuration::get_shader_configuration()->deserialize(node);
node = parent_node.append_child("textures");
for(const auto& iterator : material_configuration::get_textures_configurations())
{
std::shared_ptr<gb::texture_configuration> configuration = std::static_pointer_cast<gb::texture_configuration>(iterator);
pugi::xml_node child_node = node.append_child("texture");
configuration->deserialize(child_node);
}
document.save_file(filename.c_str());
}
#endif
}
