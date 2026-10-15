void main(){float r=length(v_texcoord);float fade=1.0-smoothstep(.15,1.0,r);vec2 radial=v_texcoord/max(r,.18);attachment_01=vec4((radial*.75+v_normal.xz*.3)*fade*v_color.a,0,0);}
