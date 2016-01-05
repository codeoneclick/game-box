#if defined(__OPENGL_30__)

in vec2 v_texcoord;
in vec4 v_ss_position;

#else

varying vec2 v_texcoord;
varying vec4 v_ss_position;

#endif

uniform sampler2D sampler_01;
uniform sampler2D sampler_02;

void main(void)
{
    vec4 ss_position = v_ss_position;
    ss_position.xy /= ss_position.w;
    vec2 ss_texcoord = 0.5 * (ss_position.xy + 1.0);
    vec4 l_color = texture2D(sampler_02, ss_texcoord);
    
    vec4 color = texture2D(sampler_01, v_texcoord) * l_color;
    gl_FragColor = color;
}