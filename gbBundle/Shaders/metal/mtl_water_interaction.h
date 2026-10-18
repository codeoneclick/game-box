vertex common_v_output_t vertex_shader_water_mask(common_v_input_t in [[stage_in]],constant common_u_input_t& mvp [[buffer(1)]],constant ocean_uniforms& u [[buffer(2)]]){common_v_output_t out={};float4 p=get_mat_m(mvp)*float4(in.position,1);float2 uv=(p.xz-u.interaction.xy)/u.interaction.z;out.position=float4(uv.x*2-1,1-uv.y*2,0,1);out.view_space_position=p;return out;}
fragment half4 fragment_shader_water_mask(common_v_output_t in [[stage_in]],constant ocean_uniforms& u [[buffer(0)]]){float3 p=in.view_space_position.xyz;float h=ocean_ocean_height(p.xz,u);return half4(half(step(h,p.y)),half(step(p.y,h)),0,1);}
vertex common_v_output_t vertex_shader_water_land(common_v_input_t in [[stage_in]],constant common_u_input_t& mvp [[buffer(1)]],constant ocean_uniforms& u [[buffer(2)]]){common_v_output_t out={};float4 p=get_mat_m(mvp)*float4(in.position,1);float2 uv=(p.xz-u.interaction.xy)/u.interaction.z;out.position=float4(uv.x*2-1,1-uv.y*2,0,1);out.view_space_position=p;return out;}
fragment half4 fragment_shader_water_land(common_v_output_t in [[stage_in]],constant ocean_uniforms& u [[buffer(0)]]){if(in.view_space_position.y<u.time_level.y)discard_fragment();return half4(1);}
vertex common_v_output_t vertex_shader_water_seed(common_v_input_t in [[stage_in]]){common_v_output_t out={};out.position=float4(in.position.xy,0,1);out.texcoord=float2(.5+.5*in.position.x,.5-.5*in.position.y);return out;}
fragment float4 fragment_shader_water_seed(common_v_output_t in [[stage_in]],texture2d<float> a [[texture(0)]],texture2d<float> b [[texture(1)]]){float2 sides=a.sample(ocean_nearest,in.texcoord).rg;float m=min(sides.r,sides.g);return m>.5?float4(in.texcoord,0,0):float4(-1,-1,0,0);}

vertex common_v_output_t vertex_shader_water_jfa0(common_v_input_t in [[stage_in]]){common_v_output_t out={};out.position=float4(in.position.xy,0,1);out.texcoord=float2(.5+.5*in.position.x,.5-.5*in.position.y);return out;}
fragment float4 fragment_shader_water_jfa0(common_v_output_t in [[stage_in]],texture2d<float> a [[texture(0)]],texture2d<float> b [[texture(1)]]){float2 uv=in.texcoord;float4 best=float4(-1,-1,0,0);float bd=1e9;
 for(int y=-1;y<=1;y++)for(int x=-1;x<=1;x++){float2 q=uv+float2(x,y)*max(1.0,exp2(ceil(log2(float(max(a.get_width(),a.get_height()))))-1.0))/float2(a.get_width(),a.get_height());if(any(q<float2(0))||any(q>float2(1)))continue;
 float4 seed=a.sample(ocean_nearest,q);float d=dot(seed.xy-uv,seed.xy-uv);if(seed.x>=0.0&&d<bd){bd=d;best=seed;}}
 return best;}

vertex common_v_output_t vertex_shader_water_jfa1(common_v_input_t in [[stage_in]]){common_v_output_t out={};out.position=float4(in.position.xy,0,1);out.texcoord=float2(.5+.5*in.position.x,.5-.5*in.position.y);return out;}
fragment float4 fragment_shader_water_jfa1(common_v_output_t in [[stage_in]],texture2d<float> a [[texture(0)]],texture2d<float> b [[texture(1)]]){float2 uv=in.texcoord;float4 best=float4(-1,-1,0,0);float bd=1e9;
 for(int y=-1;y<=1;y++)for(int x=-1;x<=1;x++){float2 q=uv+float2(x,y)*max(1.0,exp2(ceil(log2(float(max(a.get_width(),a.get_height()))))-2.0))/float2(a.get_width(),a.get_height());if(any(q<float2(0))||any(q>float2(1)))continue;
 float4 seed=a.sample(ocean_nearest,q);float d=dot(seed.xy-uv,seed.xy-uv);if(seed.x>=0.0&&d<bd){bd=d;best=seed;}}
 return best;}

vertex common_v_output_t vertex_shader_water_jfa2(common_v_input_t in [[stage_in]]){common_v_output_t out={};out.position=float4(in.position.xy,0,1);out.texcoord=float2(.5+.5*in.position.x,.5-.5*in.position.y);return out;}
fragment float4 fragment_shader_water_jfa2(common_v_output_t in [[stage_in]],texture2d<float> a [[texture(0)]],texture2d<float> b [[texture(1)]]){float2 uv=in.texcoord;float4 best=float4(-1,-1,0,0);float bd=1e9;
 for(int y=-1;y<=1;y++)for(int x=-1;x<=1;x++){float2 q=uv+float2(x,y)*max(1.0,exp2(ceil(log2(float(max(a.get_width(),a.get_height()))))-3.0))/float2(a.get_width(),a.get_height());if(any(q<float2(0))||any(q>float2(1)))continue;
 float4 seed=a.sample(ocean_nearest,q);float d=dot(seed.xy-uv,seed.xy-uv);if(seed.x>=0.0&&d<bd){bd=d;best=seed;}}
 return best;}

vertex common_v_output_t vertex_shader_water_jfa3(common_v_input_t in [[stage_in]]){common_v_output_t out={};out.position=float4(in.position.xy,0,1);out.texcoord=float2(.5+.5*in.position.x,.5-.5*in.position.y);return out;}
fragment float4 fragment_shader_water_jfa3(common_v_output_t in [[stage_in]],texture2d<float> a [[texture(0)]],texture2d<float> b [[texture(1)]]){float2 uv=in.texcoord;float4 best=float4(-1,-1,0,0);float bd=1e9;
 for(int y=-1;y<=1;y++)for(int x=-1;x<=1;x++){float2 q=uv+float2(x,y)*max(1.0,exp2(ceil(log2(float(max(a.get_width(),a.get_height()))))-4.0))/float2(a.get_width(),a.get_height());if(any(q<float2(0))||any(q>float2(1)))continue;
 float4 seed=a.sample(ocean_nearest,q);float d=dot(seed.xy-uv,seed.xy-uv);if(seed.x>=0.0&&d<bd){bd=d;best=seed;}}
 return best;}

vertex common_v_output_t vertex_shader_water_jfa4(common_v_input_t in [[stage_in]]){common_v_output_t out={};out.position=float4(in.position.xy,0,1);out.texcoord=float2(.5+.5*in.position.x,.5-.5*in.position.y);return out;}
fragment float4 fragment_shader_water_jfa4(common_v_output_t in [[stage_in]],texture2d<float> a [[texture(0)]],texture2d<float> b [[texture(1)]]){float2 uv=in.texcoord;float4 best=float4(-1,-1,0,0);float bd=1e9;
 for(int y=-1;y<=1;y++)for(int x=-1;x<=1;x++){float2 q=uv+float2(x,y)*max(1.0,exp2(ceil(log2(float(max(a.get_width(),a.get_height()))))-5.0))/float2(a.get_width(),a.get_height());if(any(q<float2(0))||any(q>float2(1)))continue;
 float4 seed=a.sample(ocean_nearest,q);float d=dot(seed.xy-uv,seed.xy-uv);if(seed.x>=0.0&&d<bd){bd=d;best=seed;}}
 return best;}

vertex common_v_output_t vertex_shader_water_jfa5(common_v_input_t in [[stage_in]]){common_v_output_t out={};out.position=float4(in.position.xy,0,1);out.texcoord=float2(.5+.5*in.position.x,.5-.5*in.position.y);return out;}
fragment float4 fragment_shader_water_jfa5(common_v_output_t in [[stage_in]],texture2d<float> a [[texture(0)]],texture2d<float> b [[texture(1)]]){float2 uv=in.texcoord;float4 best=float4(-1,-1,0,0);float bd=1e9;
 for(int y=-1;y<=1;y++)for(int x=-1;x<=1;x++){float2 q=uv+float2(x,y)*max(1.0,exp2(ceil(log2(float(max(a.get_width(),a.get_height()))))-6.0))/float2(a.get_width(),a.get_height());if(any(q<float2(0))||any(q>float2(1)))continue;
 float4 seed=a.sample(ocean_nearest,q);float d=dot(seed.xy-uv,seed.xy-uv);if(seed.x>=0.0&&d<bd){bd=d;best=seed;}}
 return best;}

vertex common_v_output_t vertex_shader_water_jfa6(common_v_input_t in [[stage_in]]){common_v_output_t out={};out.position=float4(in.position.xy,0,1);out.texcoord=float2(.5+.5*in.position.x,.5-.5*in.position.y);return out;}
fragment float4 fragment_shader_water_jfa6(common_v_output_t in [[stage_in]],texture2d<float> a [[texture(0)]],texture2d<float> b [[texture(1)]]){float2 uv=in.texcoord;float4 best=float4(-1,-1,0,0);float bd=1e9;
 for(int y=-1;y<=1;y++)for(int x=-1;x<=1;x++){float2 q=uv+float2(x,y)*max(1.0,exp2(ceil(log2(float(max(a.get_width(),a.get_height()))))-7.0))/float2(a.get_width(),a.get_height());if(any(q<float2(0))||any(q>float2(1)))continue;
 float4 seed=a.sample(ocean_nearest,q);float d=dot(seed.xy-uv,seed.xy-uv);if(seed.x>=0.0&&d<bd){bd=d;best=seed;}}
 return best;}

vertex common_v_output_t vertex_shader_water_jfa7(common_v_input_t in [[stage_in]]){common_v_output_t out={};out.position=float4(in.position.xy,0,1);out.texcoord=float2(.5+.5*in.position.x,.5-.5*in.position.y);return out;}
fragment float4 fragment_shader_water_jfa7(common_v_output_t in [[stage_in]],texture2d<float> a [[texture(0)]],texture2d<float> b [[texture(1)]]){float2 uv=in.texcoord;float4 best=float4(-1,-1,0,0);float bd=1e9;
 for(int y=-1;y<=1;y++)for(int x=-1;x<=1;x++){float2 q=uv+float2(x,y)*max(1.0,exp2(ceil(log2(float(max(a.get_width(),a.get_height()))))-8.0))/float2(a.get_width(),a.get_height());if(any(q<float2(0))||any(q>float2(1)))continue;
 float4 seed=a.sample(ocean_nearest,q);float d=dot(seed.xy-uv,seed.xy-uv);if(seed.x>=0.0&&d<bd){bd=d;best=seed;}}
 return best;}

vertex common_v_output_t vertex_shader_water_jfa8(common_v_input_t in [[stage_in]]){common_v_output_t out={};out.position=float4(in.position.xy,0,1);out.texcoord=float2(.5+.5*in.position.x,.5-.5*in.position.y);return out;}
fragment float4 fragment_shader_water_jfa8(common_v_output_t in [[stage_in]],texture2d<float> a [[texture(0)]],texture2d<float> b [[texture(1)]]){float2 uv=in.texcoord;float4 best=float4(-1,-1,0,0);float bd=1e9;
 for(int y=-1;y<=1;y++)for(int x=-1;x<=1;x++){float2 q=uv+float2(x,y)*max(1.0,exp2(ceil(log2(float(max(a.get_width(),a.get_height()))))-9.0))/float2(a.get_width(),a.get_height());if(any(q<float2(0))||any(q>float2(1)))continue;
 float4 seed=a.sample(ocean_nearest,q);float d=dot(seed.xy-uv,seed.xy-uv);if(seed.x>=0.0&&d<bd){bd=d;best=seed;}}
 return best;}

vertex common_v_output_t vertex_shader_water_jfa9(common_v_input_t in [[stage_in]]){common_v_output_t out={};out.position=float4(in.position.xy,0,1);out.texcoord=float2(.5+.5*in.position.x,.5-.5*in.position.y);return out;}
fragment float4 fragment_shader_water_jfa9(common_v_output_t in [[stage_in]],texture2d<float> a [[texture(0)]],texture2d<float> b [[texture(1)]]){float2 uv=in.texcoord;float4 best=float4(-1,-1,0,0);float bd=1e9;
 for(int y=-1;y<=1;y++)for(int x=-1;x<=1;x++){float2 q=uv+float2(x,y)*max(1.0,exp2(ceil(log2(float(max(a.get_width(),a.get_height()))))-10.0))/float2(a.get_width(),a.get_height());if(any(q<float2(0))||any(q>float2(1)))continue;
 float4 seed=a.sample(ocean_nearest,q);float d=dot(seed.xy-uv,seed.xy-uv);if(seed.x>=0.0&&d<bd){bd=d;best=seed;}}
 return best;}

vertex common_v_output_t vertex_shader_water_jfa10(common_v_input_t in [[stage_in]]){common_v_output_t out={};out.position=float4(in.position.xy,0,1);out.texcoord=float2(.5+.5*in.position.x,.5-.5*in.position.y);return out;}
fragment float4 fragment_shader_water_jfa10(common_v_output_t in [[stage_in]],texture2d<float> a [[texture(0)]],texture2d<float> b [[texture(1)]]){float2 uv=in.texcoord;float4 best=float4(-1,-1,0,0);float bd=1e9;
 for(int y=-1;y<=1;y++)for(int x=-1;x<=1;x++){float2 q=uv+float2(x,y)*max(1.0,exp2(ceil(log2(float(max(a.get_width(),a.get_height()))))-11.0))/float2(a.get_width(),a.get_height());if(any(q<float2(0))||any(q>float2(1)))continue;
 float4 seed=a.sample(ocean_nearest,q);float d=dot(seed.xy-uv,seed.xy-uv);if(seed.x>=0.0&&d<bd){bd=d;best=seed;}}
 return best;}

vertex common_v_output_t vertex_shader_water_jfa11(common_v_input_t in [[stage_in]]){common_v_output_t out={};out.position=float4(in.position.xy,0,1);out.texcoord=float2(.5+.5*in.position.x,.5-.5*in.position.y);return out;}
fragment float4 fragment_shader_water_jfa11(common_v_output_t in [[stage_in]],texture2d<float> a [[texture(0)]],texture2d<float> b [[texture(1)]]){float2 uv=in.texcoord;float4 best=float4(-1,-1,0,0);float bd=1e9;
 for(int y=-1;y<=1;y++)for(int x=-1;x<=1;x++){float2 q=uv+float2(x,y)*max(1.0,exp2(ceil(log2(float(max(a.get_width(),a.get_height()))))-12.0))/float2(a.get_width(),a.get_height());if(any(q<float2(0))||any(q>float2(1)))continue;
 float4 seed=a.sample(ocean_nearest,q);float d=dot(seed.xy-uv,seed.xy-uv);if(seed.x>=0.0&&d<bd){bd=d;best=seed;}}
 return best;}

vertex common_v_output_t vertex_shader_water_distance(common_v_input_t in [[stage_in]]){common_v_output_t out={};out.position=float4(in.position.xy,0,1);out.texcoord=float2(.5+.5*in.position.x,.5-.5*in.position.y);return out;}
fragment float4 fragment_shader_water_distance(common_v_output_t in [[stage_in]],constant ocean_uniforms& u [[buffer(0)]],texture2d<float> a [[texture(0)]],texture2d<float> b [[texture(1)]]){float4 seed=a.sample(ocean_nearest,in.texcoord);float2 delta=(in.texcoord-seed.xy)*u.interaction.z;float d=length(delta);float2 sides=b.sample(ocean_nearest,in.texcoord).rg;float inside=min(sides.r,sides.g);return float4(seed.x>=0.0?max(d-u.interaction.z*u.interaction.w*.5,0.0):320.0,d>.001&&seed.x>=0.0?delta/d:float2(0),inside);}

