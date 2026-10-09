#pragma once
#include <array>
namespace game
{
    struct ocean_vec3 { float x=0,y=0,z=0; };
    struct ocean_wave { float dx,dz,wavelength,amplitude,steepness,speed; };
    struct ocean_settings
    {
        std::array<ocean_wave,4> waves = {{{1.f,.3f,23.7f,.36f,.65f,5.7f},
            {-.4f,1.f,8.13f,.126f,.55f,3.43f},{.7f,-.6f,2.73f,.028f,.45f,1.91f},{-.8f,-.2f,17.83f,.16f,.4f,4.19f}}};
        float mean_level=-.3f;
        std::array<float,3> deep_color={{.025f,.22f,.32f}},shallow_color={{.08f,.58f,.61f}},foam_color={{.94f,.98f,.94f}};
        std::array<float,3> foam_shadow_color={{.68f,.86f,.9f}};
        float foam_width=1.65f, foam_inner_distance=.3f, foam_noise_scale=.19f, foam_noise_speed=.65f;
        float foam_sharpness=2.2f, foam_outer_strength=.9f, crest_foam_strength=.1f, facet_strength=2.4f;
        float foam_mid_min_width=.35f,foam_mid_max_width=1.4f,foam_warp_strength=.65f,foam_height_offset=.04f;
        float absorption=.2f, refraction_strength=.008f, highlight_strength=.45f;
        float depth_fade_distance=6.f, refraction_depth_distance=3.f, fresnel_power=4.f, reflection_strength=.35f;
        float caustic_scale=.8f, caustic_speed=.65f, caustic_strength=.5f, caustic_depth_falloff=.15f;
        ocean_vec3 sun_direction={.35f,.8f,.45f};
        void validate() const;
    };
    struct ocean_surface_sample { ocean_vec3 position,normal; };
    ocean_surface_sample evaluate_ocean(const ocean_settings&,float rest_x,float rest_z,double seconds);
    ocean_surface_sample sample_ocean(const ocean_settings&,float world_x,float world_z,double seconds);
}
