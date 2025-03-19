#version 450

// Execute for each pixel in the triangle defined inside the buffer -> executed on GPU via SIMD

layout(location = 0) in vec3 fragmentColor;
layout(location = 1) in vec3 fragmentWorldPos;
layout(location = 2) in vec3 fragmentWorldNormal;

// We define location because fragment shader can output on different locations
layout(location = 0) out vec4 outColor;

layout(push_constant) uniform PushConstant
{
    mat4 modelMatrix;
    mat4 normalMatrix;
} pushConstants;

// Matches with descriptor set layout
layout (set = 0, binding = 0) uniform GlobalUniformBuffer
{
    mat4 projectionViewMatrix;
    vec4 ambientLightCol; // w is intensity
    vec3 pointLightPos;
    vec4 pointLightCol; // w is intensity
} ubo;

void main()
{
    vec3 directionToLight = ubo.pointLightPos - fragmentWorldPos; // do not consider w

    // The dot prod of a vector for itself is an efficient way to compute the vetor squared
    // Attenutation to be computed before the normalization vector
    float attentuationFactor = 1.0 / dot(directionToLight, directionToLight);

    // Scale lights for their intesity
    vec3 lightColor = ubo.pointLightCol.xyz * ubo.pointLightCol.w;
    vec3 ambientLight = ubo.ambientLightCol.xyz * ubo.ambientLightCol.w;

    // Linear interpolation of two normal vectors is not necessarly normal itself
    // that's why we re-normalize fragmentWorldNormal
    vec3 diffuseLight = lightColor * max(dot(normalize(fragmentWorldNormal), normalize(directionToLight)), 0);

    // Compute final color
    outColor = vec4((diffuseLight * ambientLight) * fragmentColor,  1.0);
}
