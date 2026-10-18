void main(){vec2 uv=v_texcoord;vec4 best=vec4(-1,-1,0,0);float bd=1e9;
 for(int y=-1;y<=1;y++)for(int x=-1;x<=1;x++){vec2 q=uv+vec2(x,y)*max(1.0,exp2(ceil(log2(float(max(textureSize(sampler_01,0).x,textureSize(sampler_01,0).y))))-6.0))/vec2(textureSize(sampler_01,0));if(any(lessThan(q,vec2(0)))||any(greaterThan(q,vec2(1))))continue;
 vec4 seed=texture2D(sampler_01,q);float d=dot(seed.xy-uv,seed.xy-uv);if(seed.x>=0.0&&d<bd){bd=d;best=seed;}}
 attachment_01=best;}
