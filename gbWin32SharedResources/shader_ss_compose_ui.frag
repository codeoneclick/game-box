void main()
{
    vec4 scene_color = texture2D(sampler_01, v_texcoord);
    vec4 ui_color = texture2D(sampler_02, v_texcoord);
    gl_FragColor = mix(scene_color, ui_color, ui_color.a);
}

