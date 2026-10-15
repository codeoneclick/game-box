void main(){
 vec2 uv=vec2(.5+.5*v_position.x/v_position.w,.5-.5*v_position.y/v_position.w);
#if !defined(VULKAN_API)
 uv.y=1.0-uv.y;
#endif
 vec4 p=texture2D(sampler_01,uv);
 if(p.w<=0.0||v_texcoord.x<=.0001)discard;
 float distance_from_center=length(p.xz-v_screen_position.xz)/v_texcoord.x;
 float a=(1.0-smoothstep(.015,.055,abs(distance_from_center-.94)))*.8;
 if(a<=0.0)discard;
 attachment_01=vec4(vec3(.03,.7,1.0)*a,a);
}
