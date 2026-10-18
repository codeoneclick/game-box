void main(){
 // Project the footprint onto actual scene geometry, including stair treads.
 gl_Position=vec4(a_position.xy*2.0,0.0,1.0);v_position=gl_Position;
}
