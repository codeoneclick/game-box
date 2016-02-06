#if defined(__OPENGL_30__)

in vec2 v_position;
in vec2 v_texcoord;

#else

varying vec2 v_position;
varying vec2 v_texcoord;

#endif

uniform float u_radius;
uniform vec2 u_center;

uniform sampler2D sampler_01;

void main(void)
{
    vec4 color = texture2D(sampler_01, v_texcoord);
    color.a = 1.0 - length(v_position - u_center) / u_radius;
    gl_FragColor = color;
}