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

// Matches with descriptor set layout
layout (set = 0, binding = 0) uniform GlobalUniformBuffer
{
    mat4 projectionViewMatrix;
    vec4 ambientLightCol; // w is intensity
    vec3 pointLightPos;
    vec4 pointLightCol; // w is intensity
} ubo;

// Output data

layout(location = 0) out vec3 fragmentColor; // Also if again of location 0, there's no overlap between in and out variables

layout(push_constant) uniform PushConstant
{
    mat4 modelMatrix;
    mat4 normalMatrix;
} pushConstants;

// Executed for each vertex
// Receives input from input assembler
void main()
{
    // OpenGL has position (0, 0) in the screen center, with (-1, -1) on top left
    
    // "position" is the position of the vertex received in the shader
    // pushConstants.transform * position means that, given triangle defined in a "general way"(normalized, model space)
    // it receives a transform applied on it in the world space

    vec4 positionInWorldSpace = pushConstants.modelMatrix * vec4(position, 1.0);
    
    gl_Position = ubo.projectionViewMatrix * positionInWorldSpace;
    
    // Conversion to mat3 deletes row4 and col4. Not generally correct implementastion
    vec3 normalWorldSpace = normalize(mat3(pushConstants.normalMatrix) * normal);

    vec3 directionToLight = ubo.pointLightPos - positionInWorldSpace.xyz; // do not consider w

    // The dot prod of a vector for itself is an efficient way to compute the vetor squared
    // Attenutation to be computed before the normalization vector
    float attentuationFactor = 1.0 / dot(directionToLight, directionToLight);

    // Scale lights for their intesity
    vec3 lightColor = ubo.pointLightCol.xyz * ubo.pointLightCol.w;
    vec3 ambientLight = ubo.ambientLightCol.xyz * ubo.ambientLightCol.w;

    vec3 diffuseLight = lightColor * max(dot(normalWorldSpace, normalize(directionToLight)), 0);

    fragmentColor = (diffuseLight * ambientLight) * color;
}
