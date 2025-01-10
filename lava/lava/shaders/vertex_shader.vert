#version 450

layout(location = 0) in vec2 position;

// It is also possibile to pass data from vertex shader to fragment shader using the same pattern:
// There must be a variable out of a certain type in vertex and a certain location
// and another variable in of a certain type in the fragment shader at the same location
// Since vertex shader is executed for each shader and fragment after interpolation, we need to combine data
// using linear interpolation in order to perform conversion, where the coordinates x and y and in this case RGB
// This is because Vulkan makes an interpolation using Barycentric coordinates of the values passing from
// vertex to fragment shader, because each fragment has got a different position inside the triangle

layout(location = 1) in vec3 color;

layout(location = 0) out vec3 fragmentColor; // Also if again of location 0, there's no overlap between in and out variables

layout(push_constant) uniform PushConstant
{
    mat2 transform;
    vec2 offset;
    vec3 color;
} pushConstants;


// Executed for each vertex
// Receives input from input assembler
void main()
{
    // OpenGL has position (0, 0) in the screen center, with (-1, -1) on top left
    
    // pushConstants.transform * position means that, given triangle defined in a "general way"(normalized, model space)
    // it receives a transform applied on it in the world space
    
    gl_Position = vec4(pushConstants.transform * position + pushConstants.offset, 0.0, 1.0);
    
    // Not used with push constants
    //fragmentColor = color;
}
