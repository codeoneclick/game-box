void main(){
 vec2 uv=vec2(.5+.5*v_position.x/v_position.w,.5-.5*v_position.y/v_position.w);
#if !defined(VULKAN_API)
 uv.y=1.0-uv.y;
#endif
 vec4 receiver=texture2D(sampler_01,uv);
 // Project onto the visible terrace only, never the sky or a lower step.
 if(receiver.w<=0.0||abs(receiver.y-v_screen_position.y)>.08)discard;
 float radius=length(v_texcoord);
 float shadow=(1.0-smoothstep(.1,1.0,radius))*v_color.a;
 attachment_01=vec4(shadow,0,0,0);
}
