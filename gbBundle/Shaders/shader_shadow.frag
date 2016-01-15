#if defined(__OPENGL_30__)

in vec2 v_position;
in vec2 v_texcoord;

#else

varying vec2 v_position;
varying vec2 v_texcoord;

#endif

uniform sampler2D sampler_01;

uniform vec4 u_shadow_color;
uniform vec2 u_shadow_cast_position;

void main()
{
    float attenuation = 1.0 - length(u_shadow_cast_position - v_position) / 1024.0;
    gl_FragColor = vec4(u_shadow_color.rgb, attenuation);
}