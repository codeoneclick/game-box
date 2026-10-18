// cos(yaw), sin(yaw)
uniform vec4 sky_rotation;
uniform vec4 sun;
// GB_SAMPLER_02_CUBE selects the engine's existing cube sampler slot.
layout(location=1) out vec4 attachment_02;
layout(location=2) out vec4 attachment_03;
vec3 sky_linear(vec3 c){return mix(c/12.92,pow((c+.055)/1.055,vec3(2.4)),step(vec3(.04045),c));}
void main(){
 vec3 lookup=normalize(v_normal);
 lookup.xz=vec2(sky_rotation.x*lookup.x-sky_rotation.y*lookup.z,sky_rotation.y*lookup.x+sky_rotation.x*lookup.z);
#if defined(USE_LAYOUTS)
 vec3 c=texture(sampler_02,lookup).rgb;
#else
 vec3 c=textureCube(sampler_02,lookup).rgb;
#endif
 attachment_01=vec4(sky_linear(c),1);attachment_02=vec4(0);attachment_03=vec4(0);
}
