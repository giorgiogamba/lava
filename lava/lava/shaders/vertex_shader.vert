#version 450

vec2 positions[3] = vec2[](vec2(0.0, -0.5), vec2(0.5, 0.5), vec2(-0.5, 0.5));

// Executed for each vertex
// Receives input from input assembler
void main()
{
    // OpenGL has position (0, 0) in the screen center, with (-1, -1) on top left
    
    // Create position for gl_VertexIndex vertex with z-value 0.0 and normalized by 1.0 (nothing happens for now)
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}
