#if defined(__OPENGL_30__)

in vec2 v_texcoord;
in vec3 v_normal;
in vec3 v_position;

#else

varying vec2 v_texcoord;
varying vec3 v_normal;
varying vec3 v_position;

#endif

uniform vec3 u_vec_camera_position;

uniform sampler2D sampler_01;

float rim_bias = 0.3;
float rim_power = 8.0;
vec4 rim_color = vec4(1.0, 0.0, 0.0, 1.0);

void main(void)
{
    vec4 diffuse_color = texture2D(sampler_01, v_texcoord);
    vec3 camera_direction = normalize(u_vec_camera_position - v_position);
    gl_FragColor = diffuse_color;
}