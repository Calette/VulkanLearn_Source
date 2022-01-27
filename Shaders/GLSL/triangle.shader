cbuffer ubo 
{
    mat4 model;
    mat4 view;
    mat4 proj;
};

struct Attributes
{
	vec2 inPosition;
	vec3 inColor;
    vec2 inTexCoord;
};

struct Varyings
{
    vec4 gl_Position;
	vec2 fragColor;
	vec2 fragTexCoord;
};

struct Output
{
	vec4 vertex;
	vec2 texcoord;
};

Varyings vert(Attributes i)
{
    Varyings o;
    o.gl_Position = ubo.proj * ubo.view * ubo.model * vec4(i.inPosition, 0.0, 1.0);
    o.fragColor = i.inColor;
	o.fragTexCoord = i.inTexCoord;
}

layout(location = 0) out vec4 outColor;

void frag() 
{
    outColor = vec4(fragTexCoord, 0.0, 1.0);
}