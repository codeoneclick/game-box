uniform vec4 building_tint;
uniform vec4 building_sun;
uniform mat4 building_view;
void main(){
 vec2 p=abs(v_texcoord-.5)*2.0;
 float d=max(p.x,p.y);
 float ring=exp(-abs(d-.88)*30.0);
 float alpha=(ring*.8+(1.0-smoothstep(.7,1.0,d))*.16)*(1.0-smoothstep(.94,1.0,d));
 attachment_01=vec4(building_tint.rgb*alpha,alpha);
}
