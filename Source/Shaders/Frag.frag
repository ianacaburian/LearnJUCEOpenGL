#version 330 core

uniform float distance;
out vec4 out_color;

void main()
{
    vec2 resolution = vec2(800., 600.); // Corresponds to the screen dimensions
    vec2 p = (gl_FragCoord.xy - resolution) / distance;
    
    float thickness = 0.02;
    float radius = 0.5;
    float intensity = thickness/abs(radius - length(p));
    
    out_color = vec4(intensity, 0., intensity, .5);
}
