uniform mat4 mat_v;
uniform mat4 mat_p;
// World-space radius, angular bias, strength, encoded-normal flag.
uniform vec4 ao_params;
void main()
{
    vec4 p = texture2D(sampler_02, v_texcoord);
    vec4 ns = texture2D(sampler_01, v_texcoord);
    vec3 n = ao_params.w > 0.5 ? ns.xyz * 2.0 - 1.0 : ns.xyz;
    vec3 vp = (mat_v * vec4(p.xyz, 1.0)).xyz;
    float depth = -vp.z;
    // Object ID 9000000 is the editor hand: neither receives nor contributes AO.
    if (p.w <= 0.0 || ns.w <= 0.0 || ns.w == 9000000.0 || dot(n,n) < 0.01 || depth <= 0.001 || ao_params.z <= 0.0)
    {
        gl_FragColor = vec4(1.0);
        return;
    }
    n = normalize((mat_v * vec4(n,0.0)).xyz);
    float radius = max(ao_params.x, 0.001);
    vec2 uv_radius = 0.5 * abs(vec2(mat_p[0][0],mat_p[1][1])) * radius / depth;
    float rotation = texture2D(sampler_03,v_texcoord*32.0).r * 6.2831853;
    float occlusion = 0.0;
    // Fixed denominator: offscreen/background samples contribute no occlusion.
    for (int direction = 0; direction < 8; ++direction)
    {
        float angle = rotation + float(direction) * 0.7853982;
        vec2 direction_vector = vec2(cos(angle),sin(angle));
        for (int ring = 0; ring < 4; ++ring)
        {
            float fraction = (float(ring)+0.5)/4.0;
            vec2 uv = v_texcoord + direction_vector*uv_radius*fraction;
            if (any(lessThan(uv,vec2(0.0))) || any(greaterThan(uv,vec2(1.0)))) continue;
            vec4 q = texture2D(sampler_02,uv);
            if (q.w <= 0.0 || texture2D(sampler_01,uv).w == 9000000.0) continue;
            vec3 delta = (mat_v * vec4(q.xyz,1.0)).xyz - vp;
            float distance_value = length(delta);
            if (distance_value < 0.03 || distance_value >= radius) continue;
            float horizon = max(dot(n,delta/distance_value)-ao_params.y,0.0);
            occlusion += horizon * (1.0-smoothstep(radius*0.25,radius,distance_value));
        }
    }
    float visibility = pow(1.0-clamp(occlusion/32.0*ao_params.z*2.0,0.0,0.85),1.5);
    gl_FragColor = vec4(vec3(visibility),1.0);
}
