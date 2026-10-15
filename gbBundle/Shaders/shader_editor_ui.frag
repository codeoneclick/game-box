vec3 editor_display_linear(vec3 c){c=max(c,vec3(0));return mix(c/12.92,pow((c+.055)/1.055,vec3(2.4)),step(vec3(.04045),c));}
// Pixel-sized SDF radii keep wide panels and small knobs equally round.
// The outline and selection glow are shaded in this one native sprite draw.
void main()
{
    vec2 size = 1.0 / max(fwidth(v_texcoord), vec2(0.00001));
    float radius = min(12.0, min(size.x, size.y) * 0.5 - 1.0);
    vec2 p = (v_texcoord - 0.5) * size;
    vec2 q = abs(p) - (size * 0.5 - vec2(2.0 + radius));
    float distance = length(max(q, 0.0)) + min(max(q.x, q.y), 0.0) - radius;
    float selected = step(0.45, v_color.g) * step(0.55, v_color.b) * (1.0 - step(0.4, v_color.r));
    float edge = 1.0 - smoothstep(0.7, 1.7, abs(distance + 0.8));
    vec3 border = mix(vec3(0.32, 0.53, 0.61), vec3(0.30, 0.88, 1.0), selected);
    vec3 fill = v_color.rgb * (1.0 + (0.5 - v_texcoord.y) * 0.16);
    vec3 color = mix(fill, border, edge * mix(0.7, 1.0, selected));
    float alpha = (1.0 - smoothstep(-0.5, 0.7, distance)) * v_color.a;
    float glow = selected * exp(-abs(distance) * 0.65) * 0.45;
    color += vec3(0.04, 0.30, 0.36) * glow;
color = editor_display_linear(color);
    gl_FragColor = vec4(color, max(alpha, glow));
}
