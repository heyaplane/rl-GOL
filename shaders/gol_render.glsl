#version 430

in vec2 fragTexCoord;
out vec4 finalColor;

layout(std430, binding = 1) readonly buffer golLayout
{
    uint golBuffer[];
};

uniform vec2 resolution;

void main()
{
    ivec2 coords = ivec2(fragTexCoord * resolution);

    if ((golBuffer[coords.x + coords.y * uvec2(resolution).x]) == 1) finalColor = vec4(1.0);
    else finalColor = vec4(0.0, 0.0, 0.0, 1.0);
}
