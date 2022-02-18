#pragma once

namespace Palette
{
	struct Vector4
	{
	public:
		float x;
		float y;
		float z;
		float w;

		Vector4(float xx, float yy, float zz, float ww)
		{
			x = xx; y = yy; z = zz; w = ww;
		};

		static Vector4 Zero;
	};

	struct Vector3
	{
	public:
		float x;
		float y;
		float z;

		Vector3(float xx, float yy, float zz) 
		{
			x = xx; y = yy; z = zz;
		};

		static Vector3 Zero;
	};
}
