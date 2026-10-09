uniform vec4 interaction;
void main(){vec4 p=get_mat_m()*vec4(a_position,1);vec2 uv=(p.xz-interaction.xy)/interaction.z;gl_Position=vec4(uv.x*2-1,1-uv.y*2,0,1);v_screen_position=p;}
