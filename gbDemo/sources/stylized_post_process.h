#pragma once
#include <algorithm>
#include <array>
#include <cctype>
#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <string>
namespace game {
enum class StylizedPostProcessView {
  Final = 0,
  SceneHDR = 1,
  BloomBrightPass = 2, // Retired compatibility value; sanitized to Final.
  BloomResult = 3, // Retired compatibility value; sanitized to Final.
  ToneMapped = 4,
  ColorGraded = 5
};
// Runtime presentation preferences, independent of terrain edits and level files.
// Retired bloom/DOF/vignette fields remain for source and uniform compatibility.
struct StylizedPostProcessSettings {
  bool enableHDR = true, enableBloom = false, enableToneMapping = true;
  bool enableColorGrading = true, enableFog = true, enableDOF = false,
       enableVignette = false;
  bool enableSSAO = true;
  float ssaoIntensity = 1.2f;
  float bloomThreshold = 1.15f, bloomStrength = .12f;
  float exposure = .15f; // EV: the linear exposure multiplier is 2^exposure.
  float saturation = .96f, contrast = 1.05f, temperature = .015f, tint = 0;
  std::array<float, 3> shadowTint = {{.97f, 1.015f, 1.035f}};
  std::array<float, 3> highlightTint = {{1.035f, 1.015f, .98f}};
  float buildingMidtoneLift = .08f;
  std::array<float, 3> fogColor = {{.42f, .66f, .78f}}; // Linear RGB.
  float fogStart = 450, fogEnd = 6000, fogStrength = .08f;
  float focusDistance = 0, focusRange = 80, nearBlur = 1.5f, farBlur = 2;
  float vignetteStrength = .04f;
  int bloomDivisor = 2, debugView = 0, implementationStage = 12;
  static StylizedPostProcessSettings desktop() {
    return StylizedPostProcessSettings();
  }
  static StylizedPostProcessSettings mobile() {
    auto s = desktop();
    s.bloomDivisor = 4;
    s.enableDOF = false;
    return s;
  }
  float exposure_multiplier() const { return std::exp2(exposure); }
  void sanitize() {
    const auto defaults = desktop();
    auto range = [](float &value, float low, float high, float fallback) {
      value = std::isfinite(value) ? std::max(low, std::min(high, value))
                                   : fallback;
    };
    enableBloom = enableDOF = enableVignette = false;
    range(ssaoIntensity, 0, 3, defaults.ssaoIntensity);
    range(bloomThreshold, 0, 64, defaults.bloomThreshold);
    range(bloomStrength, 0, 4, defaults.bloomStrength);
    range(exposure, -16, 16, defaults.exposure);
    range(saturation, 0, 4, defaults.saturation);
    range(contrast, 0, 4, defaults.contrast);
    range(temperature, -1, 1, defaults.temperature);
    range(tint, -1, 1, defaults.tint);
    for (size_t i = 0; i < 3; ++i) {
      range(shadowTint[i], 0, 4, defaults.shadowTint[i]);
      range(highlightTint[i], 0, 4, defaults.highlightTint[i]);
      range(fogColor[i], 0, 16, defaults.fogColor[i]);
    }
    range(buildingMidtoneLift, 0, 1, defaults.buildingMidtoneLift);
    range(fogStart, 0, 1000000, defaults.fogStart);
    range(fogEnd, 1, 10000000, defaults.fogEnd);
    fogEnd = std::max(fogStart + 1, fogEnd);
    range(fogStrength, 0, 1, defaults.fogStrength);
    range(focusDistance, 0, 10000000, defaults.focusDistance);
    range(focusRange, .001f, 10000000, defaults.focusRange);
    range(nearBlur, 0, 32, defaults.nearBlur);
    range(farBlur, 0, 32, defaults.farBlur);
    range(vignetteStrength, 0, 1, defaults.vignetteStrength);
    bloomDivisor = bloomDivisor >= 4 ? 4 : 2;
    debugView = std::max(0, std::min(5, debugView));
    if (debugView == 2 || debugView == 3)
      debugView = 0;
    implementationStage = std::max(1, std::min(12, implementationStage));
  }
};
namespace stylized_post_detail {
inline std::string lowercase(const char *input) {
  std::string s = input ? input : "";
  for (auto &c : s)
    c = char(std::tolower(static_cast<unsigned char>(c)));
  return s;
}
inline bool end_of_value(const char *end) {
  while (*end && std::isspace(static_cast<unsigned char>(*end)))
    ++end;
  return *end == 0;
}
inline void boolean(const char *key, bool &output) {
  auto s = lowercase(std::getenv(key));
  if (s == "1" || s == "true" || s == "on")
    output = true;
  else if (s == "0" || s == "false" || s == "off")
    output = false;
}
inline void number(const char *key, float &output) {
  const char *s = std::getenv(key);
  if (!s || !*s)
    return;
  char *end = nullptr;
  errno = 0;
  float value = std::strtof(s, &end);
  if (end != s && end_of_value(end) && errno != ERANGE && std::isfinite(value))
    output = value;
}
inline void integer(const char *key, int &output) {
  const char *s = std::getenv(key);
  if (!s || !*s)
    return;
  char *end = nullptr;
  errno = 0;
  long value = std::strtol(s, &end, 10);
  if (end != s && end_of_value(end) && errno != ERANGE && value >= -1000000 &&
      value <= 1000000)
    output = int(value);
}
} // namespace stylized_post_detail
inline StylizedPostProcessSettings stylized_post_settings_from_environment() {
  bool mobile = false;
#if defined(__IOS__) || defined(__TVOS__)
  mobile = true;
#endif
  const auto preset =
      stylized_post_detail::lowercase(std::getenv("GB_POST_PRESET"));
  if (preset == "mobile" || preset == "stylizedislandmobile")
    mobile = true;
  else if (preset == "desktop" || preset == "stylizedisland")
    mobile = false;
  auto s = mobile ? StylizedPostProcessSettings::mobile()
                  : StylizedPostProcessSettings::desktop();
  using namespace stylized_post_detail;
  boolean("GB_SSAO", s.enableSSAO);
  number("GB_SSAO_INTENSITY", s.ssaoIntensity);
  boolean("GB_POST_HDR", s.enableHDR);
  boolean("GB_POST_BLOOM", s.enableBloom);
  boolean("GB_POST_TONEMAP", s.enableToneMapping);
  boolean("GB_POST_GRADE", s.enableColorGrading);
  boolean("GB_POST_FOG", s.enableFog);
  boolean("GB_POST_DOF", s.enableDOF);
  boolean("GB_POST_VIGNETTE", s.enableVignette);
  number("GB_POST_EXPOSURE", s.exposure);
  number("GB_POST_SATURATION", s.saturation);
  number("GB_POST_CONTRAST", s.contrast);
  number("GB_POST_BLOOM_THRESHOLD", s.bloomThreshold);
  number("GB_POST_BLOOM_STRENGTH", s.bloomStrength);
  number("GB_POST_FOG_STRENGTH", s.fogStrength);
  number("GB_POST_VIGNETTE_STRENGTH", s.vignetteStrength);
  integer("GB_POST_VIEW", s.debugView);
  integer("GB_POST_STAGE", s.implementationStage);
  const auto view = lowercase(std::getenv("GB_POST_VIEW"));
  const char *names[] = {"final",       "scenehdr",   "bloombrightpass",
                         "bloomresult", "tonemapped", "colorgraded"};
  for (int i = 0; i < 6; ++i)
    if (view == names[i])
      s.debugView = i;
  s.sanitize();
  return s;
}
} // namespace game
