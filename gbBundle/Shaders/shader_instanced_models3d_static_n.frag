
#if defined(__OPENGL_30__)

in vec2 v_texcoord;
in mat3 v_tbn_matrix;

#else

varying vec2 v_texcoord;
varying mat3 v_tbn_matrix;

#endif

uniform sampler2D sampler_01;
uniform sampler2D sampler_02;

void main(void)
{
    vec3 normal = (v_tbn_matrix * (texture2D(sampler_01, v_texcoord).rgb * 2.0 - 1.0) + 1.0) * 0.5;
    float specular = texture2D(sampler_02, v_texcoord).r;
    gl_FragColor = vec4(normal, specular);
}
