#!/usr/bin/python
import sys
import os
import re
from xml.etree import ElementTree
from re import sub, split, UNICODE
from os import walk, listdir
from os.path import isfile, join, basename
from sets import Set

kConfiguratiomPath = "configurations/"

kConfigurationAutogeneratedSourcesPath = "autogenerated/"

def xcode_print(string):
	print string
	sys.stdout.flush()

def get_configuration_samples(directory, extension):
    filepathes = [] 

    for root, directories, files in os.walk(directory):
        for filename in files:

            filepath = os.path.join(root, filename)
            if filename.endswith(extension) and "sample" in filename:
            	filepathes.append(filepath)

    return filepathes

def get_attribute_type_converter_json(attribute_type):
 	if attribute_type == 'bool':
 		return 'asBool()'
 	elif attribute_type == 'i64' or attribute_type == 'i32' or attribute_type == 'i16' or attribute_type == 'i8':
 		return 'asInt()'
 	elif attribute_type == 'ui64' or attribute_type == 'ui32' or attribute_type == 'ui16' or attribute_type == 'ui8':
 		return 'asUInt()'
 	elif attribute_type == 'f32':
 		return 'asFloat()'
 	elif attribute_type == 'std::string':
 		return 'asString()'
 	raise Exception('unknown attribute type')

def get_attribute_type_converter_xml(attribute_type):
 	if attribute_type == 'bool':
 		return 'as_bool()'
 	elif attribute_type == 'i64' or attribute_type == 'i32' or attribute_type == 'i16' or attribute_type == 'i8':
 		return 'as_int()'
 	elif attribute_type == 'ui64' or attribute_type == 'ui32' or attribute_type == 'ui16' or attribute_type == 'ui8':
 		return 'as_uint()'
 	elif attribute_type == 'f32':
 		return 'as_float()'
 	elif attribute_type == 'std::string':
 		return 'as_string()'
 	raise Exception('unknown attribute type')

def get_attribute_type_default_value_json(attribute_type):
 	if attribute_type == 'bool':
 		return 'false'
 	elif attribute_type == 'i64' or attribute_type == 'i32' or attribute_type == 'i16' or attribute_type == 'i8':
 		return '0'
 	elif attribute_type == 'ui64' or attribute_type == 'ui32' or attribute_type == 'ui16' or attribute_type == 'ui8':
 		return '0'
 	elif attribute_type == 'f32':
 		return '0.f'
 	elif attribute_type == 'std::string':
 		return '\"unknown\"'
 	raise Exception('unknown attribute type')

def write_attributes_serializer_xml(source_cpp_file, attributes):
	for attribute in attributes:

		if attribute.get('type') == 'GLenum':

			source_cpp_file.write('std::string ' + attribute.get("name") + ' = node.node().attribute("'+ attribute.get("name") + '").' + get_attribute_type_converter_xml('std::string') +';\n')
			source_cpp_file.write('assert(g_string_to_glenum.find('+ attribute.get("name") +') != g_string_to_glenum.end());\n')
			source_cpp_file.write('GLenum '+ attribute.get("name") + '_enum' + ' = g_string_to_glenum.find('+ attribute.get("name") +')->second;\n')
			source_cpp_file.write('configuration::set_attribute("' + attribute.get("path") + '/' + attribute.get("name") + '", std::make_shared<configuration_attribute>(' + attribute.get("name") + '_enum));\n')

		else:

			source_cpp_file.write(attribute.get('type') + ' ' + attribute.get("name") + ' = node.node().attribute("'+ attribute.get("name") + '").' + get_attribute_type_converter_xml(attribute.get('type')) +';\n')
			source_cpp_file.write('configuration::set_attribute("' + attribute.get("path") + '/' + attribute.get("name") + '", std::make_shared<configuration_attribute>(' + attribute.get("name") + '));\n')

def write_attributes_serializer_json(source_cpp_file, attributes):
	for attribute in attributes:

		if attribute.get('type') == 'GLenum':

			source_cpp_file.write('std::string ' + attribute.get("name") + ' = json.get("'+ attribute.get("name") +'", "unknown").' + get_attribute_type_converter_json('std::string') +';\n')
			source_cpp_file.write('assert(g_string_to_glenum.find('+ attribute.get("name") +') != g_string_to_glenum.end());\n')
			source_cpp_file.write('GLenum '+ attribute.get("name") + '_enum' + ' = g_string_to_glenum.find('+ attribute.get("name") +')->second;\n')
			source_cpp_file.write('configuration::set_attribute("' + attribute.get("path") + '/' + attribute.get("name") + '", std::make_shared<configuration_attribute>(' + attribute.get("name") + '_enum));\n')

		else:

			source_cpp_file.write(attribute.get('type') + ' ' + attribute.get("name") + ' = json.get("'+ attribute.get("name") + '", ' + get_attribute_type_default_value_json(attribute.get('type'))+ ').' + get_attribute_type_converter_json(attribute.get('type')) +';\n')
			source_cpp_file.write('configuration::set_attribute("' + attribute.get("path") + '/' + attribute.get("name") + '", std::make_shared<configuration_attribute>(' + attribute.get("name") + '));\n')

def write_attributes_deserializer_xml(source_cpp_file, attributes, class_name):
 	for attribute in attributes:

 		source_cpp_file.write('attribute = node.append_attribute("' + attribute.get('name') + '");\n')

		if attribute.get('type') == 'GLenum':

			source_cpp_file.write('GLenum ' + attribute.get("name") + '_enum = ' + class_name + '::get_' + attribute.get('property') +'();\n')
			source_cpp_file.write('assert(g_glenum_to_string.find('+ attribute.get("name") +'_enum) != g_glenum_to_string.end());\n')
			source_cpp_file.write('std::string '+ attribute.get("name") + ' = g_glenum_to_string.find('+ attribute.get("name") +'_enum)->second;\n')

		else:

			source_cpp_file.write(attribute.get('type') + ' ' + attribute.get("name") + ' = ' + class_name + '::get_' + attribute.get('property') +'();\n')
		
		if attribute.get('type') == 'GLenum' or attribute.get('type') == 'std::string':
			source_cpp_file.write('attribute.set_value(' + attribute.get("name") + '.c_str());\n')
		else:
			source_cpp_file.write('attribute.set_value(' + attribute.get("name") + ');\n')

def write_external_filename_serializer(source_cpp_file, relationship, path):
	source_cpp_file.write('std::string external_filename =' + path +';\n')
	source_cpp_file.write('if(external_filename.find(".xml") != std::string::npos)\n')
	source_cpp_file.write('{\n')
	source_cpp_file.write(relationship.get('name') + '->serialize_xml(external_filename);\n')
	source_cpp_file.write('}\n')
	source_cpp_file.write('else if(external_filename.find(".json") != std::string::npos)\n')
	source_cpp_file.write('{\n')
	source_cpp_file.write(relationship.get('name') + '->serialize_json(external_filename);\n')
	source_cpp_file.write('}\n')
	source_cpp_file.write('else\n')
	source_cpp_file.write('{\n')
	source_cpp_file.write('assert(false);\n')
	source_cpp_file.write('}\n')

def write_relationships_serializer_xml(source_cpp_file, relationships):
	for relationship in relationships:

		if relationship.get("is_to_many") == '0':

			source_cpp_file.write('std::shared_ptr<gb::' + relationship.get('type') + '> ' + relationship.get('name') + ' = std::make_shared<gb::' + relationship.get('type') + '>();\n')

			if relationship.get("is_external") == '0':

				source_cpp_file.write(relationship.get('name') + '->serialize_xml(document, "' + relationship.get('path') + '");\n')

			else:

				source_cpp_file.write('pugi::xpath_node ' + relationship.get('name') + '_node' + ' = document.select_single_node("' + relationship.get('path') + '/' + relationship.get('name') + '");\n')
				write_external_filename_serializer(source_cpp_file, relationship,  relationship.get('name') + '_node' + '.node().attribute("filename").as_string()')

			source_cpp_file.write('configuration::set_configuration("' + relationship.get('path') + '/' + relationship.get("name") + '", ' + relationship.get('name') + ');\n')

		else:

			source_cpp_file.write('pugi::xpath_node_set ' + relationship.get('name') + '_nodes = document.select_nodes("' + relationship.get('path') + '/' + relationship.get('name') + '");\n')
			source_cpp_file.write('for (pugi::xpath_node_set::const_iterator iterator = ' + relationship.get('name') + '_nodes.begin(); iterator != ' + relationship.get('name') + '_nodes.end(); ++iterator)\n')
			source_cpp_file.write('{\n')
			source_cpp_file.write('std::shared_ptr<gb::' + relationship.get('type') + '> ' + relationship.get('name') + ' = std::make_shared<gb::' + relationship.get('type') + '>();\n')

			if relationship.get("is_external") == '0':

				source_cpp_file.write('pugi::xpath_node node = (*iterator);\n')
				source_cpp_file.write(relationship.get('name') + '->serialize_xml(document, node);\n')

			else:

				write_external_filename_serializer(source_cpp_file, relationship, '(*iterator).node().attribute("filename").as_string()')	

			source_cpp_file.write('configuration::set_configuration("' + relationship.get('path') + '/' + relationship.get("name") + '", ' + relationship.get('name') + ');\n')
			source_cpp_file.write('}\n')

def write_relationships_serializer_json(source_cpp_file, relationships):
	for relationship in relationships:

		if relationship.get("is_to_many") == '0':

			source_cpp_file.write('std::shared_ptr<gb::' + relationship.get('type') + '> ' + relationship.get('name') + ' = std::make_shared<gb::' + relationship.get('type') + '>();\n')

			if relationship.get("is_external") == '0':

				pathes = relationship.get('path').split("/")
				pathes.append(relationship.get('name'))
				pathes.pop(0)
				pathes.pop(0)
				current_path = ''
				for path in pathes:
					current_path += '[\"' + path + '\"]'

				source_cpp_file.write(relationship.get('name') + '->serialize_json(json' + current_path + ');\n')

			else:

				source_cpp_file.write('Json::Value ' + relationship.get('name') + '_json' + ' = json[\"' + relationship.get('name') + '\"];\n')
				write_external_filename_serializer(source_cpp_file, relationship, relationship.get('name') + '_json' + '.get("filename", "unknown").asString()')
				
			source_cpp_file.write('configuration::set_configuration("' + relationship.get('path') + '/' + relationship.get("name") + '", ' + relationship.get('name') + ');\n')

		else:

			pathes = relationship.get('path').split("/")
			pathes.pop(0)
			pathes.pop(0)
			current_path = ''
			for path in pathes:
				current_path += '[\"' + path + '\"]'

			source_cpp_file.write('Json::Value ' + relationship.get('name') + 's_json_array = json' + current_path + ';\n')
			source_cpp_file.write('for (Json::ValueIterator iterator = ' + relationship.get('name') + 's_json_array.begin(); iterator != ' + relationship.get('name') + 's_json_array.end(); ++iterator)\n')
			source_cpp_file.write('{\n')
			source_cpp_file.write('std::shared_ptr<gb::' + relationship.get('type') + '> ' + relationship.get('name') + ' = std::make_shared<gb::' + relationship.get('type') + '>();\n')

			if relationship.get("is_external") == '0':

				source_cpp_file.write('Json::Value json_value = (*iterator);\n')
				source_cpp_file.write(relationship.get('name') + '->serialize_json(json_value);\n')

			else:

				source_cpp_file.write('Json::Value json_value = (*iterator);\n')
				write_external_filename_serializer(source_cpp_file, relationship, 'json_value.get("filename", "unknown").asString()')

			source_cpp_file.write('configuration::set_configuration("' + relationship.get('path') + '/' + relationship.get("name") + '", ' + relationship.get('name') + ');\n')
			source_cpp_file.write('}\n')

def write_relationships_deserializer(source_cpp_file, relationships, class_name):
	for relationship in relationships:

		if relationship.get("is_to_many") == '0':

			source_cpp_file.write('node = parent_node.append_child("' + relationship.get('name') + '");\n')

			if relationship.get("is_external") == '0':
				source_cpp_file.write(class_name + '::' + 'get_' + relationship.get('property') + '()->deserialize(node);\n')
			else:

				source_cpp_file.write('attribute = node.append_attribute("filename");\n')
				source_cpp_file.write('attribute.set_value(configuration::get_filename().c_str());\n')

		else:

			source_cpp_file.write('node = parent_node.append_child("' + relationship.get('path').split('/')[-1] + '");\n')
			source_cpp_file.write('for(const auto& iterator : ' +  class_name + '::get_' + relationship.get('property') + '())\n')
			source_cpp_file.write('{\n')
			source_cpp_file.write('std::shared_ptr<gb::' + relationship.get('type') + '> configuration = std::static_pointer_cast<gb::' + relationship.get('type') + '>(iterator);\n')
			source_cpp_file.write('pugi::xml_node child_node = node.append_child("' + relationship.get('name') + '");\n')

			if relationship.get("is_external") == '0':
				source_cpp_file.write('configuration->deserialize(child_node);\n')
			else:

				source_cpp_file.write('attribute = child_node.append_attribute("filename");\n')
				source_cpp_file.write('attribute.set_value(configuration->get_filename().c_str());\n')

			source_cpp_file.write('}\n')



def parse_xml(filename, accessor_class_source_h_file, accessor_class_source_cpp_file):
	document = ElementTree.parse(filename)
	root = document.getroot()

	class_name = root.get("class_name")
	xcode_print("generate configuration class: " + class_name)

	base_class_name = root.get("base_class_name")
	is_external = root.get("is_external")

	source_h_file = open(kConfigurationAutogeneratedSourcesPath + class_name + '.h', 'w+')
	source_cpp_file = open(kConfigurationAutogeneratedSourcesPath + class_name + '.cpp', 'w+')
	source_cpp_file.write('// autogenerated: do not add any changes\n')
	source_cpp_file.write('#include "'+ class_name + '.h"\n')
	source_cpp_file.write('namespace gb\n')
	source_cpp_file.write('{\n')
	
	source_h_file.write('// autogenerated: do not add any changes\n')
	source_h_file.write('#ifndef ' + class_name + '_h\n')
	source_h_file.write('#define ' + class_name + '_h\n')
	source_h_file.write('#include "configuration.h"\n')
	
	for relationship in root.iter('relationship'):
		source_h_file.write('#include "' + relationship.get("type") + '.h"\n')

	source_h_file.write('namespace gb\n')
	source_h_file.write('{\n')

	source_h_file.write('class ' + class_name + ' : public ' + base_class_name + '\n')
	source_h_file.write('{\n')
	
	source_h_file.write('public:\n')
	source_h_file.write(class_name + '(void) = default;\n')
	source_h_file.write('~' + class_name + '(void) = default;\n')

	for attribute in root.iter('attribute'):

		if attribute.get('type') == 'GLenum':

			source_h_file.write('GLenum get_' + attribute.get("property") + '(void) const;\n')
			source_cpp_file.write('GLenum ' + class_name + '::get_' + attribute.get("property") + '(void) const\n')
			source_cpp_file.write('{\n')
			source_cpp_file.write('const auto& iterator = m_attributes.find(\"' + attribute.get("path") + '/' + attribute.get("name") + '\");\n')
			source_cpp_file.write('assert(iterator != m_attributes.end());\n')
			source_cpp_file.write('GLenum value; iterator->second->get(&value);\n')
			source_cpp_file.write('return value;\n')
			source_cpp_file.write('}\n')

			source_h_file.write('#if defined(__EDITOR__)\n')
			source_h_file.write('void set_' + attribute.get('property') + '(GLenum ' + attribute.get('name') + ');\n')
			source_h_file.write('#endif\n')

			source_cpp_file.write('#if defined(__EDITOR__)\n')
			source_cpp_file.write('void ' + class_name + '::set_' + attribute.get('property') + '(GLenum ' + attribute.get('name') + ')\n')
			source_cpp_file.write('{\n')
			source_cpp_file.write('configuration::set_attribute("' + attribute.get("path") + '/' + attribute.get("name") + '", std::make_shared<configuration_attribute>(' + attribute.get("name") + '));\n')
			source_cpp_file.write('}\n')
			source_cpp_file.write('#endif\n')

		else:

			source_h_file.write(attribute.get('type') + ' get_' + attribute.get("property") + '(void) const;\n')
			source_cpp_file.write(attribute.get('type') + ' ' + class_name + '::get_' + attribute.get("property") + '(void) const\n')
			source_cpp_file.write('{\n')
			source_cpp_file.write('const auto& iterator = m_attributes.find(\"' + attribute.get("path") + '/' + attribute.get("name") + '\");\n')
			source_cpp_file.write('assert(iterator != m_attributes.end());\n')
			source_cpp_file.write(attribute.get('type') + ' value; iterator->second->get(&value);\n')
			source_cpp_file.write('return value;\n')
			source_cpp_file.write('}\n')

			source_h_file.write('#if defined(__EDITOR__)\n')
			source_h_file.write('void set_' + attribute.get('property') + '(' + attribute.get('type') + ' ' + attribute.get('name') + ');\n')
			source_h_file.write('#endif\n')

			source_cpp_file.write('#if defined(__EDITOR__)\n')
			source_cpp_file.write('void ' + class_name + '::set_' + attribute.get('property') + '(' + attribute.get('type') + ' ' + attribute.get('name') + ')\n')
			source_cpp_file.write('{\n')
			source_cpp_file.write('configuration::set_attribute("' + attribute.get("path") + '/' + attribute.get("name") + '", std::make_shared<configuration_attribute>(' + attribute.get("name") + '));\n')
			source_cpp_file.write('}\n')
			source_cpp_file.write('#endif\n')
		

	for relationship in root.iter('relationship'):

		if relationship.get("is_to_many") == '0':

			source_h_file.write('std::shared_ptr<' + relationship.get("type") + '> get_' + relationship.get("property") + '(void) const;\n')
			source_cpp_file.write('std::shared_ptr<gb::' + relationship.get("type") + '> ' + class_name + '::get_' + relationship.get("property") + '(void) const\n')
			source_cpp_file.write('{\n')
			source_cpp_file.write('const auto& iterator = m_configurations.find(\"' + relationship.get("path") + '/' + relationship.get("name") + '\");\n')
			source_cpp_file.write('if(iterator == m_configurations.end())\n')
			source_cpp_file.write('{\n')
			source_cpp_file.write('return nullptr;\n')
			source_cpp_file.write('}\n')
			source_cpp_file.write('assert(iterator != m_configurations.end());\n')
			source_cpp_file.write('assert(iterator->second.size() != 0);\n')
			source_cpp_file.write('return std::static_pointer_cast<gb::' + relationship.get("type") + '>(iterator->second.at(0));\n')
			source_cpp_file.write('}\n')

			source_h_file.write('#if defined(__EDITOR__)\n')
			source_h_file.write('void set_' + relationship.get('property') + '(const std::shared_ptr<gb::' + relationship.get('type') + '>& ' + relationship.get('name') + ');\n')
			source_h_file.write('#endif\n')

			source_cpp_file.write('#if defined(__EDITOR__)\n')
			source_cpp_file.write('void ' + class_name + '::set_' + relationship.get('property') + '(const std::shared_ptr<gb::' + relationship.get('type') + '>& ' + relationship.get('name') + ')\n')
			source_cpp_file.write('{\n')
			source_cpp_file.write('configuration::set_configuration("' + relationship.get('path') + '/' + relationship.get("name") + '", ' + relationship.get('name') + ', 0);\n')
			source_cpp_file.write('}\n')
			source_cpp_file.write('#endif\n')

		else:

			source_h_file.write('std::vector<std::shared_ptr<configuration>> get_' + relationship.get("property") + '(void) const;\n')
			source_cpp_file.write('std::vector<std::shared_ptr<configuration>> ' + class_name + '::get_' + relationship.get("property") + '(void) const\n')
			source_cpp_file.write('{\n')
			source_cpp_file.write('const auto& iterator = m_configurations.find(\"' + relationship.get("path") + '/' + relationship.get("name") + '\");\n')
			source_cpp_file.write('if(iterator == m_configurations.end())\n')
			source_cpp_file.write('{\n')
			source_cpp_file.write('return std::vector<std::shared_ptr<configuration>>();\n')
			source_cpp_file.write('}\n')
			source_cpp_file.write('assert(iterator != m_configurations.end());\n')
			source_cpp_file.write('return iterator->second;\n')
			source_cpp_file.write('}\n')

			source_h_file.write('#if defined(__EDITOR__)\n')
			source_h_file.write('void add_' + relationship.get('property') + '(const std::shared_ptr<gb::' + relationship.get('type') + '>& ' + relationship.get('name') + ');\n')
			source_h_file.write('#endif\n')

			source_cpp_file.write('#if defined(__EDITOR__)\n')
			source_cpp_file.write('void ' + class_name + '::add_' + relationship.get('property') + '(const std::shared_ptr<gb::' + relationship.get('type') + '>& ' + relationship.get('name') + ')\n')
			source_cpp_file.write('{\n')
			source_cpp_file.write('configuration::set_configuration("' + relationship.get('path') + '/' + relationship.get("name") + '", ' + relationship.get('name') + ');\n')
			source_cpp_file.write('}\n')
			source_cpp_file.write('#endif\n')

			source_h_file.write('#if defined(__EDITOR__)\n')
			source_h_file.write('void set_' + relationship.get('property') + '(const std::shared_ptr<gb::' + relationship.get('type') + '>& ' + relationship.get('name') + ', i32 index);\n')
			source_h_file.write('#endif\n')

			source_cpp_file.write('#if defined(__EDITOR__)\n')
			source_cpp_file.write('void ' + class_name + '::set_' + relationship.get('property') + '(const std::shared_ptr<gb::' + relationship.get('type') + '>& ' + relationship.get('name') + ', i32 index)\n')
			source_cpp_file.write('{\n')
			source_cpp_file.write('configuration::set_configuration("' + relationship.get('path') + '/' + relationship.get("name") + '", ' + relationship.get('name') + ', index);\n')
			source_cpp_file.write('}\n')
			source_cpp_file.write('#endif\n')

	if is_external == "0":

		source_h_file.write('void serialize_xml(pugi::xml_document& document, const std::string& path);\n')
		source_h_file.write('void serialize_json(Json::Value& root);\n')
		
		source_cpp_file.write('void ' + class_name + '::serialize_xml(pugi::xml_document& document, const std::string& path)\n')
		source_cpp_file.write('{\n')
		source_cpp_file.write('pugi::xpath_node node;\n')
		source_cpp_file.write('node = document.select_single_node((path + "/' + root.tag + '").c_str());\n')
		write_attributes_serializer_xml(source_cpp_file, root.iter('attribute'))
		write_relationships_serializer_xml(source_cpp_file, root.iter('relationship'))
		source_cpp_file.write('}\n')

		source_cpp_file.write('void ' + class_name + '::serialize_json(Json::Value& json)\n')
		source_cpp_file.write('{\n')
		#source_cpp_file.write('Json::Value json;\n')
		#source_cpp_file.write('json = root[\"' + root.tag + '\"];\n')
		write_attributes_serializer_json(source_cpp_file, root.iter('attribute'))
		write_relationships_serializer_json(source_cpp_file, root.iter('relationship'))
		source_cpp_file.write('}\n')

		#source_h_file.write('#if defined(__EDITOR__)\n')
		#source_h_file.write('void deserialize(pugi::xml_node& node);\n')
		#source_h_file.write('#endif\n')

		#source_cpp_file.write('#if defined(__EDITOR__)\n')
		#source_cpp_file.write('void ' + class_name + '::deserialize(pugi::xml_node& node)\n')
		#source_cpp_file.write('{\n')
		#source_cpp_file.write('pugi::xml_attribute attribute;\n');
		#write_attributes_deserializer(source_cpp_file, root.iter('attribute'), class_name)
		#write_relationships_deserializer(source_cpp_file, root.iter('relationship'), class_name)
		#source_cpp_file.write('}\n')
		#source_cpp_file.write('#endif\n')

	else:

		source_h_file.write('void serialize_xml(const std::string& filename);\n')
		source_h_file.write('void serialize_json(const std::string& filename);\n')

		source_cpp_file.write('void ' + class_name + '::serialize_xml(const std::string& filename)\n')
		source_cpp_file.write('{\n')
		source_cpp_file.write('pugi::xml_document document;\n')
		source_cpp_file.write('pugi::xml_parse_result result = configuration::open_xml(document, filename);\n')
		source_cpp_file.write('assert(result.status == pugi::status_ok);\n')
		source_cpp_file.write('pugi::xpath_node node;\n')
		source_cpp_file.write('node = document.select_single_node("/' + root.tag + '");\n')
		write_attributes_serializer_xml(source_cpp_file, root.iter('attribute'))
		write_relationships_serializer_xml(source_cpp_file, root.iter('relationship'))
		source_cpp_file.write('}\n')

		source_cpp_file.write('void ' + class_name + '::serialize_json(const std::string& filename)\n')
		source_cpp_file.write('{\n')
		source_cpp_file.write('Json::Value json;\n')
		source_cpp_file.write('bool result = configuration::open_json(json, filename);\n')
		source_cpp_file.write('assert(result);\n')
		write_attributes_serializer_json(source_cpp_file, root.iter('attribute'))
		write_relationships_serializer_json(source_cpp_file, root.iter('relationship'))
		source_cpp_file.write('}\n')

		#source_h_file.write('#if defined(__EDITOR__)\n')
		#source_h_file.write('void deserialize(const std::string& filename);\n')
		#source_h_file.write('#endif\n')

		#source_cpp_file.write('#if defined(__EDITOR__)\n')
		#source_cpp_file.write('void ' + class_name + '::deserialize(const std::string& filename)\n')
		#source_cpp_file.write('{\n')
		#source_cpp_file.write('pugi::xml_document document;\n')
		#source_cpp_file.write('pugi::xml_parse_result result = document.load("");\n')
		#source_cpp_file.write('assert(result.status == pugi::status_ok);\n')
		#source_cpp_file.write('pugi::xml_node node = document.append_child("' + root.tag + '");\n')
		#source_cpp_file.write('pugi::xml_node parent_node = node;\n')
		#source_cpp_file.write('pugi::xml_attribute attribute;\n')
		#write_attributes_deserializer(source_cpp_file, root.iter('attribute'), class_name)
		#write_relationships_deserializer(source_cpp_file, root.iter('relationship'), class_name)
		#source_cpp_file.write('document.save_file(filename.c_str());\n')
		#source_cpp_file.write('}\n')
		#source_cpp_file.write('#endif\n')
	

	if is_external == "0":

		source_h_file.write('void serialize_xml(pugi::xml_document& document, pugi::xpath_node& node);\n')
		source_cpp_file.write('void ' + class_name + '::serialize_xml(pugi::xml_document& document, pugi::xpath_node& node)\n')
		source_cpp_file.write('{\n')
		write_attributes_serializer_xml(source_cpp_file, root.iter('attribute'))
		write_relationships_serializer_xml(source_cpp_file, root.iter('relationship'))
		source_cpp_file.write('}\n')

	source_cpp_file.write('}\n')
	source_h_file.write('};\n')
	source_h_file.write('};\n')
	source_h_file.write('#endif\n')
	source_h_file.close()
	source_cpp_file.close()

def delete_character(string, n): 
    begin = string[:n]  
    end = string[n+1:] 
    return begin + end

def write_acessor_external_headers(configurations, accessor_class_source_h_file):
	
	for configuration in configurations:

		document = ElementTree.parse(configuration)
		root = document.getroot()
		class_name = root.get("class_name")
		is_external = root.get("is_external")
		if is_external == '1':
			accessor_class_source_h_file.write('#include "' + class_name + '.h"\n')


def write_acessor_header_getters(configurations, accessor_class_source_h_file):

	for configuration in configurations:

		document = ElementTree.parse(configuration)
		root = document.getroot()
		class_name = root.get("class_name")
		is_external = root.get("is_external")
		if is_external == '1':
			accessor_class_source_h_file.write('std::shared_ptr<configuration> get_' + class_name + '(const std::string& filename) const;\n')


def write_acessor_source_getters(configurations, accessor_class_source_cpp_file, accessor_class_name):

	for configuration in configurations:

		document = ElementTree.parse(configuration)
		root = document.getroot()
		class_name = root.get("class_name")
		is_external = root.get("is_external")
		if is_external == '1':

			accessor_class_source_cpp_file.write('std::shared_ptr<configuration> ' + accessor_class_name +'::get_' + class_name + '(const std::string& filename) const\n')
			accessor_class_source_cpp_file.write('{\n')
			accessor_class_source_cpp_file.write('std::shared_ptr<' + class_name + '> configuration = std::make_shared<' + class_name + '>();\n')
			accessor_class_source_cpp_file.write('if(filename.find(".xml") != std::string::npos)\n')
			accessor_class_source_cpp_file.write('{\n')
			accessor_class_source_cpp_file.write('configuration->serialize_xml(filename);\n')
			accessor_class_source_cpp_file.write('}\n')
			accessor_class_source_cpp_file.write('else if(filename.find(".json") != std::string::npos)\n')
			accessor_class_source_cpp_file.write('{\n')
			accessor_class_source_cpp_file.write('configuration->serialize_json(filename);\n')
			accessor_class_source_cpp_file.write('}\n')
			accessor_class_source_cpp_file.write('else\n')
			accessor_class_source_cpp_file.write('{\n')
			accessor_class_source_cpp_file.write('assert(false);\n')
			accessor_class_source_cpp_file.write('}\n')
			accessor_class_source_cpp_file.write('assert(configuration);\n')
			accessor_class_source_cpp_file.write('return configuration;\n')
			accessor_class_source_cpp_file.write('}\n')

def main(argv):

	accessor_class_name = ""
	base_accessor_class_name = ""

	for argument in argv[:]:

		if "base_accessor_" in argument:

			base_accessor_class_name = argument[len("base_accessor_"):]
			print base_accessor_class_name

		elif "current_accessor_" in argument:

			accessor_class_name = argument[len("current_accessor_"):]
			print accessor_class_name

	configurations = get_configuration_samples(kConfiguratiomPath, ".xml")
	accessor_class_source_h_file = open(kConfigurationAutogeneratedSourcesPath + accessor_class_name + '.h', 'w+')
	accessor_class_source_cpp_file = open(kConfigurationAutogeneratedSourcesPath + accessor_class_name + '.cpp', 'w+')

	accessor_class_source_h_file.write('// autogenerated: do not add any changes, added changes will be lost.\n')
	accessor_class_source_h_file.write('#ifndef ' + accessor_class_name + '_h\n')
	accessor_class_source_h_file.write('#define ' + accessor_class_name + '_h\n')

	if len(base_accessor_class_name) != 0:
		accessor_class_source_h_file.write('#include "' + base_accessor_class_name + '.h"\n')
	write_acessor_external_headers(configurations, accessor_class_source_h_file)

	accessor_class_source_h_file.write('namespace gb\n')
	accessor_class_source_h_file.write('{\n')

	if len(base_accessor_class_name) != 0:
		accessor_class_source_h_file.write('class ' + accessor_class_name + ': public ' + base_accessor_class_name + '\n')
	else:
		accessor_class_source_h_file.write('class ' + accessor_class_name + '\n')
	accessor_class_source_h_file.write('{\n')
	accessor_class_source_h_file.write('public:\n')
	accessor_class_source_h_file.write(accessor_class_name + '(void) = default;\n')
	accessor_class_source_h_file.write('~' + accessor_class_name + '(void) = default;\n')
	write_acessor_header_getters(configurations, accessor_class_source_h_file)
	
	accessor_class_source_cpp_file.write('// autogenerated: do not add any changes, added changes will be lost.\n')
	accessor_class_source_cpp_file.write('#include "'+ accessor_class_name + '.h"\n')
	accessor_class_source_cpp_file.write('namespace gb\n')
	accessor_class_source_cpp_file.write('{\n')
	write_acessor_source_getters(configurations, accessor_class_source_cpp_file, accessor_class_name)

	for configuration in configurations:
		parse_xml(configuration, accessor_class_source_h_file, accessor_class_source_cpp_file)

	accessor_class_source_cpp_file.write('}\n')
	accessor_class_source_h_file.write('};\n')
	accessor_class_source_h_file.write('};\n')
	accessor_class_source_h_file.write('#endif\n')

	accessor_class_source_h_file.close()
	accessor_class_source_cpp_file.close()

if __name__ == "__main__":
    main(sys.argv[1:])