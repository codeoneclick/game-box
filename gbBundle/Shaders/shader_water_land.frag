uniform vec4 time_level;
void main(){vec3 p=v_screen_position.xyz;if(p.y<time_level.y)discard;attachment_01=vec4(1);}
