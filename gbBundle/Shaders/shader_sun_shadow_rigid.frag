void main(){
 if(v_tangent.x>.5)discard;
 if(v_tangent.y>.5&&texture2D(sampler_01,v_texcoord).a<.1)discard;
 // RGB24 depth in an existing UNORM render format; never filter before decoding.
 vec3 depth=fract(min(gl_FragCoord.z,.999999)*vec3(1,255,65025));
 depth.xy-=depth.yz/255.0;attachment_01=vec4(depth,1);
}
