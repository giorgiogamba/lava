#version 450

// Input data

layout(location = 0) in vec3 position;

// It is also possibile to pass data from vertex shader to fragment shader using the same pattern:
// There must be a variable out of a certain type in vertex and a certain location
// and another variable in of a certain type in the fragment shader at the same location
// Since vertex shader is executed for each shader and fragment after interpolation, we need to combine data
// using linear interpolation in order to perform conversion, where the coordinates x and y and in this case RGB
// This is because Vulkan makes an interpolation using Barycentric coordinates of the values passing from
// vertex to fragment shader, because each fragment has got a different position inside the triangle

layout(location = 1) in vec3 color;

layout(location = 2) in vec3 normal;

layout(location = 3) in vec2 uv;

// Output data

layout(location = 0) out vec3 fragmentColor; // Also if again of location 0, there's no overlap between in and out variables

layout(push_constant) uniform PushConstant
{
    mat4 modelMatrix;
    mat4 normalMatrix;
} pushConstants;

// DIRECT LIGHTNING SIMULATION
// Each vertex is hit by the same raylight. Simulates a light that is infinitely far away, like the sun, and parallel
const vec3 DIRECTION_TO_LIGHT = normalize(vec3(1.0, -3.0, -1.0));

// INDIRECT LIGHTNING SIMULATION (AMBIENT)
const float AMBIENT_INTENSITY = 0.02;

// Executed for each vertex
// Receives input from input assembler
void main()
{
    // OpenGL has position (0, 0) in the screen center, with (-1, -1) on top left
    
    // "position" is the position of the vertex received in the shader
    // pushConstants.transform * position means that, given triangle defined in a "general way"(normalized, model space)
    // it receives a transform applied on it in the world space
    
    gl_Position = ubo.projectionViewMatrix * pushConstants.modelMatrix * vec4(position, 1.0);
    
    // Conversion to mat3 deletes row4 and col4. Not generally correct implementastion
    vec3 normalWorldSpace = normalize(mat3(pushConstants.normalMatrix) * normal);

    // Minimized to 0 in case vector is not facing light
    float lightIntensity = max(dot(normalWorldSpace, DIRECTION_TO_LIGHT), 0);

    // Not used with push constants
    const float totalLightIntensity = AMBIENT_INTENSITY + lightIntensity;
    fragmentColor = totalLightIntensity * color;
}
