“Hidden/Error”

struct Attributes
{
	vec4 vertex : POSITION;
	vec2 texcoord : TEXCOORD0;
};

struct Varyings
{
	vec4 vertex : SV_POSITION;
	vec2 texcoord : TEXCOORD0;
};

Varyings Vert(Attributes v)
{
	Varyings o;
	o.vertex = vec4(v.vertex.xy * 2.0 - 1.0, 0.0, 1.0);
	o.texcoord = v.vertex.xy;

	return o;
}

vec4 Pixel1(Varyings IN)
{
	return vec4(1.0, 0.0, 0.0, 1.0);
}

vec4 Pixel2(Varyings IN)
{
	return vec4(1.0, 0.0, 0.0, 1.0);
}

technique ErrorPass1
{
	VertexShader = Vert();
	PixelShader = Pixel1();
}

technique ErrorPass2
{
	VertexShader = Vert();
	PixelShader = Pixel2();
}
