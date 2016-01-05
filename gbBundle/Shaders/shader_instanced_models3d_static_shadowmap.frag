#if defined(__OPENGL_30__)

in vec2 v_texcoord;
in vec4 v_shadow_parameters;

#else

varying vec2 v_texcoord;
varying vec4 v_shadow_parameters;

#endif

uniform sampler2D sampler_01;
uniform sampler2DShadow sampler_02;

void main(void)
{
#if defined(__IOS__)
    
    float shadow = shadow2DProjEXT(sampler_02, v_shadow_parameters);
    
#else
    
    float shadow = textureProj(sampler_02, v_shadow_parameters);
    
#endif
    
    vec4 color = texture2D(sampler_01, v_texcoord);
    color.rgb *= clamp(shadow, 0.33, 1.0);
    gl_FragColor = color;
}