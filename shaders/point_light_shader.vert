#version 450

// Represents a quad for the gizmo
const vec2 Gizmo[6] = vec2[]
(
    vec2(-1.0, -1.0),
    vec2(-1.0, 1.0),
    vec2(1.0, -1.0),
    vec2(1.0, -1.0),
    vec2(-1.0, 1.0),
    vec2(1.0, 1.0)
);

layout (location = 0) out vec2 FragmentOffset;

layout (set = 0, binding = 0) uniform GlobalUniformBuffer
{
    mat4 ProjectionMatrix;
    mat4 ViewMatrix;
    vec4 AmbientColorLight;
    vec3 LightPosition;
    vec3 LightColor;
} UniformBuffer;

const float LightRadius = 0.1;

void main()
{
    FragmentOffset = Gizmo[gl_VertexIndex];

    vec3 CameraRightWorld = {UniformBuffer.ViewMatrix[0][0], UniformBuffer.ViewMatrix[1][0], UniformBuffer.ViewMatrix[2][0]};
    vec3 CameraUpWorld = {UniformBuffer.ViewMatrix[0][1], UniformBuffer.ViewMatrix[1][1], UniformBuffer.ViewMatrix[2][1]};

    // NOTE: Remember that this operations are applied for every vertex
    vec3 PositionWorld = UniformBuffer.LightPosition.xyz;
    PositionWorld += LightRadius * FragmentOffset.x * CameraRightWorld;
    PositionWorld += LightRadius * FragmentOffset.y * CameraUpWorld;

    gl_Position = UniformBuffer.ProjectionMatrix * UniformBuffer.ViewMatrix * vec4(PositionWorld, 1.0); 
}