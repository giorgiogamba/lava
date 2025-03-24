#version 450

layout (location = 0) in vec2 FragmentOffset;

layout (location = 0) out vec4 OutColor;

layout (set = 0, binding = 0) uniform GlobalUniformBuffer
{
    mat4 ProjectionMatrix;
    mat4 ViewMatrix;
    vec4 AmbientColorLight;
    vec3 LightPosition;
    vec3 LightColor;
} UniformBuffer;

void main()
{
    OutColor = vec4(UniformBuffer.LightColor.xyz, 1.0); 
}

