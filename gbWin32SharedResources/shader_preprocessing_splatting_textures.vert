
#if defined(__OPENGL_30__)

out vec2 v_texcoord;
out vec2 v_texcoord_scaled;

#else

varying vec2 v_texcoord;
varying vec2 v_texcoord_scaled;

#endif

const float k_texcoord_scale = 8.0;

void main()
{
    v_texcoord = a_texcoord;
    v_texcoord_scaled = v_texcoord * k_texcoord_scale;
    gl_Position = vec4(a_position, 1.0);
}
