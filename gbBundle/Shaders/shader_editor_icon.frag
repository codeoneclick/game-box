vec3 editor_display_linear(vec3 c){c=max(c,vec3(0));return mix(c/12.92,pow((c+.055)/1.055,vec3(2.4)),step(vec3(.04045),c));}
void main()
{
    vec4 color = texture2D(sampler_01, v_texcoord) * v_color;
color.rgb = editor_display_linear(color.rgb);
    gl_FragColor = color;
}
