#pragma once

namespace Palette
{
	struct Vecter4
	{
	public:
		float x;
		float y;
		float z;
		float w;

		Vecter4(float xx, float yy, float zz, float ww)
		{
			x = xx; y = yy; z = zz; w = ww;
		};

		static Vecter4 Zero;
	};

	struct Vecter3
	{
	public:
		float x;
		float y;
		float z;

		Vecter3(float xx, float yy, float zz) 
		{
			x = xx; y = yy; z = zz;
		};

		static Vecter3 Zero;
	};
}
