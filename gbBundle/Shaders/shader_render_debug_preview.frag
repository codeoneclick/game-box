uniform vec4 preview_params;
void main(){
 vec4 data=texture2D(sampler_01,v_texcoord);vec3 color=data.rgb;
 if(preview_params.x<.5)color=max(color,vec3(0))/(1.0+max(color,vec3(0)));
 else if(preview_params.x<1.5)color=clamp(color,0.0,1.0);
 else if(preview_params.x<2.5)color=vec3(data.w>0.0?1.0-exp(-data.w*.01):0.0);
 else if(preview_params.x<3.5)color=vec3(clamp(data.x/32.0,0.0,1.0),clamp(data.y*.5+.5,0.0,1.0),clamp(data.z*.5+.5,0.0,1.0));
 else color=vec3(clamp(data.r,0.0,1.0));
 attachment_01=vec4(color,1.0)*v_color;
}
