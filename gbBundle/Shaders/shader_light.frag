#if defined(__OPENGL_30__)

in vec2 v_position;

#else

varying vec2 v_position;

#endif

uniform float u_radius;
uniform vec4 u_color;

uniform sampler2D sampler_01;

void main()
{
    float attenuation = 1.0 - length(-v_position * u_radius) / u_radius;
    gl_FragColor = vec4(1.0);//vec4(u_color.rgb, attenuation);
}