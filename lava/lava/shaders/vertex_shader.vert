#version 450

layout(location = 0) in vec2 position;

// Executed for each vertex
// Receives input from input assembler
void main()
{
    // OpenGL has position (0, 0) in the screen center, with (-1, -1) on top left
    gl_Position = vec4(position, 0.0, 1.0);
}
