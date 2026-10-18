uniform vec4 building_tint;
uniform vec4 building_sun;
// World-to-footprint transform for the aura only.
uniform mat4 building_view;
void main(){
 vec2 uv=vec2(.5+.5*v_position.x/v_position.w,.5-.5*v_position.y/v_position.w);
#if !defined(VULKAN_API)
 uv.y=1.0-uv.y;
#endif
 vec4 world=texture2D(sampler_01,uv);if(world.w<=0.0)discard;
 vec2 p=abs((building_view*vec4(world.xyz,1.0)).xz)*2.0;
 float d=max(p.x,p.y);
 float ring=exp(-abs(d-.88)*30.0);
 float alpha=(ring*.8+(1.0-smoothstep(.7,1.0,d))*.16)*(1.0-smoothstep(.94,1.0,d));
 if(alpha<.001)discard;
 attachment_01=vec4(building_tint.rgb*alpha,alpha);
}
