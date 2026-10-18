#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <string>

namespace game {
// Demo presentation state, accessed only on the game/UI thread. No terrain data
// or resource ownership lives here. Materials share one sun and ambient fill.
struct StylizedLightingSettings {
    int mode = 1; // 0: legacy Cel comparison, 1: StylizedSoft.
    float skyboxRotation=0.f; // Degrees around world Y; independent of solar time.
    float timeOfDay=10.f; // Manual solar time; 0h and 24h are the same position.
    std::array<float,3> sunColor = {{1.f,.94f,.84f}};
    std::array<float,3> ambientColor = {{.55f,.68f,.82f}};
    float sunIntensity=1.1f, ambientStrength=.32f, wrapAmount=.35f, diffusePower=.8f;
    float globalSpecularMultiplier=1.f, vegetationBacklightStrength=.08f;
    bool enableStylizedSpecular=true, enableVegetationBacklight=true;
    // Material strengths; one broad highlight exponent keeps this inexpensive.
    float grassSpecular=.04f, woodSpecular=.06f, stoneSpecular=.1f;
    float leavesSpecular=.14f, plasterSpecular=.07f, metalSpecular=.35f;
    float specularPower=24.f;
    static StylizedLightingSettings soft(){return StylizedLightingSettings();}
    static StylizedLightingSettings sunset(){auto s=soft();s.timeOfDay=17.4f;s.sunColor={{1.f,.71f,.43f}};s.ambientColor={{.62f,.66f,.85f}};s.sunIntensity=1.25f;s.ambientStrength=.48f;s.wrapAmount=.4f;return s;}
    static StylizedLightingSettings mobile(){auto s=soft();s.globalSpecularMultiplier=.5f;s.enableVegetationBacklight=false;return s;}
    std::array<float,3> sun_direction()const{
        const float hour=std::isfinite(timeOfDay)?std::max(0.f,std::min(24.f,timeOfDay)):10.f;
        const float angle=(hour-6.f)*(6.28318530718f/24.f);
        return {{.70710678118f*std::cos(angle),std::sin(angle),.70710678118f*std::cos(angle)}};
    }
    float daylight()const{
        const float t=std::max(0.f,std::min(1.f,(sun_direction()[1]+.06f)/.24f));
        return t*t*(3.f-2.f*t);
    }
    void sanitize(){
        const auto d=soft();
        auto range=[](float& value,float lo,float hi,float fallback){value=std::isfinite(value)?std::max(lo,std::min(hi,value)):fallback;};
        mode=mode==0?0:1;
        range(timeOfDay,0,24,d.timeOfDay);
        range(skyboxRotation,0,360,d.skyboxRotation);
        for(size_t i=0;i<3;++i){range(sunColor[i],0,2,d.sunColor[i]);range(ambientColor[i],0,2,d.ambientColor[i]);}
        range(sunIntensity,0,3,d.sunIntensity);range(ambientStrength,0,1,d.ambientStrength);
        range(wrapAmount,.2f,.5f,d.wrapAmount);range(diffusePower,.4f,2,d.diffusePower);
        range(globalSpecularMultiplier,0,2,d.globalSpecularMultiplier);
        range(vegetationBacklightStrength,0,.2f,d.vegetationBacklightStrength);
        range(grassSpecular,0,1,d.grassSpecular);range(woodSpecular,0,1,d.woodSpecular);
        range(stoneSpecular,0,1,d.stoneSpecular);range(leavesSpecular,0,1,d.leavesSpecular);
        range(plasterSpecular,0,1,d.plasterSpecular);range(metalSpecular,0,1,d.metalSpecular);
        range(specularPower,16,32,d.specularPower);
    }
};
inline StylizedLightingSettings lighting_settings_from_environment(const StylizedLightingSettings* saved=nullptr){
    auto s=StylizedLightingSettings::sunset(); // Reference island presentation.
#if defined(__IOS__) || defined(__TVOS__)
    s=StylizedLightingSettings::mobile();
#endif
    if(saved)s=*saved;
    if(const char* p=std::getenv("GB_LIGHTING_PRESET")){
        const std::string preset(p);
        if(preset=="StylizedSunsetIsland"||preset=="sunset")s=StylizedLightingSettings::sunset();
        else if(preset=="StylizedSoftIsland"||preset=="soft")s=StylizedLightingSettings::soft();
        else if(preset=="mobile")s=StylizedLightingSettings::mobile();
    }
    if(const char* p=std::getenv("GB_LIGHTING_MODE"))s.mode=std::string(p)=="Cel"||std::string(p)=="cel"||std::string(p)=="0"?0:1;
    if(const char* p=std::getenv("GB_TIME_OF_DAY")){char* end=nullptr;const float value=std::strtof(p,&end);if(end!=p&&*end==0&&std::isfinite(value))s.timeOfDay=value;}
    s.sanitize();return s;
}
inline StylizedLightingSettings& stylized_lighting_settings(){static auto s=lighting_settings_from_environment();return s;}
}
