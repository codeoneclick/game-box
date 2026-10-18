uniform vec4 interaction;
void main(){vec4 seed=texture2D(sampler_01,v_texcoord);vec2 delta=(v_texcoord-seed.xy)*interaction.z;float d=length(delta);vec2 sides=texture2D(sampler_02,v_texcoord).rg;float inside=min(sides.r,sides.g);attachment_01=vec4(seed.x>=0.0?max(d-interaction.z*interaction.w*.5,0.0):320.0,d>.001&&seed.x>=0.0?delta/d:vec2(0),inside);}
