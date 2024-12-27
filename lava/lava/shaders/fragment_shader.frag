#version 450

// We define location because fragment shader can output on different locations
layout (location = 0) out vec4 outColor;

void main()
{
    otuColor = vec4(1.0, 0.0, 0.0, 1.0);
}
