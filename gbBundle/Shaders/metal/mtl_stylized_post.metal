#include "mtl_common.h"
using namespace metal;
constexpr sampler stylized_nearest(coord::normalized,address::clamp_to_edge,filter::nearest);
struct stylized_post_uniforms {
 float4 post_flags;
 float4 post_color;
 float4 post_shadow;
 float4 post_highlight;
 float4 post_bloom;
 float4 post_fog;
 float4 post_ranges;
 float4 post_dof;
 float4 post_extra;
 float4 post_camera;
 float4 post_display;
 float4x4 post_view,post_projection;
};
vertex common_v_output_t vertex_shader_stylized_output(common_v_input_t in [[stage_in]],constant common_u_input_t& u [[buffer(1)]]){
 common_v_output_t out={};out.position=float4(in.position,1);out.texcoord=in.texcoord;return out;
}
fragment half4 fragment_shader_stylized_output(common_v_output_t in [[stage_in]],constant stylized_post_uniforms& u [[buffer(1)]],texture2d<float> color [[texture(0)]],texture2d<float> debug_ui [[texture(1)]]){
 float4 scene=color.sample(linear_sampler,in.texcoord),overlay=debug_ui.sample(linear_sampler,in.texcoord);
 return half4(float4(mix(scene.rgb,overlay.rgb,overlay.a),1));
}

float3 stylized_tone(float3 c,constant stylized_post_uniforms& u){
 c=max(c,float3(0))*exp2(u.post_color.x);
 return u.post_flags.y>.5?clamp((c*(2.51*c+.03))/(c*(2.43*c+.59)+.14),0.0,1.0):c;
}
vertex common_v_output_t vertex_shader_stylized_final(common_v_input_t in [[stage_in]],constant common_u_input_t& u [[buffer(1)]]){common_v_output_t out={};out.position=float4(in.position,1);out.texcoord=in.texcoord;return out;}
float3 stylized_grade(float3 c,float4 surface,constant stylized_post_uniforms& u){
 if(u.post_flags.z<.5)return c;
 float object_id=surface.w;
 float luminance=dot(c,float3(.2126,.7152,.0722));
 if(object_id>=12000000.0){
  // Lift the toe/midtones without raising true black or flattening cel bands.
  c+=u.post_highlight.w*sqrt(max(c,float3(0)))*(1.0-smoothstep(.1,.65,luminance));
  float chroma=max(c.r,max(c.g,c.b))-min(c.r,min(c.g,c.b));
  float neutral=1.0-smoothstep(.02,.12,chroma);
  float3 mineral=mix(float3(.96,1.0,1.04),float3(1.02,1.01,.97),smoothstep(.35,.75,luminance));
  c*=mix(float3(1),mineral,neutral);
 }
 c=mix(float3(dot(c,float3(.2126,.7152,.0722))),c,u.post_color.y);
 c=(c-float3(.18))*u.post_color.z+float3(.18);
 // Keep bright foliage while suppressing the most acid green chroma.
 float green=smoothstep(.02,.15,c.g-max(c.r,c.b))*smoothstep(.35,.85,c.g);
 if(object_id>0.0)c=mix(c,float3(dot(c,float3(.2126,.7152,.0722))),.16*green);
 if(object_id>=2.0&&object_id<=27.0&&surface.g<.9)c*=float3(1.025,1.0,.985);
 c*=float3(1.0+u.post_color.w,1.0+u.post_shadow.w,1.0-u.post_color.w);
 float balance=smoothstep(.08,.75,luminance);
 c*=mix(u.post_shadow.rgb,u.post_highlight.rgb,balance);
 return max(c,float3(0));
}
float2 stylized_depth(float2 uv,texture2d<float> positions,texture2d<float> water_tex,constant stylized_post_uniforms& u){
 float4 geometry=positions.sample(stylized_nearest,uv);
 float water=water_tex.sample(linear_sampler,uv).a;
 float2 ndc=float2(uv.x*2.0-1.0,1.0-uv.y*2.0);
 float3 view_ray=normalize(float3((ndc+float2(u.post_projection[2][0],u.post_projection[2][1]))/float2(u.post_projection[0][0],u.post_projection[1][1]),-1));
 float3 ray=transpose(float3x3(u.post_view[0].xyz,u.post_view[1].xyz,u.post_view[2].xyz))*view_ray;
 if(water>.5&&ray.y<-.00001){float d=max(0.0,(u.post_extra.z-u.post_camera.y)/ray.y);return float2(d,d*-view_ray.z);}
 if(geometry.w>0.0)return float2(length(geometry.xyz-u.post_camera.xyz),geometry.w);
 return float2(u.post_ranges.y);
}
fragment half4 fragment_shader_stylized_final(common_v_output_t in [[stage_in]],constant stylized_post_uniforms& u [[buffer(1)]],texture2d<float> scene [[texture(0)]],texture2d<float> normal [[texture(1)]],texture2d<float> positions [[texture(4)]],texture2d<float> water [[texture(5)]]){
 float4 source=scene.sample(linear_sampler,in.texcoord);float3 hdr=source.rgb;hdr=u.post_flags.x>.5?hdr:clamp(hdr,0.0,1.0);
 if(u.post_ranges.z>.5){float fog=u.post_fog.w*smoothstep(u.post_ranges.x,u.post_ranges.y,stylized_depth(in.texcoord,positions,water,u).x);hdr=mix(hdr,u.post_fog.rgb,fog);}
 float3 toned=stylized_tone(hdr,u);
 float3 graded=stylized_grade(toned,normal.sample(stylized_nearest,in.texcoord),u);
 float3 color=graded;
 color=mix(color,float3(.05126946),source.a);
 int debug_view=int(u.post_bloom.w+.5);
 if(debug_view==1)color=max(source.rgb,float3(0))/(1.0+max(source.rgb,float3(0)));
 else if(debug_view==4)color=toned;
 else if(debug_view==5)color=graded;
 return half4(float4(color,1));
}
vertex common_v_output_t vertex_shader_stylized_bright(common_v_input_t in [[stage_in]],constant common_u_input_t& u [[buffer(1)]]){common_v_output_t out={};out.position=float4(in.position,1);out.texcoord=in.texcoord;return out;}
vertex common_v_output_t vertex_shader_stylized_blur_h(common_v_input_t in [[stage_in]],constant common_u_input_t& u [[buffer(1)]]){common_v_output_t out={};out.position=float4(in.position,1);out.texcoord=in.texcoord;return out;}
vertex common_v_output_t vertex_shader_stylized_blur_v(common_v_input_t in [[stage_in]],constant common_u_input_t& u [[buffer(1)]]){common_v_output_t out={};out.position=float4(in.position,1);out.texcoord=in.texcoord;return out;}
float3 bloom_bright(float2 uv,texture2d<float> scene,constant stylized_post_uniforms& u){
 float4 source=scene.sample(linear_sampler,uv);float3 c=u.post_flags.x>.5?max(source.rgb,float3(0)):clamp(source.rgb,0.0,1.0);
 float peak=max(c.r,max(c.g,c.b)),threshold=u.post_bloom.x,knee=max(threshold*.25,.001);
 float soft=clamp(peak-threshold+knee,0.0,2.0*knee);soft=soft*soft/(4.0*knee);
 return c*(max(peak-threshold,soft)/max(peak,.0001))*(1.0-source.a);
}
fragment half4 fragment_shader_stylized_bright(common_v_output_t in [[stage_in]],constant stylized_post_uniforms& u [[buffer(1)]],texture2d<float> scene [[texture(0)]]){
 if(u.post_flags.w<.5)return half4(0);float2 d=float2(u.post_bloom.z*.25)/float2(scene.get_width(),scene.get_height());
 float3 c=(bloom_bright(in.texcoord+d,scene,u)+bloom_bright(in.texcoord-d,scene,u)+bloom_bright(in.texcoord+float2(d.x,-d.y),scene,u)+bloom_bright(in.texcoord+float2(-d.x,d.y),scene,u))*.25;
 return half4(float4(c,0));
}
float3 stylized_blur(float2 uv,float2 direction,texture2d<float> scene){
 float2 d=direction/float2(scene.get_width(),scene.get_height());float3 c=scene.sample(linear_sampler,uv).rgb*.227027;
 for(int i=1;i<=4;i++){float w=i==1?.1945946:i==2?.1216216:i==3?.054054:.016216;c+=(scene.sample(linear_sampler,uv+d*float(i)).rgb+scene.sample(linear_sampler,uv-d*float(i)).rgb)*w;}
 return c;
}
fragment half4 fragment_shader_stylized_blur_h(common_v_output_t in [[stage_in]],constant stylized_post_uniforms& u [[buffer(1)]],texture2d<float> scene [[texture(0)]]){if(u.post_flags.w<.5)return half4(0);return half4(float4(stylized_blur(in.texcoord,float2(1,0),scene),0));}
fragment half4 fragment_shader_stylized_blur_v(common_v_output_t in [[stage_in]],constant stylized_post_uniforms& u [[buffer(1)]],texture2d<float> scene [[texture(0)]]){if(u.post_flags.w<.5)return half4(0);return half4(float4(stylized_blur(in.texcoord,float2(0,1),scene),0));}

vertex common_v_output_t vertex_shader_render_debug_blit(common_v_input_t in [[stage_in]],constant common_u_input_t& u [[buffer(1)]]){
 common_v_output_t out={};out.position=float4(in.position,1);out.texcoord=in.texcoord;return out;
}
fragment half4 fragment_shader_render_debug_blit(common_v_output_t in [[stage_in]],texture2d<float> source [[texture(0)]]){
 return half4(source.sample(linear_sampler,in.texcoord));
}
vertex common_v_output_t vertex_shader_render_debug_preview(common_v_input_t in [[stage_in]],constant common_u_input_t& u [[buffer(1)]]){
 common_v_output_t out={};out.position=get_mat_mvp(u)*float4(in.position,1);out.texcoord=in.texcoord;out.color=in.color;return out;
}
struct render_debug_preview_values {float4 preview_params;};
fragment half4 fragment_shader_render_debug_preview(common_v_output_t in [[stage_in]],constant render_debug_preview_values& u [[buffer(0)]],texture2d<float> source [[texture(0)]]){
 float4 data=source.sample(linear_sampler,in.texcoord);float3 color=data.rgb;
 if(u.preview_params.x<.5)color=max(color,float3(0))/(1.0+max(color,float3(0)));
 else if(u.preview_params.x<1.5)color=clamp(color,0.0,1.0);
 else if(u.preview_params.x<2.5)color=float3(data.w>0.0?1.0-exp(-data.w*.01):0.0);
 else if(u.preview_params.x<3.5)color=float3(clamp(data.x/32.0,0.0,1.0),clamp(data.y*.5+.5,0.0,1.0),clamp(data.z*.5+.5,0.0,1.0));
 else color=float3(clamp(data.r,0.0,1.0));
 return half4(float4(color,1)*in.color);
}
