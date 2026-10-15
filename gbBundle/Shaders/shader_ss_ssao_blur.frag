void main()
{
#if !defined(USE_LAYOUTS)
    // The legacy GL 2 path has no textureSize; keep the raw AO mask.
    gl_FragColor = texture2D(sampler_01,v_texcoord)*(1.0-clamp(texture2D(sampler_04,v_texcoord).r,0.0,.6));
#else
    vec4 p = texture2D(sampler_02,v_texcoord);
    vec4 ns = texture2D(sampler_03,v_texcoord);
    if (p.w <= 0.0 || ns.w <= 0.0) { gl_FragColor=vec4(1.0); return; }
    vec3 n = normalize(ns.xyz*2.0-1.0);
    vec2 pixel = 1.0/vec2(textureSize(sampler_01,0));
    float total = 0.0, weights = 0.0;
    for (int y=-2; y<=2; ++y) for (int x=-2; x<=2; ++x)
    {
        vec2 uv = v_texcoord + vec2(x,y)*pixel;
        if (any(lessThan(uv,vec2(0.0))) || any(greaterThan(uv,vec2(1.0)))) continue;
        vec4 q = texture2D(sampler_02,uv);
        vec4 nq = texture2D(sampler_03,uv);
        if (q.w <= 0.0 || nq.w <= 0.0) continue;
        float weight = exp(-float(x*x+y*y)/8.0-abs(q.w-p.w)/0.36);
        weight *= pow(max(dot(n,normalize(nq.xyz*2.0-1.0)),0.0),8.0);
        total += texture2D(sampler_01,uv).r*weight;
        weights += weight;
    }
    float blob=1.0-clamp(texture2D(sampler_04,v_texcoord).r,0.0,.6);
    gl_FragColor = vec4(vec3(total/max(weights,0.0001)*blob),1.0);
#endif
}
