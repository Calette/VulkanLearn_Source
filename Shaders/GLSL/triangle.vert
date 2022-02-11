#version 450
// #extension GL_ARB_shading_language_include : enable

// include "test.h"

layout(binding = 0) uniform GlobalUniformBuffer 
{
    mat4 model;
    mat4 view;
    mat4 proj;
} gub;

// layout(binding = 0) uniform ObjectUniformBuffer 
// {
//     mat4 model;
//     mat4 view;
//     mat4 proj;
// } oub;

// sturct AttributesTest
// {
//     vec2 inPosition;
//     vec3 inColor;
//     vec2 inTexCoord;
// };

// layout(location = 0) in AttributesTest inTest;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() 
{
    gl_Position = gub.proj * gub.view * gub.model * vec4(inPosition, 0.0, 1.0);
    fragColor = inColor;
	fragTexCoord = inTexCoord;
}
