#define main_headers_h
#define configuration_h
#define common_h
#define NO_GRAPHICS_API -1
#define USED_GRAPHICS_API -1
#include <cassert>
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <array>
#include <map>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <pugixml/pugixml.hpp>
#include <jsoncpp-1.7.2/json/json.h>
using ui32=uint32_t;using i32=int32_t;
std::string bundlepath(){return "configuration-cache-bundle/";}
namespace gb {
class configuration_attribute{};
class configuration {protected:std::string m_filename;std::unordered_map<std::string,std::shared_ptr<configuration_attribute>> m_attributes;std::unordered_map<std::string,std::vector<std::shared_ptr<configuration>>> m_configurations;
public:configuration();~configuration();void set_attribute(const std::string&,const std::shared_ptr<configuration_attribute>&);void set_configuration(const std::string&,const std::shared_ptr<configuration>&,i32);pugi::xml_parse_result open_xml(pugi::xml_document&,const std::string&);bool open_json(Json::Value&,const std::string&);std::string get_filename() const;};
class game_object_configuration:public configuration {public:i32 get_z_order()const;};
}
#include "../gbCore/sources/configuration.cpp"
static void require(bool ok,const char* why){if(!ok)throw std::runtime_error(why);}
static void write(const std::string& filename,const std::string& contents){std::ofstream file(filename,std::ios::binary);file<<contents;}
int main(){try {
 gb::configuration config;pugi::xml_document one,two,three;
 write("configuration-cache.xml","<root value='one'><child/></root>");require(config.open_xml(one,"configuration-cache.xml"),"first load failed");
 one.child("root").attribute("value").set_value("mutated");
 require(config.open_xml(two,"configuration-cache.xml")&&std::string(two.child("root").attribute("value").value())=="one","caller mutation leaked into shared DOM");
 write("configuration-cache.xml","<root value='two'><child/></root>");
 require(config.open_xml(three,"configuration-cache.xml")&&std::string(three.child("root").attribute("value").value())=="two","same-size content edit returned stale cache");
 write("configuration-fallback.xml","<broken");write(bundlepath()+"configuration-fallback.xml","<fallback/>");pugi::xml_document fallback;
 require(config.open_xml(fallback,"configuration-fallback.xml")&&fallback.child("fallback"),"bundle fallback changed");
 for(int i=0;i<70;++i){auto name="configuration-evict-"+std::to_string(i)+".xml";write(name,"<entry index='"+std::to_string(i)+"'/>");pugi::xml_document doc;require(config.open_xml(doc,name),"cache limit broke parsing");}
 pugi::xml_document reloaded;require(config.open_xml(reloaded,"configuration-cache.xml")&&std::string(reloaded.child("root").attribute("value").value())=="two","eviction broke content");
 std::cout<<"PASS real XML cache independent DOM, same-size edits, bundle fallback, capacity eviction\n";return 0;
 }catch(const std::exception& e){std::cerr<<"FAIL "<<e.what()<<"\n";return 1;}}
