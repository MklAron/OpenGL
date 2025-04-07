#version 430

uniform float offsetX;
uniform float offsetY;
uniform bool Vonal;
uniform float lineOffsetY;

layout (location = 0) in vec3 aPos;

void main(void)
{
    vec2 positionOffset = Vonal ? vec2(0.0, lineOffsetY) : vec2(offsetX, offsetY);
    gl_Position = vec4(aPos.xy + positionOffset, aPos.z, 1.0);
}
