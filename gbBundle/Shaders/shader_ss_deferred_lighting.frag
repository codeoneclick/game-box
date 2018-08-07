void main()
{
    vec3 diffuse_color = texture2D(sampler_01, v_texcoord).rgb;
    vec3 diffuse_intensity = texture2D(sampler_02, v_texcoord).rgb;
    vec3 emissive_color = texture2D(sampler_03, v_texcoord).rgb;
    diffuse_color = diffuse_color * clamp(diffuse_intensity + emissive_color, vec3(0.33), vec3(1.0));
    gl_FragColor = vec4(diffuse_color, 1.0);
}

