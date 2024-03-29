void main()
{
    lowp vec4 splatting_color = texture2D(sampler_04, v_texcoord);
    lowp vec4 diffuse_color = vec4(0.0, 0.0, 0.0, 1.0);
    
    if(splatting_color.x > 0.0)
    {
        diffuse_color = diffuse_color + texture2D(sampler_03, v_texcoord_scaled) * splatting_color.x;
    }
    if(splatting_color.y > 0.0)
    {
        diffuse_color = diffuse_color + texture2D(sampler_01, v_texcoord_scaled) * splatting_color.y;
    }
    if(splatting_color.z > 0.0)
    {
        diffuse_color = diffuse_color + texture2D(sampler_02, v_texcoord_scaled) * splatting_color.z;
    }
    
    gl_FragColor = diffuse_color;
}
