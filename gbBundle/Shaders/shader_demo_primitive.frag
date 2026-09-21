layout(location = 1) out vec4 attachment_02;
layout(location = 2) out vec4 attachment_03;

void main()
{
    vec3 normal = normalize(v_normal);
    float diffuse = max(dot(normal, normalize(vec3(0.35, 0.8, 0.45))), 0.0);
    vec4 albedo = texture2D(sampler_01, v_texcoord);
    attachment_01 = vec4(albedo.rgb * (0.45 + 0.55 * diffuse), albedo.a);
    attachment_02 = vec4(normal, 1.0);
    attachment_03 = vec4(v_screen_position.xyz, gl_FragCoord.z);
}
