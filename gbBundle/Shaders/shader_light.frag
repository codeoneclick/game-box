#if defined(__OPENGL_30__)

in vec2 v_position;

#else

varying vec2 v_position;

#endif

uniform int u_mask;
uniform float u_radius;
uniform vec4 u_color;

uniform sampler2D sampler_01;

void main()
{
    vec4 color = vec4(1.0);
    if(u_mask == 0)
    {
        float attenuation = 1.0 - length(-v_position * u_radius) / u_radius;
        color = vec4(u_color.rgb, attenuation);
    }
    gl_FragColor = color;
}