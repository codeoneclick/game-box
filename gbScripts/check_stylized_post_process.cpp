#include "stylized_post_process.h"
#include "stylized_lighting_settings.h"
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
static void env(const char *key, const char *value) {
#ifdef _WIN32
  _putenv_s(key, value ? value : "");
#else
  if (value)
    setenv(key, value, 1);
  else
    unsetenv(key);
#endif
}
int main() {
  using game::StylizedPostProcessSettings;
  auto desktop = StylizedPostProcessSettings::desktop();
  auto mobile = StylizedPostProcessSettings::mobile();
  for (const auto &preset : {desktop, mobile}) {
    for (float value :
         {preset.bloomThreshold, preset.bloomStrength, preset.exposure,
          preset.saturation, preset.contrast, preset.temperature, preset.tint,
          preset.buildingMidtoneLift, preset.fogStart, preset.fogEnd,
          preset.fogStrength, preset.focusDistance, preset.focusRange,
          preset.nearBlur, preset.farBlur, preset.vignetteStrength, preset.ssaoIntensity})
      assert(std::isfinite(value));
    for (auto value : preset.shadowTint)
      assert(std::isfinite(value));
    for (auto value : preset.highlightTint)
      assert(std::isfinite(value));
    for (auto value : preset.fogColor)
      assert(std::isfinite(value));
  }
  assert(desktop.enableHDR && !desktop.enableBloom &&
         desktop.enableToneMapping && desktop.enableColorGrading &&
         desktop.enableFog && !desktop.enableVignette && !desktop.enableDOF &&
         desktop.enableSSAO && desktop.ssaoIntensity == 1.2f);
  assert(desktop.implementationStage == 12 && desktop.bloomDivisor == 2 &&
         mobile.bloomDivisor == 4 && !mobile.enableDOF);
  assert(desktop.bloomStrength == mobile.bloomStrength &&
         std::abs(desktop.exposure_multiplier() - std::pow(2.f, .15f)) < 1e-6f);
  desktop.exposure = 1;
  assert(desktop.exposure_multiplier() == 2);
  desktop.exposure = -1;
  assert(desktop.exposure_multiplier() == .5f);
  auto bad = desktop;
  bad.enableBloom = bad.enableDOF = bad.enableVignette = true;
  bad.ssaoIntensity = std::nanf("");
  bad.exposure = std::numeric_limits<float>::infinity();
  bad.bloomThreshold = std::nanf("");
  bad.fogStart = 600;
  bad.fogEnd = 100;
  bad.focusRange = 0;
  bad.saturation = -5;
  bad.fogStrength = 9;
  bad.shadowTint[0] = std::nanf("");
  bad.debugView = 99;
  bad.implementationStage = -1;
  bad.bloomDivisor = 0;
  bad.sanitize();
  assert(!bad.enableBloom && !bad.enableDOF && !bad.enableVignette);
  assert(bad.ssaoIntensity == 1.2f);
  bad.ssaoIntensity = -1;
  bad.sanitize();
  assert(bad.ssaoIntensity == 0);
  bad.ssaoIntensity = 8;
  bad.sanitize();
  assert(bad.ssaoIntensity == 3);
  for (int retired : {2, 3}) {
    bad.debugView = retired;
    bad.sanitize();
    assert(bad.debugView == 0);
  }
  assert(std::isfinite(bad.exposure_multiplier()) &&
         std::isfinite(bad.bloomThreshold) && std::isfinite(bad.shadowTint[0]));
  assert(bad.fogEnd > bad.fogStart && bad.focusRange > 0 &&
         bad.saturation >= 0 && bad.fogStrength <= 1 && bad.debugView <= 5 &&
         bad.implementationStage >= 1 && bad.bloomDivisor == 2);
  for (auto key :
       {"GB_POST_PRESET", "GB_POST_HDR", "GB_POST_BLOOM", "GB_POST_TONEMAP",
        "GB_POST_GRADE", "GB_POST_FOG", "GB_POST_DOF", "GB_POST_VIGNETTE",
        "GB_POST_EXPOSURE", "GB_POST_SATURATION", "GB_POST_CONTRAST",
        "GB_POST_BLOOM_THRESHOLD", "GB_POST_BLOOM_STRENGTH",
        "GB_POST_FOG_STRENGTH", "GB_POST_VIGNETTE_STRENGTH", "GB_POST_VIEW",
        "GB_POST_STAGE", "GB_SSAO", "GB_SSAO_INTENSITY"})
    env(key, nullptr);
  const auto startup = game::stylized_post_settings_from_environment();
#if defined(__IOS__) || defined(__TVOS__)
  assert(startup.bloomDivisor == 4);
#else
  assert(startup.bloomDivisor == 2);
#endif
  env("GB_POST_PRESET", "StylizedIslandMobile");
  assert(game::stylized_post_settings_from_environment().bloomDivisor == 4);
  env("GB_POST_PRESET", "StylizedIsland");
  assert(game::stylized_post_settings_from_environment().bloomDivisor == 2);
  env("GB_POST_PRESET", "mobile");
  env("GB_POST_HDR", "false");
  env("GB_POST_DOF", "1");
  env("GB_POST_BLOOM", "true");
  env("GB_POST_VIGNETTE", "on");
  env("GB_SSAO", "off");
  env("GB_SSAO_INTENSITY", "2.5");
  env("GB_POST_EXPOSURE", "1");
  env("GB_POST_VIEW", "ColorGraded");
  env("GB_POST_STAGE", "8");
  auto settings = game::stylized_post_settings_from_environment();
  assert(settings.bloomDivisor == 4 && !settings.enableHDR &&
         !settings.enableDOF && !settings.enableBloom && !settings.enableVignette &&
         !settings.enableSSAO && settings.ssaoIntensity == 2.5f &&
         settings.exposure_multiplier() == 2 &&
         settings.debugView == 5 && settings.implementationStage == 8);
  env("GB_POST_HDR", "malformed");
  env("GB_POST_EXPOSURE", "nan");
  env("GB_POST_SATURATION", "2junk");
  env("GB_POST_VIEW", "2");
  settings = game::stylized_post_settings_from_environment();
  assert(settings.enableHDR && settings.exposure == .15f &&
         settings.saturation == .96f && settings.debugView == 0);
  env("GB_SSAO", "TRUE");
  env("GB_SSAO_INTENSITY", "12");
  settings = game::stylized_post_settings_from_environment();
  assert(settings.enableSSAO && settings.ssaoIntensity == 3);
  env("GB_SSAO", "malformed");
  for (auto invalid : {"nan", "inf", "2junk", ""}) {
    env("GB_SSAO_INTENSITY", invalid);
    settings = game::stylized_post_settings_from_environment();
    assert(settings.enableSSAO && settings.ssaoIntensity == 1.2f);
  }
  env("GB_SSAO_INTENSITY", "-2");
  assert(game::stylized_post_settings_from_environment().ssaoIntensity == 0);
  env("GB_POST_VIEW", "BloomResult");
  assert(game::stylized_post_settings_from_environment().debugView == 0);
  for (auto view : {1, 4, 5}) {
    env("GB_POST_VIEW", std::to_string(view).c_str());
    assert(game::stylized_post_settings_from_environment().debugView == view);
  }
  using game::StylizedLightingSettings;
  const auto day=StylizedLightingSettings::soft(), sunset=StylizedLightingSettings::sunset();
  const auto light_mobile=StylizedLightingSettings::mobile();
  assert(day.mode==1 && day.wrapAmount==.35f && day.diffusePower==.8f);
  assert(day.ambientStrength==.32f && day.sunColor[0]>day.sunColor[2]);
  assert(sunset.sunIntensity>day.sunIntensity && sunset.sunColor[2]<day.sunColor[2]);
  assert(!light_mobile.enableVegetationBacklight && light_mobile.globalSpecularMultiplier<day.globalSpecularMultiplier);
  auto invalid_light=day;
  invalid_light.wrapAmount=-20;invalid_light.diffusePower=std::nanf("");
  invalid_light.sunColor[0]=std::numeric_limits<float>::infinity();
  invalid_light.sunIntensity=-3;invalid_light.ambientStrength=50;
  invalid_light.specularPower=100;invalid_light.mode=99;invalid_light.sanitize();
  assert(invalid_light.wrapAmount==.2f && invalid_light.diffusePower==day.diffusePower);
  assert(invalid_light.sunColor[0]==day.sunColor[0] && invalid_light.sunIntensity==0);
  assert(invalid_light.ambientStrength==1 && invalid_light.specularPower==32 && invalid_light.mode==1);
  env("GB_LIGHTING_PRESET","StylizedSunsetIsland");env("GB_LIGHTING_MODE","Cel");
  const auto from_env=game::lighting_settings_from_environment();
  assert(from_env.mode==0 && from_env.sunColor==sunset.sunColor);
  env("GB_LIGHTING_PRESET",nullptr);env("GB_LIGHTING_MODE",nullptr);
  auto clock=day;clock.timeOfDay=12;
  assert(clock.sun_direction()[1]>.99999f && clock.daylight()==1.f);
  clock.timeOfDay=6;const auto dawn=clock.sun_direction();
  clock.timeOfDay=18;const auto dusk=clock.sun_direction();
  assert(std::abs(dawn[1])<1e-5f && std::abs(dusk[1])<1e-5f);
  assert(dawn[0]>.7f && dusk[0]<-.7f);
  clock.timeOfDay=0;const auto midnight=clock.sun_direction();assert(clock.daylight()==0.f);
  clock.timeOfDay=24;const auto end_day=clock.sun_direction();assert(clock.daylight()==0.f);
  for(int i=0;i<3;++i)assert(std::abs(midnight[i]-end_day[i])<1e-5f);
  for(int hour=0;hour<=24;++hour){clock.timeOfDay=float(hour);const auto direction=clock.sun_direction();
    float length=0;for(float c:direction){assert(std::isfinite(c));length+=c*c;}assert(std::abs(length-1.f)<1e-5f);
    assert(clock.daylight()>=0.f&&clock.daylight()<=1.f);}
  clock.timeOfDay=std::nanf("");clock.sanitize();assert(clock.timeOfDay==10.f);
  clock.timeOfDay=-1;clock.sanitize();assert(clock.timeOfDay==0);
  clock.timeOfDay=25;clock.sanitize();assert(clock.timeOfDay==24);
  env("GB_TIME_OF_DAY","18");assert(game::lighting_settings_from_environment().timeOfDay==18);
  env("GB_TIME_OF_DAY","bad");
#if defined(__IOS__) || defined(__TVOS__)
  assert(game::lighting_settings_from_environment().timeOfDay==light_mobile.timeOfDay);
#else
  assert(game::lighting_settings_from_environment().timeOfDay==sunset.timeOfDay);
#endif
  env("GB_TIME_OF_DAY",nullptr);
  std::cout << "Stylized post-process and lighting settings CPU checks passed\n";
}
