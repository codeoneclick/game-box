#include "graphics_settings.h"
#include <jsoncpp-1.7.2/json/json.h>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <limits>
#include <vector>
#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <direct.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif
#endif

namespace game {
namespace {
#ifdef _WIN32
std::wstring wide(const std::string& value) {
    const int n=MultiByteToWideChar(CP_UTF8,0,value.c_str(),-1,nullptr,0);
    std::wstring result(n,0);
    MultiByteToWideChar(CP_UTF8,0,value.c_str(),-1,&result[0],n);
    if(!result.empty()) result.pop_back();
    return result;
}
std::string utf8(const wchar_t* value) {
    const int n=WideCharToMultiByte(CP_UTF8,0,value,-1,nullptr,0,nullptr,nullptr);
    std::string result(n,0);
    WideCharToMultiByte(CP_UTF8,0,value,-1,&result[0],n,nullptr,nullptr);
    if(!result.empty()) result.pop_back();
    return result;
}
#endif
bool directory(const std::string& path) {
#ifdef _WIN32
    const auto attributes=GetFileAttributesW(wide(path).c_str());
    return attributes!=INVALID_FILE_ATTRIBUTES && (attributes&FILE_ATTRIBUTE_DIRECTORY)!=0;
#else
    struct stat info;
    return stat(path.c_str(),&info)==0 && S_ISDIR(info.st_mode);
#endif
}
std::string parent(std::string path) {
    const auto separator=path.find_last_of("/\\");
    return separator==std::string::npos?std::string():path.substr(0,separator);
}
Json::Value encode(const StylizedPostProcessSettings& post, const StylizedLightingSettings& lighting,const TerrainLodSettings& lod) {
    Json::Value root(Json::objectValue); root["version"]=1;
    root["renderPasses"]=Json::Value(Json::objectValue);
    for(const auto& pass:post.renderPasses) root["renderPasses"][pass.first]=pass.second;
    root["terrainLOD"]["lodCount"]=lod.lodCount;
    root["terrainLOD"]["contourSimplificationPercent"]=lod.contourSimplificationPercent;
    root["terrainLOD"]["wireframe"]=lod.wireframe;root["terrainLOD"]["nearErrorPx"]=lod.nearErrorPx;root["terrainLOD"]["normalErrorPx"]=lod.normalErrorPx;root["terrainLOD"]["farErrorPx"]=lod.farErrorPx;root["terrainLOD"]["debugMode"]=lod.debugMode;
    auto& p=root["postProcess"]; auto& l=root["lighting"];
#define POST(field) p[#field]=post.field
    POST(enableSSAO); POST(enableToneMapping); POST(enableColorGrading); POST(enableFog);
    POST(ssaoIntensity); POST(exposure); POST(saturation); POST(contrast); POST(fogStrength); POST(debugView);
    POST(enableWaterReflection); POST(enableWaterRefraction); POST(enableWaterCaustics);
    POST(waterReflectionIntensity); POST(waterRefractionIntensity); POST(waterCausticsIntensity);
#undef POST
#define LIGHT(field) l[#field]=lighting.field
    LIGHT(mode); LIGHT(timeOfDay); LIGHT(skyboxRotation); LIGHT(sunIntensity); LIGHT(ambientStrength); LIGHT(wrapAmount);
    LIGHT(diffusePower); LIGHT(globalSpecularMultiplier); LIGHT(vegetationBacklightStrength);
    LIGHT(enableStylizedSpecular); LIGHT(enableVegetationBacklight);
#undef LIGHT
    for(auto i=0;i<3;++i) { l["sunColor"].append(lighting.sunColor[i]); l["ambientColor"].append(lighting.ambientColor[i]); }
    return root;
}
bool valid_render_passes(const Json::Value& passes, std::string& error) {
    if(!passes.isObject() || passes.size()>256) {
        error="Invalid renderPasses object (maximum 256 entries)"; return false;
    }
    for(const auto& guid:passes.getMemberNames()) {
        const bool nameValid=!guid.empty() && guid.size()<=128 &&
            std::all_of(guid.begin(),guid.end(),[](unsigned char c) {
                return (c>='a' && c<='z') || (c>='A' && c<='Z') ||
                    (c>='0' && c<='9') || c=='.' || c=='_' || c=='-';
            });
        if(!nameValid || !passes[guid].isBool()) {
            error="Invalid renderPasses GUID or boolean value"; return false;
        }
    }
    return true;
}
// Validate against the encoded schema before touching either live settings object.
bool valid(const Json::Value& value, const Json::Value& schema, const std::string& path, std::string& error) {
    bool match=false;
    if(schema.isObject()) {
        match=value.isObject();
        if(match) for(const auto& key:schema.getMemberNames())
            if(value.isMember(key) && !valid(value[key],schema[key],path+"."+key,error)) return false;
    } else if(schema.isArray()) {
        match=value.isArray() && value.size()==schema.size();
        if(match) for(Json::ArrayIndex i=0;i<schema.size();++i)
            if(!valid(value[i],schema[i],path+"[]",error)) return false;
    } else if(schema.isBool()) match=value.isBool();
    else if(schema.type()==Json::intValue || schema.type()==Json::uintValue) match=value.isInt();
    else match=value.isNumeric() && std::isfinite(value.asDouble()) &&
        std::abs(value.asDouble())<=std::numeric_limits<float>::max();
    if(!match) error="Invalid value at "+path;
    return match;
}
}

std::string graphics_settings_path() {
    std::vector<std::string> roots;
#ifdef _WIN32
    wchar_t executable[32768]={}, cwd[32768]={};
    if(GetModuleFileNameW(nullptr,executable,32768)) roots.push_back(parent(utf8(executable)));
    if(_wgetcwd(cwd,32768)) roots.push_back(utf8(cwd));
#else
    char cwd[4096]={};
#ifdef __APPLE__
    char executable[4096]={}; uint32_t size=sizeof(executable);
    if(_NSGetExecutablePath(executable,&size)==0) roots.push_back(parent(executable));
#endif
    if(getcwd(cwd,sizeof(cwd))) roots.push_back(cwd);
#endif
    for(auto root:roots) for(int level=0;level<10 && !root.empty();++level,root=parent(root))
        if(directory(root+"/gbResources")) return root+"/gbResources/GraphicsSettings.json";
    return (roots.empty()?std::string("."):roots.back())+"/gbResources/GraphicsSettings.json";
}

bool load_graphics_settings(const std::string& path, StylizedPostProcessSettings& post,
                            StylizedLightingSettings& lighting,TerrainLodSettings& lod,std::string& error) {
    error.clear();
#ifdef _WIN32
    std::ifstream stream(wide(path).c_str(),std::ios::binary);
#else
    std::ifstream stream(path,std::ios::binary);
#endif
    if(!stream) { error="Cannot open graphics settings"; return false; }
    // JsonCpp 1.7's failIfExtra ignores invalid trailing tokens. An enclosing
    // array makes trailing garbage a syntax error inside the document instead.
    std::ostringstream contents; contents<<stream.rdbuf();
    if(stream.bad()) { error="Cannot read graphics settings"; return false; }
    auto document=contents.str();
    if(document.compare(0,3,"\xEF\xBB\xBF")==0) document.erase(0,3);
    std::istringstream wrapped("["+document+"]");
    Json::Value envelope; Json::CharReaderBuilder reader;
    reader["collectComments"]=false;
    reader["failIfExtra"]=true;
    reader["rejectDupKeys"]=true;
    if(!Json::parseFromStream(reader,wrapped,&envelope,&error)) return false;
    if(envelope.size()!=1) { error="Expected one graphics settings object"; return false; }
    const auto& root=envelope[0];
    if(!valid(root,encode(post,lighting,lod),"settings",error)) return false;
    if(root.isMember("renderPasses") && !valid_render_passes(root["renderPasses"],error)) return false;
    if(root.isMember("version") && root["version"].asInt()!=1) { error="Unsupported graphics settings version"; return false; }
    TerrainLodSettings next=lod;const auto& jlsettings=root["terrainLOD"];
    const int count=jlsettings.isMember("lodCount")?jlsettings["lodCount"].asInt():5;
    const float percent=jlsettings.isMember("contourSimplificationPercent")?jlsettings["contourSimplificationPercent"].asFloat():0.f;
    if(count<1||count>8||!std::isfinite(percent)||percent<0||percent>90){error="Invalid terrainLOD range";return false;}
    next.set_lod_count(count);next.set_contour_percent(percent);
    next.nearErrorPx=jlsettings.isMember("nearErrorPx")?jlsettings["nearErrorPx"].asFloat():.5f;next.normalErrorPx=jlsettings.isMember("normalErrorPx")?jlsettings["normalErrorPx"].asFloat():1.5f;next.farErrorPx=jlsettings.isMember("farErrorPx")?jlsettings["farErrorPx"].asFloat():5.f;next.debugMode=jlsettings.isMember("debugMode")?jlsettings["debugMode"].asInt():0;if(!next.valid()){error="Invalid terrain pixel error range";return false;}
    next.wireframe=jlsettings.isMember("wireframe")?jlsettings["wireframe"].asBool():false;
    auto p=post; auto l=lighting;
    if(root.isMember("renderPasses")) {
        p.renderPasses.clear();
        for(const auto& guid:root["renderPasses"].getMemberNames())
            p.renderPasses[guid]=root["renderPasses"][guid].asBool();
    }
    const auto& jp=root["postProcess"]; const auto& jl=root["lighting"];
#define POST_BOOL(field) if(jp.isMember(#field)) p.field=jp[#field].asBool()
    POST_BOOL(enableSSAO); POST_BOOL(enableToneMapping); POST_BOOL(enableColorGrading); POST_BOOL(enableFog);
    POST_BOOL(enableWaterReflection); POST_BOOL(enableWaterRefraction); POST_BOOL(enableWaterCaustics);
#undef POST_BOOL
#define POST_FLOAT(field) if(jp.isMember(#field)) p.field=jp[#field].asFloat()
    POST_FLOAT(ssaoIntensity); POST_FLOAT(exposure); POST_FLOAT(saturation); POST_FLOAT(contrast); POST_FLOAT(fogStrength);
    POST_FLOAT(waterReflectionIntensity); POST_FLOAT(waterRefractionIntensity); POST_FLOAT(waterCausticsIntensity);
#undef POST_FLOAT
    if(jp.isMember("debugView")) p.debugView=jp["debugView"].asInt();
    if(jl.isMember("mode")) l.mode=jl["mode"].asInt();
#define LIGHT_FLOAT(field) if(jl.isMember(#field)) l.field=jl[#field].asFloat()
    LIGHT_FLOAT(timeOfDay); LIGHT_FLOAT(skyboxRotation); LIGHT_FLOAT(sunIntensity); LIGHT_FLOAT(ambientStrength); LIGHT_FLOAT(wrapAmount);
    LIGHT_FLOAT(diffusePower); LIGHT_FLOAT(globalSpecularMultiplier); LIGHT_FLOAT(vegetationBacklightStrength);
#undef LIGHT_FLOAT
    if(jl.isMember("enableStylizedSpecular")) l.enableStylizedSpecular=jl["enableStylizedSpecular"].asBool();
    if(jl.isMember("enableVegetationBacklight")) l.enableVegetationBacklight=jl["enableVegetationBacklight"].asBool();
    for(Json::ArrayIndex i=0;i<3;++i) {
        if(jl.isMember("sunColor")) l.sunColor[i]=jl["sunColor"][i].asFloat();
        if(jl.isMember("ambientColor")) l.ambientColor[i]=jl["ambientColor"][i].asFloat();
    }
    p.sanitize(); l.sanitize(); post=p; lighting=l; lod=next; return true;
}

bool save_graphics_settings(const std::string& path, const StylizedPostProcessSettings& post,
                            const StylizedLightingSettings& lighting,const TerrainLodSettings& lod,std::string& error) {
    error.clear(); auto p=post; auto l=lighting; p.sanitize(); l.sanitize();
    if(!lod.valid()){error="Invalid terrainLOD range";return false;}
    const auto root=encode(p,l,lod);
    if(!valid_render_passes(root["renderPasses"],error)) return false;
    const auto temporary=path+".tmp";
#ifdef _WIN32
    std::ofstream stream(wide(temporary).c_str(),std::ios::binary|std::ios::trunc);
#else
    std::ofstream stream(temporary,std::ios::binary|std::ios::trunc);
#endif
    if(!stream) { error="Cannot create temporary graphics settings file"; return false; }
    Json::StyledWriter writer; stream<<writer.write(root); stream.flush();
    const bool written=stream.good(); stream.close();
    bool replaced=false;
    if(written && !stream.fail()) {
#ifdef _WIN32
        replaced=MoveFileExW(wide(temporary).c_str(),wide(path).c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)!=0;
#else
        replaced=std::rename(temporary.c_str(),path.c_str())==0;
#endif
    }
    if(!replaced) {
#ifdef _WIN32
        DeleteFileW(wide(temporary).c_str());
#else
        std::remove(temporary.c_str());
#endif
        error="Cannot write or replace graphics settings file"; return false;
    }
    return true;
}
bool load_graphics_settings(const std::string& path,StylizedPostProcessSettings& post,StylizedLightingSettings& lighting,std::string& error){TerrainLodSettings lod;return load_graphics_settings(path,post,lighting,lod,error);}
bool save_graphics_settings(const std::string& path,const StylizedPostProcessSettings& post,const StylizedLightingSettings& lighting,std::string& error){return save_graphics_settings(path,post,lighting,TerrainLodSettings(),error);}

}
