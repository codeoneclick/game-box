void main(){vec2 sides=texture2D(sampler_01,v_texcoord).rg;float m=min(sides.r,sides.g);attachment_01=m>.5?vec4(v_texcoord,1,1):vec4(0);}
