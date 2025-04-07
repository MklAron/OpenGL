#version 430

uniform vec3 circleCenter;
uniform float lineOffsetY;
uniform bool Vonal;

out vec4 color;

void main(void)
{
    if (Vonal) {
        color = vec4(0.0, 0.1, 1.0, 1.0);
    }
    else {
        vec2 scaledFragCoord = gl_FragCoord.xy - vec2(300.0);
        vec2 scaledCircleCenter = circleCenter.xy * 300.0;
        float distance = length(scaledCircleCenter - scaledFragCoord);

        vec2 p0 = vec2(-1.0f / 3, lineOffsetY) * 300.0;
        vec2 p1 = vec2(1.0f / 3, lineOffsetY) * 300.0;
        float distance_p0 = length(scaledCircleCenter - p0);
        float distance_p1 = length(scaledCircleCenter - p1);

        bool closeToLeftEndpoint = circleCenter.x < -1.0f / 3 && distance_p0 < 50;
        bool closeToRightEndpoint = circleCenter.x > 1.0f / 3 && distance_p1 < 50;
        bool closeToMiddleSegment = circleCenter.x > -1.0f / 3 && circleCenter.x < 1.0f / 3 && abs(circleCenter.y - lineOffsetY) < 50 / 300.0;

        if (closeToLeftEndpoint || closeToRightEndpoint || closeToMiddleSegment) {
            color = vec4(distance / 45, 1.0 - distance / 45, 0.0, 1.0);
        }
        else {
            color = vec4(1.0 - distance / 45, distance / 45, 0.0, 1.0);
        }
    }
}
