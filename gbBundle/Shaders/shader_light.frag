#if defined(__OPENGL_30__)

in vec2 v_position;
in vec2 v_texcoord;

#else

varying vec2 v_position;
varying vec2 v_texcoord;

#endif

uniform float u_radius;
uniform vec4 u_color;

uniform sampler2D sampler_01;

void main()
{
    float attenuation = 1.0 - length(-v_position * u_radius) / u_radius;
    gl_FragColor = vec4(u_color.rgb, attenuation);
}