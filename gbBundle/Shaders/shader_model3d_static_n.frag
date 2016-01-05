
#if defined(__OPENGL_30__)

in vec2 v_texcoord;
in mat3 v_tbn_matrix;
in vec3 v_position;

#else

varying vec2 v_texcoord;
varying mat3 v_tbn_matrix;
varying vec3 v_position;

#endif

uniform vec3 u_vec_camera_position;

uniform sampler2D sampler_01;
uniform sampler2D sampler_02;

void main(void)
{
    vec3 camera_direction = normalize((v_position - u_vec_camera_position) * v_tbn_matrix);
    
    float scale = 0.075;
    float bias = 0.05;
    
    float specular = texture2D(sampler_02, v_texcoord).r;
    
    float displace = specular * scale - bias;
    vec2 offset = (displace * camera_direction.xy) / camera_direction.z;
    
    vec3 normal_color = texture2D(sampler_01, v_texcoord + offset).rgb * 2.0 - 1.0;
    vec3 normal_tbn = v_tbn_matrix * normal_color;

#define __ENCODE_NORMAL_SPHEREMAP__
#if defined(__ENCODE_NORMAL_XYZ__)
    

    gl_FragColor = vec4(normal_tbn * 0.5 + 0.5, specular);
    
#elif defined(__ENCODE_NORMAL_SPHEREMAP__)
    
    float p = sqrt(normal_tbn.z * 8.0 + 8.0);
    
    gl_FragColor = vec4(normal_tbn.xy / p + 0.5, 0.0, specular);

#endif
}
