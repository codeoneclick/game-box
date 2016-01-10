#if defined(__OPENGL_30__)

in vec2 v_position;
in vec2 v_texcoord;

#else

varying vec2 v_position;
varying vec2 v_texcoord;

#endif

uniform float u_radius;

uniform sampler2D sampler_01;

void main(void)
{
    float attenuation = 1.0 - length(-v_position) / (u_radius);
    gl_FragColor = vec4(1.0, 0.0, 0.0, attenuation);
}