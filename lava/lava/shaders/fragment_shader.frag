#version 450

layout(location = 0) in vec3 fragmentColor;

// We define location because fragment shader can output on different locations
layout(location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(fragmentColor,  1.0);
}
