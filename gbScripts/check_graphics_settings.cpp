#include "graphics_settings.h"
#include "terrain_lod_settings.h"
#include <limits>
#include <cassert>
#include <fstream>
#include <cstdio>
#include <iostream>
#include <iterator>
#ifdef _WIN32
#include <sys/stat.h>
#include <io.h>
#endif
using namespace game;
static void write(const char* text) { std::ofstream("settings-test.json") << text; }
int main() {
    auto post=StylizedPostProcessSettings::desktop();
    auto light=StylizedLightingSettings::sunset();
    std::string error;
    {
        auto p=post; auto l=light;
        p.enableWaterReflection=false;p.enableWaterRefraction=false;p.enableWaterCaustics=false;
        p.waterReflectionIntensity=.6f;p.waterRefractionIntensity=1.7f;p.waterCausticsIntensity=2.4f;
        assert(save_graphics_settings("settings-test.json",p,l,error));
        auto restored=post;assert(load_graphics_settings("settings-test.json",restored,l,error));
        assert(!restored.enableWaterReflection&&!restored.enableWaterRefraction&&!restored.enableWaterCaustics);
        assert(restored.waterReflectionIntensity==.6f&&restored.waterRefractionIntensity==1.7f&&restored.waterCausticsIntensity==2.4f);
        write("{\"postProcess\":{\"enableWaterReflection\":1}}");
        assert(!load_graphics_settings("settings-test.json",restored,l,error));
        assert(!restored.enableWaterReflection);
    }
    {
        TerrainLodSettings lod;assert(!lod.wireframe);lod.wireframe=true;assert(lod.lodCount==5&&lod.contourSimplificationPercent==0);
        assert(lod.set_contour_percent(50));for(int i=0;i<5;++i)assert(lod.contour_percent(i)==i*12.5f);
        const auto revision=lod.revision;assert(!lod.set_contour_percent(50)&&lod.revision==revision);
        lod.set_lod_count(1);assert(lod.contour_percent(0)==0&&lod.contourSimplificationPercent==50);
        lod.set_lod_count(5);assert(lod.contour_percent(4)==50);
        auto p=post;auto l=light;assert(save_graphics_settings("settings-test.json",p,l,lod,error));
        TerrainLodSettings restored;assert(load_graphics_settings("settings-test.json",p,l,restored,error));
        assert(restored.lodCount==5&&restored.contourSimplificationPercent==50&&restored.wireframe);
        lod.nearErrorPx=.25f;lod.normalErrorPx=2.f;lod.farErrorPx=6.f;lod.debugMode=3;lod.set_contour_percent(90);
        assert(save_graphics_settings("settings-test.json",p,l,lod,error));assert(load_graphics_settings("settings-test.json",p,l,restored,error));
        assert(restored.nearErrorPx==.25f&&restored.normalErrorPx==2.f&&restored.farErrorPx==6.f&&restored.debugMode==3&&restored.contourSimplificationPercent==90);
        restored.set_contour_percent(50);
        write("{\"terrainLOD\":{\"nearErrorPx\":3,\"normalErrorPx\":1}}");assert(!load_graphics_settings("settings-test.json",p,l,restored,error));
        for(const char* invalid:{"{\"terrainLOD\":{\"wireframe\":1}}","{\"terrainLOD\":{\"lodCount\":5.5}}","{\"terrainLOD\":{\"lodCount\":0}}","{\"terrainLOD\":{\"lodCount\":9}}","{\"terrainLOD\":{\"contourSimplificationPercent\":-1}}","{\"terrainLOD\":{\"contourSimplificationPercent\":91}}","{\"terrainLOD\":{\"lodCount\":true}}"}){
            write(invalid);const auto before=restored.revision;assert(!load_graphics_settings("settings-test.json",p,l,restored,error));assert(restored.contourSimplificationPercent==50&&restored.revision==before);
        }
        assert(!restored.set_contour_percent(std::numeric_limits<float>::quiet_NaN()));
        write("{\"version\":1}");assert(load_graphics_settings("settings-test.json",p,l,restored,error));assert(restored.lodCount==5&&restored.contourSimplificationPercent==0&&!restored.wireframe);
    }
    write("{\"postProcess\":{\"exposure\":2},\"renderPasses\":{\"ws.base\":1}}");
    assert(!load_graphics_settings("settings-test.json",post,light,error));
    assert(post.exposure==.15f && !error.empty());
    post.enableSSAO=false; post.ssaoIntensity=0; post.exposure=.75f;
    post.renderPasses={{"ws.base",false},{"ss.future_pass-2",true}};
    light.skyboxRotation=234.5f;
    light.timeOfDay=21; light.sunColor={{.2f,.3f,.4f}}; light.enableVegetationBacklight=false;
    post.enableToneMapping=false; post.enableColorGrading=false; post.enableFog=false;
    post.saturation=1.17f; post.contrast=.83f; post.fogStrength=.19f; post.debugView=4;
    light.mode=0; light.ambientColor={{.17f,.23f,.31f}}; light.sunIntensity=2.1f;
    light.ambientStrength=.18f; light.wrapAmount=.25f; light.diffusePower=1.4f;
    light.globalSpecularMultiplier=.3f; light.vegetationBacklightStrength=.12f; light.enableStylizedSpecular=false;
    assert(save_graphics_settings("settings-test.json",post,light,error));
    std::ifstream saved("settings-test.json");
    const std::string content((std::istreambuf_iterator<char>(saved)),std::istreambuf_iterator<char>());
    saved.close();
    assert(content.find("Bloom")==std::string::npos && content.find("Vignette")==std::string::npos);
#ifdef _WIN32
    assert(_chmod("settings-test.json",_S_IREAD)==0);
    auto changed=post; changed.exposure=-1;
    assert(!save_graphics_settings("settings-test.json",changed,light,error));
    assert(_chmod("settings-test.json",_S_IREAD|_S_IWRITE)==0);
    std::ifstream preserved("settings-test.json");
    const std::string unchanged((std::istreambuf_iterator<char>(preserved)),std::istreambuf_iterator<char>());
    preserved.close(); assert(unchanged==content);
#endif
    auto loaded=StylizedPostProcessSettings::desktop(); auto lights=StylizedLightingSettings::soft();
    assert(load_graphics_settings("settings-test.json",loaded,lights,error));
    assert(!loaded.enableSSAO && loaded.ssaoIntensity==0 && loaded.exposure==.75f);
    assert(loaded.renderPasses==post.renderPasses);
    assert(lights.skyboxRotation==234.5f);
    assert(lights.timeOfDay==21 && lights.sunColor==light.sunColor && !lights.enableVegetationBacklight);
    assert(loaded.enableToneMapping==post.enableToneMapping && loaded.enableColorGrading==post.enableColorGrading);
    assert(loaded.enableFog==post.enableFog && loaded.saturation==post.saturation && loaded.contrast==post.contrast);
    assert(loaded.fogStrength==post.fogStrength && loaded.debugView==post.debugView);
    assert(lights.mode==light.mode && lights.ambientColor==light.ambientColor && lights.sunIntensity==light.sunIntensity);
    assert(lights.ambientStrength==light.ambientStrength && lights.wrapAmount==light.wrapAmount && lights.diffusePower==light.diffusePower);
    assert(lights.globalSpecularMultiplier==light.globalSpecularMultiplier && lights.vegetationBacklightStrength==light.vegetationBacklightStrength);
    assert(lights.enableStylizedSpecular==light.enableStylizedSpecular);
    write("{\"version\":1,\"postProcess\":{\"exposure\":1.25}}");
    assert(load_graphics_settings("settings-test.json",loaded,lights,error));
    assert(loaded.exposure==1.25f && !loaded.enableSSAO && lights.timeOfDay==21);
    assert(loaded.renderPasses==post.renderPasses); // Old/partial version 1 files retain existing overrides.
    for(const auto invalid : {"{", "{\"version\":2}", "{\"postProcess\":{\"exposure\":0,\"enableSSAO\":1}}",
            "{\"lighting\":{\"sunColor\":[1,2]}}", "[]", "{} trailing", "{\"version\":1,\"version\":1}",
            "{\"renderPasses\":[]}", "{\"renderPasses\":null}", "{\"renderPasses\":{\"\":true}}",
            "{\"renderPasses\":{\"ws/base\":true}}", "{\"renderPasses\":{\"ws.base\":\"false\"}}",
            "{\"postProcess\":{\"exposure\":2},\"lighting\":{\"timeOfDay\":5},\"renderPasses\":{\"new.pass\":false,\"bad\":0}}"}) {
        write(invalid);
        assert(!load_graphics_settings("settings-test.json",loaded,lights,error));
        assert(!error.empty() && loaded.exposure==1.25f && lights.timeOfDay==21);
        assert(loaded.renderPasses==post.renderPasses);
    }
    write(("{\"renderPasses\":{\""+std::string(129,'a')+"\":false}}").c_str());
    assert(!load_graphics_settings("settings-test.json",loaded,lights,error));
    assert(loaded.renderPasses==post.renderPasses);
    // Both name/count boundaries round-trip, including passes unknown to this build.
    auto bounded=post; bounded.renderPasses.clear();
    bounded.renderPasses[std::string(128,'a')]=false;
    for(int i=1;i<256;++i) bounded.renderPasses["future."+std::to_string(i)]=(i%2)==0;
    assert(save_graphics_settings("settings-test.json",bounded,light,error));
    auto boundaryLoaded=StylizedPostProcessSettings::desktop();
    assert(load_graphics_settings("settings-test.json",boundaryLoaded,lights,error));
    assert(boundaryLoaded.renderPasses==bounded.renderPasses);
    bounded.renderPasses["too.many"]=true;
    assert(!save_graphics_settings("settings-test.json",bounded,light,error));
    assert(load_graphics_settings("settings-test.json",boundaryLoaded,lights,error));
    assert(boundaryLoaded.renderPasses.size()==256); // Invalid save preserved prior file.
    std::string tooMany="{\"postProcess\":{\"exposure\":2},\"renderPasses\":{";
    for(int i=0;i<257;++i) tooMany+=(i?",":"")+std::string("\"pass.")+std::to_string(i)+"\":false";
    tooMany+="}}"; write(tooMany.c_str());
    assert(!load_graphics_settings("settings-test.json",loaded,lights,error));
    assert(loaded.renderPasses==post.renderPasses && loaded.exposure==1.25f);
    auto invalidSave=post; invalidSave.renderPasses["invalid guid"]=false;
    assert(!save_graphics_settings("settings-test.json",invalidSave,light,error));
    write("{\"version\":1,\"renderPasses\":{}}");
    assert(load_graphics_settings("settings-test.json",loaded,lights,error));
    assert(loaded.renderPasses.empty());
    auto defaults=StylizedPostProcessSettings::desktop();
    write("{\"version\":1}");
    assert(load_graphics_settings("settings-test.json",defaults,lights,error));
    assert(defaults.renderPasses.empty());
    write("{\"postProcess\":{\"ssaoIntensity\":99},\"lighting\":{\"timeOfDay\":-2}}");
    assert(load_graphics_settings("settings-test.json",loaded,lights,error));
    assert(loaded.ssaoIntensity==3 && lights.timeOfDay==0);
    assert(!load_graphics_settings("missing-folder/settings.json",loaded,lights,error));
    assert(!save_graphics_settings("missing-folder/settings.json",post,light,error));
#ifdef _WIN32
    _putenv_s("GB_POST_PRESET",""); _putenv_s("GB_POST_EXPOSURE","");
    assert(stylized_post_settings_from_environment(&post).exposure==.75f);
    _putenv_s("GB_POST_PRESET","typo");
    assert(stylized_post_settings_from_environment(&post).exposure==.75f);
    _putenv_s("GB_POST_EXPOSURE","-.25");
    assert(stylized_post_settings_from_environment(&post).exposure==-.25f);
    _putenv_s("GB_LIGHTING_PRESET",""); _putenv_s("GB_TIME_OF_DAY","6.5");
    assert(lighting_settings_from_environment(&light).timeOfDay==6.5f);
#endif
    assert(graphics_settings_path().find("gbResources")!=std::string::npos);
    std::remove("settings-test.json");
    std::cout << "Graphics settings persistence checks passed\n";
}
