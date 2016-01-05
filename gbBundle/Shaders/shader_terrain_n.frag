
#if defined(__OPENGL_30__)

in vec2 v_texcoord;
in vec3 v_camera_direction_ts;
in vec3 v_normal_ts;
in mat3 v_mat_tangent_space;

#else

varying vec2 v_texcoord;
varying vec3 v_camera_direction_ts;
varying vec3 v_normal_ts;
varying mat3 v_mat_tangent_space;

#endif

uniform sampler2D sampler_02;

const float k_scale = 0.005;
const float k_min_steps_num = 8.0;
const float k_max_steps_num = 32.0;

vec2 get_pom_texcoord(in sampler2D sampler, in vec2 texcoord, in vec3 camera_direction_ts, in vec3 normal_ts)
{
    vec2 parallax_limit = camera_direction_ts.xy * -length(camera_direction_ts.xy) / camera_direction_ts.z * k_scale;
    float current_ray_height = 1.0;
    float current_sampler_height = 0.0;
    float previous_sampler_height = 1.0;
    vec2 current_texcoord = v_texcoord;
    
    float steps_num = mix(k_min_steps_num, k_max_steps_num, dot(camera_direction_ts, normal_ts));
    float step_size = 1.0 / steps_num;
    vec2 step_offset = camera_direction_ts.xy * k_scale / ( steps_num * camera_direction_ts.z );
    
    vec2 current_texcoord_offset = vec2(0.0);
    vec2 previous_texcoord_offset = vec2(0.0);
    
    float step_index = 0.0;
    while(step_index < steps_num)
    {
        current_sampler_height = texture2D(sampler, current_texcoord + current_texcoord_offset).a;
        if (current_sampler_height > current_ray_height)
        {
            float delta1 = current_sampler_height - current_ray_height;
            float delta2 = (current_ray_height + step_size) - previous_sampler_height;
            float ratio = delta1 / (delta1 + delta2);
            
            current_texcoord_offset = ratio * previous_texcoord_offset + (1.0 - ratio) * current_texcoord_offset;
            
            step_index = steps_num + 1.0;
        }
        else
        {
            step_index++;
            
            current_ray_height -= step_size;
            previous_texcoord_offset = current_texcoord_offset;
            current_texcoord_offset += step_size * parallax_limit;
            previous_sampler_height = current_sampler_height;
        }
    }
    current_texcoord = v_texcoord + current_texcoord_offset;
    return current_texcoord;
}

void main()
{
    vec2 texcoord = get_pom_texcoord(sampler_02, v_texcoord, normalize(v_camera_direction_ts), normalize(v_normal_ts));
    
    vec4 color = texture2D(sampler_02, texcoord);
    
    vec3 normal_sampler_ts = normalize(v_mat_tangent_space * (color.xyz * 2.0 - 1.0));
    gl_FragColor = vec4(normal_sampler_ts * 0.5 + 0.5, color.a);
}
