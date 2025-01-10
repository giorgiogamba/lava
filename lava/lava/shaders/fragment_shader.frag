#version 450

// Execute for each pixel in the triangle defined inside the buffer -> executed on GPU via SIMD

layout(location = 0) in vec3 fragmentColor;

// We define location because fragment shader can output on different locations
layout(location = 0) out vec4 outColor;

layout(push_constant) uniform PushConstant
{
    mat2 transform;
    vec2 offset;
    vec3 color;
} pushConstants;

void main()
{
    outColor = vec4(pushConstants.color,  1.0);
}
