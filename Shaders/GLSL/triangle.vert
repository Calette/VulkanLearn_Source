#version 450
// #extension GL_ARB_shading_language_include : enable

// #include "test.h"

layout(set = 0, binding = 0) uniform GlobalUniformBuffer 
{
    mat4 view;
    mat4 proj;
} globalUniformBuffer;

layout(set = 1, binding = 0) uniform ObjectUniformBuffer 
{
    mat4 model;
} objectUniformBuffer;

// sturct AttributesTest
// {
//     vec3 inPosition;
//     vec3 inColor;
//     vec2 inTexCoord;
// };

// layout(location = 0) in AttributesTest inTest;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

vec4 TranslateModelToClip(vec3 position)
{
	return globalUniformBuffer.proj * globalUniformBuffer.view * objectUniformBuffer.model * vec4(position, 1.0);
}

void main() 
{
    gl_Position = TranslateModelToClip(inPosition);
    fragColor = inColor;
	fragTexCoord = inTexCoord;
}
