layout(location = 1) out vec4 attachment_02;
layout(location = 2) out vec4 attachment_03;

void main()
{
    vec3 normal = normalize(v_normal);
    float diffuse = max(dot(normal, normalize(vec3(0.35, 0.8, 0.45))), 0.0);
    attachment_01 = vec4(v_color.rgb * (0.5 + 0.5 * diffuse), 1.0);
    attachment_02 = vec4(normal, 1.0);
    attachment_03 = vec4(v_screen_position.xyz, gl_FragCoord.z);
}
