
#if defined(OPENGL_30)

layout (location = 1) in vec2 v_texcoord;

layout (binding = 0) uniform sampler2D sampler_01;

#else

varying vec2 v_texcoord;

uniform sampler2D sampler_01;

#endif


void main()
{
    gl_FragColor = texture2D(sampler_01, v_texcoord);
}

