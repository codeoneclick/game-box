//
//  File.metal
//  gbDemo
//
//  Created by serhii.s on 3/1/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "mtl_common.h"

using namespace metal;

#include "mtl_ocean.h"
#include "mtl_water_interaction.h"
#include "mtl_island_resident.h"
#include "mtl_sun_shadow.h"
vertex common_v_output_t vertex_shader_blob_shadow(common_v_input_t in [[stage_in]],constant common_u_input_t& u [[buffer(1)]]){
 common_v_output_t out={};out.position=get_mat_mvp(u)*float4(in.position,1);
 out.view_space_position=get_mat_m(u)*float4(in.position,1);out.texcoord=in.texcoord;out.color=in.color;return out;
}
fragment half4 fragment_shader_blob_shadow(common_v_output_t in [[stage_in]],texture2d<float> positions [[texture(0)]]){
 // All WS attachments use the same runtime window pixel dimensions.
 float2 uv=in.position.xy/float2(positions.get_width(),positions.get_height());
 float4 receiver=positions.sample(ocean_nearest,uv);
 if(receiver.w<=0.0||abs(receiver.y-in.view_space_position.y)>.08)discard_fragment();
 float shadow=(1.0-smoothstep(.1,1.0,length(float2(in.texcoord))))*in.color.a;
 return half4(shadow,0,0,0);
}
vertex common_v_output_t vertex_shader_editor_brush(common_v_input_t in [[stage_in]],constant common_u_input_t& u [[buffer(1)]]){
 common_v_output_t out={};float4x4 model=get_mat_m(u);
 out.position=float4(in.position.x,in.position.z,0,1);
 out.view_space_position=model*float4(0,0,0,1);
 out.texcoord=float2(length(model[0].xyz),0);return out;
}
fragment half4 fragment_shader_editor_brush(common_v_output_t in [[stage_in]],texture2d<float> positions [[texture(0)]]){
 float2 uv=in.position.xy/float2(positions.get_width(),positions.get_height());
 float4 p=positions.sample(ocean_nearest,uv);if(p.w<=0.0||in.texcoord.x<=.0001)discard_fragment();
 float distance_from_center=length(p.xz-in.view_space_position.xz)/in.texcoord.x;
 float a=(1.0-smoothstep(.015,.055,abs(distance_from_center-.94)))*.8;
 if(a<=0.0)discard_fragment();
 return half4(half3(float3(.03,.7,1)*a),half(a));
}


float2 grass_wind(float2 p,float t,float phase){
 float gust=.3+.25*(.5+.5*sin(dot(p,float2(.063,.041))-t*.71))
                 +.2*(.5+.5*sin(dot(p,float2(-.037,.079))+t*.43));
 float2 broad=float2(sin(t*.83+dot(p,float2(.091,-.053))),cos(t*.67+dot(p,float2(.047,.083))));
 float2 local=float2(sin(t*(1.61+.17*sin(phase))+phase),cos(t*(1.37+.19*cos(phase))+phase*1.73));
 float2 wind=(broad*.72+local*.28)*gust;
 return wind*min(1.0,.7/max(length(wind),.0001));
}

vertex common_v_output_t vertex_shader_island_terrain(common_v_input_t in [[stage_in]],
                                                       constant common_u_input_t& uniforms [[buffer(1)]],
                                                       constant ocean_uniforms& u [[buffer(2)]],
                                                       texture2d<float> grass_field [[texture(0)]])
{
    common_v_output_t out;
    float4 position = float4(in.position, 1.0);
    float3 normal=in.normal.xyz;
    if(in.tangent.w<-.5&&u.terrain_lod.y>.5){
        position.xz+=float2(in.texcoord)*u.terrain_lod.x;
        if(abs(normal.y)<.01)normal=normalize(mix(normal,float3(in.tangent.x,0,in.tangent.y),u.terrain_lod.x));
    }
    if(in.tangent.z>.5){
        float weight=in.tangent.x*in.tangent.x;
        float2 wind=grass_wind(float2(in.texcoord),u.time_level.x,in.tangent.y*91.7);
        int layer=clamp(int(floor((position.y-in.tangent.x*in.tangent.y)/2.0+.5)),1,4)-1;
        float2 tile=float2(layer%2,layer/2);
        float2 uv=clamp((float2(in.texcoord)-u.interaction.xy)/u.interaction.z,float2(.001),float2(.999));
        float2 push=grass_field.sample(linear_sampler,(tile+uv)*.5,level(0)).xy;
        float force=length(push);push*=min(1.0,.85/max(force,.0001));
        wind=mix(wind,wind*.2+push,smoothstep(0.0,.4,force));
        wind*=min(1.0,1.0/max(length(wind),.0001));
        wind*=in.tangent.y*.8;
        position.xz+=wind*weight;
        position.y-=dot(wind,wind)*.18*weight;
        normal=normalize(normal-float3(wind.x,0,wind.y)*weight*.45);
        // Match GLSL: a planted root and a 75-degree pose under strong contact.
        float contact=smoothstep(.04,.35,force)*weight;
        float2 direction=push/max(length(push),.0001);
        float2 side=normalize(float2(in.normal.z,-in.normal.x));
        float2 width_offset=side*dot(float2(in.position.xz)-float2(in.texcoord),side);
        float height=in.tangent.y,root_y=in.position.y-in.tangent.x*height;
        const float angle=1.308996939;
        position.xz=mix(position.xz,float2(in.texcoord)+width_offset+direction*height*sin(angle),contact);
        position.y=mix(position.y,root_y+height*cos(angle),contact);
        float3 axis=float3(direction.y,0,-direction.x);
        float3 contact_normal=in.normal.xyz*cos(angle)+cross(axis,in.normal.xyz)*sin(angle)
                             +axis*dot(axis,in.normal.xyz)*(1.0-cos(angle));
        normal=normalize(mix(normal,contact_normal,contact));
    }
    else if(in.tangent.w>.5){
        float weight=in.tangent.x*in.tangent.x;
        float2 wind=grass_wind(float2(in.texcoord),u.time_level.x*.4,in.tangent.y*91.7)*.4;
        wind+=.02*float2(sin(u.time_level.x*1.37+in.tangent.y*31.0+in.texcoord.x*.17),cos(u.time_level.x*1.12+in.tangent.y*47.0+in.texcoord.y*.19));
        position.xz+=wind*weight;
        normal=normalize(normal-float3(wind.x,0,wind.y)*weight*.12);
    }
    out.position = get_mat_mvp(uniforms) * position;
    out.view_space_position = get_mat_m(uniforms) * position;
    out.normal = normalize((get_mat_n(uniforms) * float4(normal, 0.0)).xyz);
    out.color = in.color;
    float outline_id=floor(in.color.a*255.0+.5)*16384.0+floor(in.normal.w*127.0+.5)*128.0+floor(-in.tangent.z*127.0+.5);
    out.color.a = in.tangent.z>.5 ? .5 : (in.normal.w>0.0||in.tangent.z<0.0||in.color.a<.9999?outline_id:1.0);
    out.texcoord = in.texcoord;
    if(in.tangent.z>.5){
        out.texcoord=float2(in.color.a,in.tangent.x);
    }
    if(in.tangent.w>.5){
        float3 rest=in.position;rest.xz-=in.texcoord;
        out.texcoord=abs(in.normal.y)>.5?rest.xz:(abs(in.normal.x)>abs(in.normal.z)?rest.zy:rest.xy);
    }
    out.tangent = float3(in.tangent.w<-.5?1.0:0.0, in.tangent.w>.5?1.0:(in.tangent.w>.1?2.0:0.0), fract(sin(dot(float2(in.texcoord),float2(127.1,311.7)))*43758.5453));
    out.bitangent = cross(out.normal, out.tangent);
    return out;
}

// Keep material selection tied to the constant logical terrace/outline ID.
float terrain_material(float terrace){
    if(terrace<=0.0)return 0.0;
    if(terrace<1.5)return 1.0;
    if(terrace<2.5)return 2.0;
    return 3.0;
}
float3 terrain_texel(texture2d<float> atlas,float material,float side,float2 uv,float lod){
    float resolution=32.0*exp2(-lod);
    float2 pixel=float2(material*36.0+2.0,lod*80.0+side*40.0+1.0)
                +min(floor(uv*resolution),float2(resolution-1.0));
    float2 atlas_uv=(pixel+.5)/float2(256.0,480.0);
    atlas_uv.y=1.0-atlas_uv.y; // The common PNG serializer reverses rows.
    float3 c=atlas.sample(linear_sampler,atlas_uv,level(0)).rgb;
    return mix(c/12.92,pow((c+.055)/1.055,float3(2.4)),step(float3(.04045),c));
}
float3 terrain_texture(texture2d<float> atlas,float3 p,float3 n,float terrace,float material,float layer_height,float shore_height){
    layer_height=max(layer_height,.001);
    float top_y=terrace==1.0?shore_height:(terrace>=2.0?(terrace-1.0)*layer_height:terrace*layer_height);
    float2 top_q=p.xz*2.0;
    float2 side_q=float2((abs(n.x)>abs(n.z)?p.z:p.x)*2.0,(top_y-p.y)/layer_height*4.0);
    // World pixels choose independent palette entries; no repeated block seams.
    float2 top_footprint=fwidth(top_q),side_footprint=fwidth(side_q);
    float side=n.y>.5?0.0:1.0;
    float2 footprint=mix(top_footprint,side_footprint,side);
    float2 cell=floor(top_q),seed=cell+float2(material*37.0,material*71.0);
    float a=fract(sin(dot(seed,float2(127.1,311.7)))*43758.5453);
    float b=fract(sin(dot(seed+float2(19.1,83.7),float2(127.1,311.7)))*43758.5453);
    float2 uv=(floor(float2(a,b)*4.0)+.5)/4.0;
    if(side>.5){
        float column=floor(fract(sin(dot(float2(floor(side_q.x),material+17.0),float2(127.1,311.7)))*43758.5453)*4.0);
        uv=float2((column+.5)/4.0,clamp(side_q.y/4.0,0.0,1.0));
    }
    float visible=1.0-smoothstep(.5,2.0,max(footprint.x,footprint.y));
    return mix(terrain_texel(atlas,material,side,uv,5.0),terrain_texel(atlas,material,side,uv,0.0),visible);
}

float tree_pixel_hash(float2 p){return fract(sin(dot(p,float2(127.1,311.7)))*43758.5453);}
float3 tree_pixel_color(float3 base,float2 uv){
 float2 pixel=uv*6.0,footprint=abs(dfdx(pixel))+abs(dfdy(pixel));
 float visible=1.0-smoothstep(.5,2.0,max(footprint.x,footprint.y));
 float grain=floor(tree_pixel_hash(floor(pixel))*4.0)/3.0;
 float leaves=base.g>max(base.r,base.b)?1.0:0.0;
 float stripes=floor(tree_pixel_hash(float2(floor(uv.x*4.0),floor(uv.y*2.0)))*4.0)/3.0;
 float detail=mix(.7+.3*stripes+.3*grain,.6+.8*grain,leaves);
 return base*mix(1.0,detail,visible);
}
float3 grass_pixel_color(float3 base,float2 uv){
 float2 pixel=uv*float2(4.0,8.0),footprint=abs(dfdx(pixel))+abs(dfdy(pixel));
 float visible=1.0-smoothstep(.5,2.0,max(footprint.x,footprint.y));
 float dx=dfdx(uv.y),dy=dfdy(uv.y);
 float3 slope=(dfdx(base)*dx+dfdy(base)*dy)/max(dx*dx+dy*dy,.00000001);
 float center=(floor(pixel.y)+.5)/8.0;
 float3 stepped=max(base+slope*(center-uv.y),float3(0));
 float tone=.8+.2*floor(tree_pixel_hash(floor(pixel))*3.0);
 return mix(base,stepped*tone,visible);
}
fragment g_buffer_output_t fragment_shader_island_terrain(common_v_output_t in [[stage_in]],constant ocean_uniforms& u [[buffer(0)]],texture2d<float> terrain_atlas [[texture(2)]])
{
    g_buffer_output_t out;
    if(in.color.a>.49&&in.color.a<.51){
        float x=in.tangent.z>.5?1.0-in.texcoord.x:in.texcoord.x;
        float row=floor(clamp(in.texcoord.y,0.0,.9999)*8.0);
        float column=floor(clamp(x,0.0,.9999)*4.0),left=floor(row*.5);
        if(column<left||column>=min(left+2.0,4.0))discard_fragment();
    }
    float3 normal = normalize(in.normal);
    float diffuse = max(dot(normal, normalize(u.sun.xyz)), 0.0);
    float3 p=in.view_space_position.xyz;
    float water=ocean_ocean_height(p.xz,u),submerged=max(water-p.y,0.0);
    // Only surfaces within wave reach receive instantaneous wetness.
    // Stored buoyancy wetness remains independent of the current wave envelope.
    float crest_level=u.time_level.y+1.1*(u.wave0.w+u.wave1.w+u.wave2.w+u.wave3.w);
    float reachable=1.0-smoothstep(crest_level-.05,crest_level,p.y);
    float wet=max(u.extra.y*(1.0-smoothstep(0.0,1.5,p.y-u.time_level.y)),reachable*(1.0-smoothstep(0.0,.65,p.y-water)));
    bool terrain=in.tangent.x>.5;
    float terrace=floor(in.color.a+.5)-11.0,material=terrain_material(terrace);
    float3 base_color=terrain?terrain_texture(terrain_atlas,p,normal,terrace,material,u.extra.z,u.extra.w):float3(in.color.rgb);
    if(in.tangent.y>1.5){
        float2 pixels=in.texcoord*5.0,footprint=abs(dfdx(pixels))+abs(dfdy(pixels));
        float2 uv=(fmod(fmod(floor(pixels),float2(8.0))+8.0,float2(8.0))+.5)/8.0;
        float3 stone=terrain_texel(terrain_atlas,0.0,0.0,uv,0.0);
        float grain=clamp(dot(stone,float3(.2126,.7152,.0722))/.6,.65,1.2);
        base_color*=mix(grain,.9,smoothstep(.5,2.0,max(footprint.x,footprint.y)));
    }else if(in.tangent.y>.5)base_color=tree_pixel_color(base_color,in.texcoord);
    if(in.color.a>.49&&in.color.a<.51)base_color=grass_pixel_color(base_color,in.texcoord);
    float3 l=normalize(u.sun.xyz),v=normalize(u.camera.xyz-p);
    float grass=in.color.a>.49&&in.color.a<.51?1.0:0.0,leaves=in.tangent.y>.5&&base_color.g>max(base_color.r,base_color.b)?1.0:0.0;
    float strength=base_color.r>base_color.b*1.25&&base_color.r>base_color.g*1.1?u.light_material0.y:u.light_material0.z;
    if(in.tangent.y>1.5)strength=u.light_material0.z;
    strength=mix(strength,u.light_material0.x,grass);strength=mix(strength,u.light_material0.w,leaves);
    float vegetation=max(grass,leaves);
    if(terrain){
        float grassy_top=normal.y>.5&&material>=2.0&&material<=5.0?1.0:0.0;
        strength=mix(u.light_material0.z,u.light_material0.x,grassy_top);vegetation=grassy_top;
    }
    float3 color=u.light_shape.z<.5?base_color*mix(float3(.32,.38,.50),cel_light(dot(normal,l)),u.light_options.w):island_soft_light(base_color,normal,l,v,strength,vegetation,u.light_sun,u.light_ambient,u.light_shape,u.light_options,u.light_material1);
    color*=mix(1.0,.64,wet);
    if(submerged>0.0&&u.caustic_params.z>0.0){float caustics=ocean_caustic_pattern(p.xz*u.caustic_params.x,u)*max(normal.y,.12)+ocean_caustic_pattern(float2(p.x+p.z,p.y)*u.caustic_params.x,u)*(1.0-abs(normal.y))*.4;color+=float3(.36,.54,.39)*caustics*u.caustic_params.z*exp(-submerged*u.caustic_params.w);}
    float3 half_direction=v+l,h=half_direction/max(length(half_direction),.0001);
    if(u.light_shape.z<.5)color+=wet*.2*u.light_options.w*smoothstep(.94,.955,max(dot(normal,h),0.0))*float3(1,.94,.82);
    else color+=wet*.2*pow(max(dot(normal,h),0.0),u.light_material1.z)*max(dot(normal,l),0.0)*u.light_sun.rgb*u.light_sun.a*u.light_shape.w*u.light_options.y;
    if(terrain&&u.terrain_lod.z>.5){
        float r=clamp(u.terrain_lod.w,0.0,1.0);
        color=r<.5?mix(float3(.05,.85,.12),float3(1,.85,.05),r*2):mix(float3(1,.85,.05),float3(1,.08,.03),(r-.5)*2);
        if(u.terrain_lod.z>3.5)color=float3(.12,.18,.2);else if(u.terrain_lod.z>2.5)color=float3(r,.2,1-r);
    }
    out.color = half4(half3(color),1);
    out.normal = float4(normal*.5+.5, in.color.a>=2.0?floor(in.color.a+.5):in.color.a);
    out.position = in.view_space_position;
    out.position.w = max(-(u.view*float4(p,1)).z,.001);
    return out;
}

//

vertex common_v_output_t vertex_shader_grass_interaction(common_v_input_t in [[stage_in]]){
 common_v_output_t out;
 out.position=float4(in.position.xy,0,1);out.texcoord=in.texcoord;
 out.color=in.color;out.normal=in.normal.xyz;return out;
}
fragment half4 fragment_shader_grass_interaction(common_v_output_t in [[stage_in]]){
 float2 q=float2(in.texcoord);float r=length(q);
 float fade=1.0-smoothstep(.15,1.0,r);float2 radial=q/max(r,.18);
 return half4(half2((radial*.75+in.normal.xz*.3)*fade*in.color.a),0,0);
}

vertex common_v_output_t vertex_shader_island_sky(common_v_input_t in [[stage_in]],constant ocean_uniforms& u [[buffer(2)]]){
 common_v_output_t out;out.position=float4(in.position.xy,1,1);
 float3 ray=float3((in.position.xy+float2(u.projection[2][0],u.projection[2][1]))/float2(u.projection[0][0],u.projection[1][1]),-1);
 float3x3 rotation=float3x3(u.view[0].xyz,u.view[1].xyz,u.view[2].xyz);
 out.normal=transpose(rotation)*ray;return out;
}
fragment g_buffer_output_t fragment_shader_island_sky(common_v_output_t in [[stage_in]],constant ocean_uniforms& u [[buffer(0)]],texturecube<float> sky [[texture(1)]]){
 g_buffer_output_t out;float3 lookup=normalize(in.normal);
 lookup.xz=float2(u.sky_rotation.x*lookup.x-u.sky_rotation.y*lookup.z,u.sky_rotation.y*lookup.x+u.sky_rotation.x*lookup.z);
 float3 c=sky.sample(linear_sampler,lookup).rgb;
 c=mix(c/12.92,pow((c+.055)/1.055,float3(2.4)),step(float3(.04045),c));
 out.color=half4(half3(c),1);out.normal=half4(0);out.position=float4(0);return out;
}

half4 blur_texture(texture2d<half> texture,
                   float2 texcoord, float2 direction)
{
    float2 resolution = float2(texture.get_width(), texture.get_height());
    half4 color = texture.sample(linear_sampler, texcoord) * 0.2270270270;
    float2 offset_1 = 1.3846153846 * direction;
    float2 offset_2 = 3.2307692308 * direction;
    color += texture.sample(linear_sampler, texcoord + (offset_1 / resolution)) * 0.3162162162;
    color += texture.sample(linear_sampler, texcoord - (offset_1 / resolution)) * 0.3162162162;
    color += texture.sample(linear_sampler, texcoord + (offset_2 / resolution)) * 0.0702702703;
    color += texture.sample(linear_sampler, texcoord - (offset_2 / resolution)) * 0.0702702703;
    return color;
}

//

vertex common_v_output_t vertex_shader_ss_deferred_lighting(common_v_input_t in [[stage_in]],
                                                            constant common_u_input_t& uniforms [[ buffer(1) ]])
{
    common_v_output_t out;
    
    float4 in_position = float4(float3(in.position), 1.0);
    out.position = in_position;
    out.texcoord = (float2)in.texcoord;
    
    return out;
}

fragment half4 fragment_shader_ss_deferred_lighting(common_v_output_t in [[stage_in]],
                                                     texture2d<half> opaque_color_texture [[texture(0)]],
                                                     texture2d<float> transparent_color_texture [[texture(1)]],
                                                     texture2d<half> lighting_texture [[texture(2)]],
                                                     texture2d<half> mask_texture [[texture(3)]])
{
    half4 opaque_color = opaque_color_texture.sample(linear_sampler, in.texcoord);
    half4 transparent_color = (half4)transparent_color_texture.sample(linear_sampler, in.texcoord);
    half4 color = opaque_color + transparent_color;
    half4 mask = mask_texture.sample(linear_sampler, in.texcoord);
    
    half4 lighting = lighting_texture.sample(linear_sampler, in.texcoord);
    color.rbg = color.rbg * clamp(lighting.rbg, half3(mask.r * .15f), half3(.999f));
    return color;
}


//

vertex common_v_output_t vertex_shader_ss_hdr(common_v_input_t in [[stage_in]],
                                                 constant common_u_input_t& uniforms [[ buffer(1) ]])
{
    common_v_output_t out;
    
    float4 in_position = float4(float3(in.position), 1.0);
    out.position = in_position;
    out.texcoord = (float2)in.texcoord;
    
    return out;
}

fragment half4 fragment_shader_ss_hdr(common_v_output_t in [[stage_in]],
                                         texture2d<half> color_texture [[texture(0)]])
{
    float3 hdr_color = (float3)color_texture.sample(linear_sampler, in.texcoord).rgb;
    const float3 LUM_FACTOR = float3(0.299, 0.587, 0.114);
    float l_scale = dot(hdr_color, LUM_FACTOR);
    hdr_color *= l_scale;
    return half4((half3)hdr_color, 1.0);;
}

//

vertex common_v_output_t vertex_shader_ss_gaussian_blur_h(common_v_input_t in [[stage_in]],
                                                          constant common_u_input_t& uniforms [[ buffer(1) ]])
{
    common_v_output_t out;
    
    float4 in_position = float4(float3(in.position), 1.0);
    out.position = in_position;
    out.texcoord = (float2)in.texcoord;
    
    return out;
}

fragment half4 fragment_shader_ss_gaussian_blur_h(common_v_output_t in [[stage_in]],
                                                  texture2d<half> hdr_texture [[texture(0)]])
{
    float width = hdr_texture.get_width();
    half4 color = blur_texture(hdr_texture, in.texcoord,
                                float2(1.f, .0f));
    return color;
}

//

vertex common_v_output_t vertex_shader_ss_gaussian_blur_v(common_v_input_t in [[stage_in]],
                                                          constant common_u_input_t& uniforms [[ buffer(1) ]])
{
    common_v_output_t out;
    
    float4 in_position = float4(float3(in.position), 1.0);
    out.position = in_position;
    out.texcoord = (float2)in.texcoord;
    
    return out;
}

fragment half4 fragment_shader_ss_gaussian_blur_v(common_v_output_t in [[stage_in]],
                                                  texture2d<half> hdr_texture [[texture(0)]])
{
    float height = hdr_texture.get_height();
    half4 color = blur_texture(hdr_texture, in.texcoord,
                                float2(0.f, 1.f));
    return color;
}

//

vertex common_v_output_t vertex_shader_screen_quad(common_v_input_t in [[stage_in]],
                                                   constant common_u_input_t& uniforms [[ buffer(1) ]])
{
    common_v_output_t out;
    
    float4 in_position = float4(float3(in.position), 1.0);
    out.position = in_position;
    out.texcoord = (float2)in.texcoord;
    
    return out;
}

fragment half4 fragment_shader_screen_quad(common_v_output_t in [[stage_in]],
                                           texture2d<half> color_texture [[texture(0)]])
{
    float4 color = (float4)color_texture.sample(linear_sampler, in.texcoord);
    
    return half4(color);
}

//

vertex common_v_output_t vertex_shader_ss_compose(common_v_input_t in [[stage_in]],
                                                 constant common_u_input_t& uniforms [[ buffer(1) ]])
{
    common_v_output_t out;
    
    float4 in_position = float4(float3(in.position), 1.0);
    out.position = in_position;
    out.texcoord = (float2)in.texcoord;
    
    return out;
}

fragment half4 fragment_shader_ss_compose(common_v_output_t in [[stage_in]],
                                          constant ss_output_u_input_t& uniforms [[ buffer(1) ]],
                                          texture2d<half> color_texture [[texture(0)]],
                                          texture2d<half> ao_texture [[texture(1)]],
                                          texture2d<half> emissive_texture [[texture(2)]],
                                          texture2d<half> mask_texture [[texture(3)]],
                                          texture2d<half> bloom_texture [[texture(4)]])
{
    const float bloom_intensity = 2.f;
    const float original_intensity = 2.f;
    const float bloom_saturation = .75f;
    const float original_saturation = 1.f;
    
    float2 motion_direction = uniforms.motion_direction.xy * 3.f;
    float motion_blur_power = uniforms.motion_direction.z;

    float4 color = (float4)color_texture.sample(linear_sampler, in.texcoord);
    if (motion_blur_power > 0.f)
    {
        float4 motion_color = float4(0.f, 0.f, 0.f, 1.f);
        float2 texcoord = in.texcoord;
        for (int i = 0; i < 4; i++)
        {
            texcoord.x = texcoord.x - 0.001 * motion_direction.x * motion_blur_power;
            texcoord.y = texcoord.y - 0.001 * motion_direction.y * motion_blur_power;
            
            float4 mask_color = (float4)mask_texture.sample(linear_sampler, texcoord);
            
            motion_color += mix((float4)color_texture.sample(linear_sampler, texcoord), color, 1.0f - mask_color.r);
        }
        motion_color /= 4;
        float4 mask_color = (float4)mask_texture.sample(linear_sampler, in.texcoord);
        color = mix(motion_color, color, mask_color.r);
    }
    
    float4 bloom = (float4)bloom_texture.sample(linear_sampler, in.texcoord);
    float ao = (float)ao_texture.sample(linear_sampler, in.texcoord).r;
    float4 emissive = (float4)emissive_texture.sample(linear_sampler, in.texcoord);
    
    bloom = adjust_saturation(bloom, bloom_saturation) * bloom_intensity;
    color = adjust_saturation(color, original_saturation) * original_intensity;
    color *= ao;
    color += bloom;
    color += emissive;
    
    if (uniforms.parameters_01.y > 0.f)
    {
        float3 sepia_color;
        sepia_color.r = (color.r * 0.393) + (color.g * 0.769) + (color.b * 0.189);
        sepia_color.g = (color.r * 0.349) + (color.g * 0.686) + (color.b * 0.168);
        sepia_color.b = (color.r * 0.272) + (color.g * 0.534) + (color.b * 0.131);
        float4 mask_color = (float4)mask_texture.sample(linear_sampler, in.texcoord);
        if (mask_color.r != 1.f)
        {
            color.rgb = mix(color.rbg, sepia_color, (1.f - mask_color.r) * uniforms.parameters_01.y);
        }
    }
    
    float vignetting_size = uniforms.parameters_01.x;
    float edge = 0.1;
    float2 direction = max(abs(in.texcoord * 2.0 - 1.0) * (1.0 + edge) - edge + vignetting_size, float2(0.f));
    float vignetting_value = dot(direction, direction);
    vignetting_value = 1.f - vignetting_value / (1.f + vignetting_value);
    color.rgb += (1.f - vignetting_value) * uniforms.vignetting_color.rbg;
    
    return half4(color);
}

//

vertex common_v_output_t vertex_shader_ss_compose_ui(common_v_input_t in [[stage_in]],
                                                     constant common_u_input_t& uniforms [[ buffer(1) ]])
{
    common_v_output_t out;
    
    float4 in_position = float4(float3(in.position), 1.0);
    out.position = in_position;
    out.texcoord = (float2)in.texcoord;
    
    return out;
}

fragment half4 fragment_shader_ss_compose_ui(common_v_output_t in [[stage_in]],
                                             texture2d<half> color_texture [[texture(0)]],
                                             texture2d<half> ui_texture [[texture(1)]])
{
    float4 color = (float4)color_texture.sample(linear_sampler, in.texcoord);
    float4 ui = (float4)ui_texture.sample(linear_sampler, in.texcoord);
    color = mix(color, ui, ui.a);
    return half4(color);
}


//

vertex common_v_output_t vertex_shader_ss_output(common_v_input_t in [[stage_in]],
                                                 constant common_u_input_t& uniforms [[ buffer(1) ]])
{
    common_v_output_t out;
    
    float4 in_position = float4(float3(in.position), 1.0);
    out.position = in_position;
    out.texcoord = (float2)in.texcoord;
    
    return out;
}

fragment half4 fragment_shader_ss_output(common_v_output_t in [[stage_in]],
                                         texture2d<half> color_texture [[texture(0)]])
{
    float4 color = (float4)color_texture.sample(linear_sampler, in.texcoord);
    return half4(color);
}

//

vertex common_v_output_t vertex_shader_ss_tv(common_v_input_t in [[stage_in]],
                                             constant common_u_input_t& uniforms [[ buffer(1) ]])
{
    common_v_output_t out;
    
    float4 in_position = float4(float3(in.position), 1.0);
    out.position = in_position;
    out.texcoord = (float2)in.texcoord;
    
    return out;
}

fragment half4 fragment_shader_ss_tv(common_v_output_t in [[stage_in]],
                                     constant ss_tv_u_input_t& uniforms [[ buffer(1) ]],
                                     texture2d<half> color_texture [[texture(0)]])
{
    float4 color;
    float2 uv = in.texcoord;
    
    if (uniforms.enabled > 0.0)
    {
        float time = uniforms.time * 0.001;
        
        float distance_to_center = length(uv - float2(0.5, 0.5));
        float blur = 0.0;
        blur = (1.0 + sin(time * 6.0)) * 0.5;
        blur *= 1.0 + sin(time * 16.0) * 0.5;
        blur = pow(blur, 3.0);
        blur *= 0.05;
        blur *= distance_to_center;
        
        color.r = (float)color_texture.sample(linear_sampler, float2(uv.x + blur,uv.y)).r;
        color.g = (float)color_texture.sample(linear_sampler, uv).g;
        color.b = (float)color_texture.sample(linear_sampler, float2(uv.x - blur,uv.y)).b;
        float scanline = sin(uv.y * 768.0) * 0.04;
        color -= scanline;
        color *= 1.0 - distance_to_center * 0.5;
        color.a = 1.0;
    }
    else
    {
        color = (float4)color_texture.sample(linear_sampler, uv);
    }
    
    return half4(color);
}

//

vertex common_v_output_t vertex_shader_ss_cross_fade(common_v_input_t in [[stage_in]],
                                                     constant common_u_input_t& uniforms [[ buffer(1) ]])
{
    common_v_output_t out;
    
    float4 in_position = float4(float3(in.position), 1.0);
    out.position = in_position;
    out.texcoord = (float2)in.texcoord;
    
    return out;
}

fragment half4 fragment_shader_ss_cross_fade(common_v_output_t in [[stage_in]],
                                             constant ss_cross_fade_u_input_t& uniforms [[ buffer(1) ]],
                                             texture2d<half> color_texture [[texture(0)]])
{
    float4 color = float4(0.f, 0.f, 0.f, 1.0);
    float2 uv = in.texcoord;
    
    if (uniforms.enabled > 0.0)
    {
        float progress = uniforms.progress;
        float2 center = float2(linear_ease(0.5, 0.0, 1.0, progress),0.5);
        float strength = sinusoidal_ease_in_out(0.0, 1.0, 1.0, progress);
        
        float max_weight = 0.0;
        float2 to_center = center - uv;
        float offset = rand(float2(uv.y * progress));
        
        for (float i = 0.0; i <= 20.0; i++)
        {
            float percent = (i + offset) / 20.0;
            float weight = 1.0 * (percent - percent * percent);
            color += mix((float4)color_texture.sample(linear_sampler, uv + to_center * percent * strength), float4(0.0, 0.0, 0.0, 1.0), progress) * weight;
            max_weight += weight;
        }
        
        color = color / max_weight;
        color.a = 1.0;
    }
    else
    {
        color = (float4)color_texture.sample(linear_sampler, uv);
    }
    
    return half4(color);
}

//

vertex common_v_output_t vertex_shader_shape_3d(common_v_input_t in [[stage_in]],
                                                constant common_u_input_t& uniforms [[buffer(1)]])
{
    common_v_output_t out;
    
    float4 in_position = float4(in.position, 1.0);
    float4x4 mvp = get_mat_mvp(uniforms);
    out.position = mvp * in_position;
    out.view_space_position = get_mat_m(uniforms) * in_position;
    out.texcoord = in.texcoord;
    
    float4x4 mat_n = get_mat_n(uniforms);
    float3 normal = normalize((mat_n * in.normal).xyz);
    out.normal = normal;
    float3 tangent = normalize(mat_n * in.tangent).xyz;
    out.tangent = tangent;
    float3 bitangent = normalize(cross(-normal, tangent));
    out.bitangent = bitangent;
    
    return out;
}

fragment g_buffer_output_t fragment_shader_shape_3d(common_v_output_t in [[stage_in]],
                                                    texture2d<half> diffuse_texture [[texture(0)]],
                                                    texture2d<half> normal_texture [[texture(1)]])
{
    g_buffer_output_t out;
    half4 color = diffuse_texture.sample(repeat_sampler, in.texcoord);
    out.color = color;
    
#if defined(TBN)
    
    float3x3 mat_tbn = float3x3(in.tangent, in.bitangent, in.normal);
    float3 normal_color = (float3)normal_texture.sample(repeat_sampler, in.texcoord).rgb * 2.0 - 1.0;
    
    out.normal = half4(half3(in.normal), 1.0);
    float3 normal_tbn = mat_tbn * normal_color;
    
#else
    
    out.normal = half4(half3(in.normal), 1.0);
    
#endif
    
    out.position = in.view_space_position;
    out.position.w = in.position.z;
    
    return out;
}

//

vertex reflect_v_output_t vertex_shader_shape_3d_reflect(common_v_input_t in [[stage_in]],
                                                         constant common_u_input_t& uniforms [[buffer(1)]],
                                                         constant reflect_u_input_t& reflect_uniforms [[buffer(2)]])
{
    reflect_v_output_t out;
    
    float4 in_position = float4(in.position, 1.0);
    float4x4 mvp = get_mat_mvp(uniforms);
    out.position = mvp * in_position;
    out.position_m = get_mat_m(uniforms) * in_position;
    out.texcoord = in.texcoord;
    
    float4x4 mat_n = get_mat_n(uniforms);
    float4 normal = float4(in.normal.xyz, 0.f);
    float4 tangent = float4(in.tangent.xyz, 0.f);
    float4 bitangent = float4(cross(-normal.xyz, tangent.xyz), 0.f);
    
    float3 normal_vm = normalize((get_mat_v(uniforms) * get_mat_m(uniforms) * normal).xyz);
    out.normal_vm = normal_vm;
    float3 position_vm = (get_mat_v(uniforms) * get_mat_m(uniforms) * in_position).xyz;
    out.camera_direction_vm = normalize(-position_vm);
    
    normal = mat_n * normal;
    tangent = mat_n * tangent;
    bitangent = mat_n * bitangent;
    
    out.normal = normalize(normal.xyz);
    out.tangent = normalize(tangent.xyz);
    out.bitangent = normalize(bitangent.xyz);

    return out;
}

fragment g_buffer_output_t fragment_shader_shape_3d_reflect(reflect_v_output_t in [[stage_in]],
                                                            constant colorization_u_input_t& colorization_uniforms [[buffer(0)]],
                                                            texture2d<half> diffuse_texture [[texture(0)]],
                                                            texturecube<half> reflection_texture [[texture(1)]],
                                                            texture2d<half> color_body_mask_texture [[texture(2)]],
                                                            texture2d<half> color_windows_mask_texture [[texture(3)]],
                                                            texture2d<half> normal_texture [[texture(4)]])
{
    g_buffer_output_t out;
    
    float3 reflection_dir = reflect(in.camera_direction_vm, in.normal_vm);
    const half4 reflection_color = reflection_texture.sample(linear_sampler, reflection_dir);
    
    half4 color = diffuse_texture.sample(linear_sampler, in.texcoord);
    const half3 body_color = static_cast<half3>(colorization_uniforms.body_color.rgb);
    const half4 color_body_mask = color_body_mask_texture.sample(linear_sampler, in.texcoord);
    if (color_body_mask.a > .5f)
    {
        color.rgb *= body_color * color_body_mask.a;
    }
    const half4 color_windows_mask = color_windows_mask_texture.sample(linear_sampler, in.texcoord);
    const half3 windows_color = static_cast<half3>(colorization_uniforms.windows_color.rgb);
    color.rgb = mix(color.rgb, windows_color, color_windows_mask.a);
    
    out.color = mix(pow(reflection_color, 2), color, .66f - color_windows_mask * .33f);
    out.color.a = 1.f;
    out.position = in.position_m;
    out.position.w = in.position.z;
    
    float4 normal_color = (float4)normal_texture.sample(linear_sampler, in.texcoord);
    half3 tbn_normal = (half3)normalize((normal_color.xyz * 2.f) - 1.f);
    
    tbn_normal = (tbn_normal.x * (half3)in.tangent +
                  tbn_normal.y * (half3)in.bitangent +
                  tbn_normal.z * (half3)in.normal);
    
    tbn_normal = normalize(tbn_normal);
    out.normal = half4(tbn_normal, .0f);
    
    const half rim_power = 8.f;
    const half rim = smoothstep(.7f , 1.f , 1.f - max(dot(in.camera_direction_vm, in.normal_vm), 0.f));
    out.color.rgb += rim * out.color.rgb * rim_power;
    
    return out;
}

//

vertex common_v_output_t vertex_shader_particle_emitter_emissive(common_v_input_t in [[stage_in]],
                                                                 constant common_u_input_t& uniforms [[buffer(1)]])
{
    common_v_output_t out;
    
    float4 in_position = float4(in.position, 1.0);
    float4x4 mvp = get_mat_p(uniforms) * get_mat_v(uniforms) * float4x4(1.0);
    out.position = mvp * in_position;
    out.texcoord = in.texcoord;
    out.color = in.color;
    
    return out;
}

fragment half4 fragment_shader_particle_emitter_emissive(common_v_output_t in [[stage_in]],
                                                         texture2d<half> emissive_texture [[texture(0)]])
{
    half4 color = (half4)in.color;
    color.a = emissive_texture.sample(linear_sampler, in.texcoord).a;
    
    return color;
}

//

vertex common_v_output_t vertex_shader_label_3d_emissive(common_v_input_t in [[stage_in]],
                                                         constant common_u_input_t& uniforms [[buffer(1)]])
{
    common_v_output_t out;
    
    float4 in_position = float4(in.position, 1.0);
    float4x4 mvp = get_mat_mvp(uniforms);
    out.position = mvp * in_position;
    out.texcoord = in.texcoord;
    out.color = in.color;
    
    return out;
}

fragment half4 fragment_shader_label_3d_emissive(common_v_output_t in [[stage_in]],
                                                 texture2d<half> emissive_texture [[texture(0)]])
{
    half emissive = emissive_texture.sample(linear_sampler, in.texcoord).r;
    
    return half4(in.color.r, in.color.g, in.color.b, emissive * in.color.a);
}

//

vertex common_v_output_t vertex_shader_label_2d(common_v_input_t in [[stage_in]],
                                                constant common_u_input_t& uniforms [[buffer(1)]])
{
    common_v_output_t out;
    
    float4 in_position = float4(in.position, 1.0);
    float4x4 mvp = get_mat_mvp(uniforms);
    out.position = mvp * in_position;
    out.texcoord = in.texcoord;
    out.color = in.color;
    
    return out;
}

fragment half4 fragment_shader_label_2d(common_v_output_t in [[stage_in]],
                                        texture2d<half> emissive_texture [[texture(0)]])
{
    half emissive = emissive_texture.sample(linear_sampler, in.texcoord).r;
    
    return half4(in.color * emissive);
}

//

vertex common_v_output_t vertex_shader_shape_2d_color(common_v_input_t in [[stage_in]],
                                                      constant common_u_input_t& uniforms [[buffer(1)]])
{
    common_v_output_t out;
    
    float4 in_position = float4(in.position, 1.0);
    float4x4 mvp = get_mat_mvp(uniforms);
    out.position = mvp * in_position;
    out.color = in.color;
    
    return out;
}

fragment half4 fragment_shader_shape_2d_color(common_v_output_t in [[stage_in]])
{
    return half4(in.color);
}

//

vertex common_v_output_t vertex_shader_shape_2d_texture_color(common_v_input_t in [[stage_in]],
                                                              constant common_u_input_t& uniforms [[buffer(1)]])
{
    common_v_output_t out;
    
    float4 in_position = float4(in.position, 1.0);
    float4x4 mvp = get_mat_mvp(uniforms);
    out.position = mvp * in_position;
    out.color = in.color;
    out.texcoord = in.texcoord;
    
    return out;
}

fragment half4 fragment_shader_shape_2d_texture_color(common_v_output_t in [[stage_in]],
                                                      texture2d<half> diffuse_texture [[texture(0)]])
{
    float4 color = float4(in.color.rbg, 1.0);
    color.a = (float)diffuse_texture.sample(linear_sampler, in.texcoord).a * in.color.a;
    return half4(color);
}

//

vertex common_v_output_t vertex_shader_shape_2d_texture(common_v_input_t in [[stage_in]],
                                                        constant common_u_input_t& uniforms [[buffer(1)]])
{
    common_v_output_t out;
    
    float4 in_position = float4(in.position, 1.0);
    float4x4 mvp = get_mat_mvp(uniforms);
    out.position = mvp * in_position;
    out.color = in.color;
    out.texcoord = in.texcoord;
    
    return out;
}

fragment half4 fragment_shader_shape_2d_texture(common_v_output_t in [[stage_in]],
                                                texture2d<half> diffuse_texture [[texture(0)]])
{
    half4 color = diffuse_texture.sample(linear_sampler, in.texcoord);
    return color;
}

//

vertex common_v_output_t vertex_shader_particle_emitter(common_v_input_t in [[stage_in]],
                                                        constant common_u_input_t& uniforms [[buffer(1)]])
{
    common_v_output_t out;
    
    float4 in_position = float4(in.position, 1.0);
    float4x4 mvp = get_mat_p(uniforms) * get_mat_v(uniforms) * float4x4(1.0);
    out.position = mvp * in_position;
    out.view_space_position = in_position;
    out.texcoord = in.texcoord;
    out.color = in.color;
    out.normal = (get_mat_m(uniforms) * in.normal).xyz;
    
    return out;
}

fragment float4 fragment_shader_particle_emitter(common_v_output_t in [[stage_in]],
                                                 texture2d<half> diffuse_texture [[texture(0)]],
                                                 texture2d<float> position_texture [[texture(1)]])
{
    uint2 screen_space_position = uint2(in.position.xy);
    float4 position = position_texture.read(screen_space_position);
    half depth = position.w;
    
    half4 color = half4(half3(in.color.rbg), 1.0);
    color.a = diffuse_texture.sample(linear_sampler, in.texcoord).a * in.color.a;
    half weight = (depth - in.position.w) / 1.66;
    return float4(float3(color.rgb * color.a), color.a * weight);
}

//

vertex common_v_output_t vertex_shader_label(common_v_input_t in [[stage_in]],
                                                        constant common_u_input_t& uniforms [[buffer(1)]])
{
    common_v_output_t out;
    
    float4 in_position = float4(in.position, 1.0);
    float4x4 mvp = get_mat_mvp(uniforms);
    out.position = mvp * in_position;
    out.view_space_position = in_position;
    out.texcoord = in.texcoord;
    out.color = in.color;
    out.normal = (get_mat_m(uniforms) * in.normal).xyz;
    
    return out;
}

fragment float4 fragment_shader_label(common_v_output_t in [[stage_in]],
                                     texture2d<half> diffuse_texture [[texture(0)]])
{
    float4 color = float4(float3(4.f), diffuse_texture.sample(linear_sampler, in.texcoord).r);
    return color;
}

//

vertex common_v_output_t vertex_shader_trail(common_v_input_t in [[stage_in]],
                                             constant common_u_input_t& uniforms [[buffer(1)]])
{
    common_v_output_t out;
    
    float4 in_position = float4(in.position, 1.0);
    float4x4 mvp = get_mat_p(uniforms) * get_mat_v(uniforms) * float4x4(1.0);
    out.position = mvp * in_position;
    out.view_space_position = in_position;
    out.texcoord = in.texcoord;
    out.color = in.color;
    
    float3 normal = normalize(in.normal.xyz);
    out.normal = normal;
    float3 tangent = normalize(in.tangent.xyz);
    out.tangent = tangent;
    float3 bitangent = normalize(cross(-normal, tangent));
    out.bitangent = bitangent;
    
    return out;
}

fragment float4 fragment_shader_trail(common_v_output_t in [[stage_in]],
                                                   texture2d<half> diffuse_texture [[texture(0)]],
                                                   texture2d<float> position_texture [[texture(1)]])
{
    uint2 screen_space_position = uint2(in.position.xy);
    half depth = position_texture.read(screen_space_position).w;
    
    half4 color = half4(half3(in.color.rbg * -1.f), 0.0);
    color.a = diffuse_texture.sample(linear_sampler, in.texcoord).a * in.color.a;
    half weight = (depth - in.position.w) / 1.66;
    return float4(float3(color.rgb * color.a), color.a * weight);
}

//

vertex common_v_output_t vertex_shader_arrow(common_v_input_t in [[stage_in]],
                                             constant common_u_input_t& uniforms [[buffer(1)]])
{
    common_v_output_t out;
    
    float4 in_position = float4(in.position, 1.0);
    float4x4 mvp = get_mat_mvp(uniforms);
    out.position = mvp * in_position;
    out.view_space_position = in_position;
    out.texcoord = in.texcoord;
    out.color = in.color;
    
    return out;
}

fragment half4 fragment_shader_arrow(common_v_output_t in [[stage_in]],
                                                   texture2d<half> emissive_texture [[texture(0)]])
{
    half emissive = emissive_texture.sample(linear_sampler, in.texcoord).a;
    
    return half4(in.color.r, in.color.g, in.color.b, emissive);
}

//

vertex common_v_output_t vertex_shader_deferred_point_light(common_v_input_t in [[stage_in]],
                                                            constant common_u_input_t& mvp_uniforms [[buffer(1)]])
{
    common_v_output_t out;
    
    float4 in_position = float4(in.position, 1.0);
    float4x4 mvp = get_mat_mvp(mvp_uniforms);
    out.position = mvp * in_position;
    
    return out;
}

fragment half4 fragment_shader_deferred_point_light(common_v_output_t in [[stage_in]],
                                                    constant point_light_u_input_t& custom_uniforms [[buffer(0)]],
                                                    texture2d<half> normal_texture [[texture(0)]],
                                                    texture2d<float> position_texture [[texture(1)]])
{
    const uint2 screen_space_position = uint2(in.position.xy);
    const float4 position = position_texture.read(screen_space_position);
    const half4 normal = normal_texture.read(screen_space_position);
    
    const half min_value = 0.f;
    const half light_range = static_cast<half>(custom_uniforms.light_position_and_ray_length.w);
    const half3 light_vector = static_cast<half3>(custom_uniforms.light_position_and_ray_length.xyz - position.xyz);
    const half light_distance = length(light_vector);
    
    half4 color = half4(0.f);
    if (light_distance < light_range)
    {
        const half3 light_direction = normalize(light_vector);
        const half intensity = max(dot(light_direction, normal.xyz), min_value);
        half attenuation = 1.f - (light_distance / light_range);
        attenuation *= attenuation;
        const half4 light_color = static_cast<half4>(custom_uniforms.light_color);
        const half specular_power = 16.0;
        const half specular_intensity = intensity;
        const half3 reflection_vector = normalize(reflect(-light_direction, normal.xyz));
        const half3 camera_direction = normalize(static_cast<half3>(custom_uniforms.camera_position.xyz - position.xyz));
        half4 specular = light_color * specular_intensity * pow(max(dot(reflection_vector, camera_direction), min_value), specular_power);
        specular *= attenuation * normal.w;
        color = ((light_color * intensity) + specular) * attenuation;
    }
    return color;
}

//

vertex common_v_output_t vertex_shader_stencil_deferred_point_light(common_v_input_t in [[stage_in]],
                                                                    constant common_u_input_t& mvp_uniforms [[buffer(1)]])
{
    common_v_output_t out;
    
    float4 in_position = float4(in.position, 1.0);
    float4x4 mvp = get_mat_mvp(mvp_uniforms);
    out.position = mvp * in_position;
    
    return out;
}

fragment half4 fragment_shader_stencil_deferred_point_light(common_v_output_t in [[stage_in]],
                                                    constant point_light_u_input_t& custom_uniforms [[buffer(0)]])
{
    return half4(1.0);
}

//

vertex common_v_output_t vertex_shader_deferred_spot_light(common_v_input_t in [[stage_in]],
                                                           constant common_u_input_t& mvp_uniforms [[buffer(1)]])
{
    common_v_output_t out;
    
    float4 in_position = float4(in.position, 1.0);
    float4x4 mvp = get_mat_mvp(mvp_uniforms);
    out.position = mvp * in_position;
    
    return out;
}

fragment half4 fragment_shader_deferred_spot_light(common_v_output_t in [[stage_in]],
                                                        constant spot_light_u_input_t& custom_uniforms [[buffer(0)]],
                                                        texture2d<half> normal_texture [[texture(0)]],
                                                        texture2d<float> position_texture [[texture(1)]])
{
    const uint2 screen_space_position = uint2(in.position.xy);
    const float4 position = position_texture.read(screen_space_position);
    const half4 normal = normal_texture.read(screen_space_position);
    
    const half min_value = 0.f;
    const half3 light_vector = static_cast<half3>(custom_uniforms.light_position.xyz - position.xyz);
    const half3 light_direction = normalize(light_vector);
    
    const half theta = dot(light_direction, normalize(-static_cast<half3>(custom_uniforms.light_direction.xyz)));
    const half epsilon = static_cast<half>(custom_uniforms.light_cutoff_angles.x - custom_uniforms.light_cutoff_angles.y);
    const half attenuation = max((theta - static_cast<half>(custom_uniforms.light_cutoff_angles.y)) / epsilon, min_value);
    const half intensity = max(dot(light_direction, normal.xyz), min_value);
    
    const half4 light_color = static_cast<half4>(custom_uniforms.light_color);
    half4 color = light_color;
    color *= intensity;
    color *= attenuation;
    
    const half specular_power = 16.f;
    const half specular_intensity = intensity;
    const half3 reflection_vector = normalize(reflect(-light_direction, normal.xyz));
    const half3 camera_direction = normalize(static_cast<half3>(custom_uniforms.camera_position.xyz - position.xyz));
    half4 specular = light_color * specular_intensity * pow(max(dot(reflection_vector, camera_direction), min_value), specular_power);
    specular *= attenuation * normal.w;
    color += specular;
    
    return color;
}

//

vertex common_v_output_t vertex_shader_car_mask(common_v_input_t in [[stage_in]],
                                                         constant common_u_input_t& uniforms [[buffer(1)]])
{
    common_v_output_t out;
    
    float4 in_position = float4(in.position, 1.0);
    float4x4 mvp = get_mat_mvp(uniforms);
    out.position = mvp * in_position;
    out.texcoord = in.texcoord;
    out.color = in.color;
    
    return out;
}

fragment half4 fragment_shader_car_mask(common_v_output_t in [[stage_in]])
{
    return half4(1.f);
}

