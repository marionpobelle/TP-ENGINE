#pragma once

#include "engine/utils/types_3d.h"

class YPerlin
{
public:
	YVec3f * Gradients;
	int Width;
	int Size;
	float Freq;

	YPerlin()
	{
		Width = 41;
		Size = Width * Width*Width;
		Gradients = new YVec3f[Size];
		updateVecs();
		Freq = 1;
	}

	void updateVecs() {
		for (int i = 0; i < Size; i++)
		{
			Gradients[i].X = (float)randf();
			Gradients[i].Y = (float)randf();
			Gradients[i].Z = (float)randf();
		}
	}

	float lerp(float a, float b, float alpha) {
		float alphaSmooth = alpha * (3 * alpha - 2 * alpha*alpha);
		return (1 - alphaSmooth)*a + alphaSmooth * b;
	}
public:

	void setFreq(float freq)
	{
		Freq = freq;
	}

	virtual float sample(float xBase, float yBase, float zBase)
	{
		float x = xBase * Freq;
		float y = yBase * Freq;
		float z = zBase * Freq;

		while (x < 0)
			x += Width;
		while (y < 0)
			y += Width;
		while (z < 0)
			z += Width;

		while (x >= Width)
			x -= Width;
		while (y >= Width)
			y -= Width;
		while (z >= Width)
			z -= Width;

		int x1 = (int)floor(x);
		int x2 = (int)floor(x) + 1;
		int y1 = (int)floor(y);
		int y2 = (int)floor(y) + 1;
		int z1 = (int)floor(z);
		int z2 = (int)floor(z) + 1;

		float dx = abs(x - x1);
		float dy = abs(y - y1);
		float dz = abs(z - z1);

		YVec3f pos(x, y, z);

		YVec3f sommetsPos[8];
		//plan X2
		sommetsPos[0] = YVec3f((float)x2, (float)y1, (float)z1);
		sommetsPos[1] = YVec3f((float)x2, (float)y1, (float)z2);
		sommetsPos[2] = YVec3f((float)x2, (float)y2, (float)z2);
		sommetsPos[3] = YVec3f((float)x2, (float)y2, (float)z1);

		//plan X1
		sommetsPos[4] = YVec3f((float)x1, (float)y1, (float)z1);
		sommetsPos[5] = YVec3f((float)x1, (float)y1, (float)z2);
		sommetsPos[6] = YVec3f((float)x1, (float)y2, (float)z2);
		sommetsPos[7] = YVec3f((float)x1, (float)y2, (float)z1);

		//Pour reboucler correctement
		x2 = x2 % (Width);
		y2 = y2 % (Width);
		z2 = z2 % (Width);

		YVec3f sommetsSample[8];
		//plan X2
		sommetsSample[0] = YVec3f((float)x2, (float)y1, (float)z1);
		sommetsSample[1] = YVec3f((float)x2, (float)y1, (float)z2);
		sommetsSample[2] = YVec3f((float)x2, (float)y2, (float)z2);
		sommetsSample[3] = YVec3f((float)x2, (float)y2, (float)z1);

		//plan X1
		sommetsSample[4] = YVec3f((float)x1, (float)y1, (float)z1);
		sommetsSample[5] = YVec3f((float)x1, (float)y1, (float)z2);
		sommetsSample[6] = YVec3f((float)x1, (float)y2, (float)z2);
		sommetsSample[7] = YVec3f((float)x1, (float)y2, (float)z1);

		float angles[8];
		for (int i = 0; i < 8; i++)
			angles[i] = (pos - sommetsPos[i]).dot(Gradients[(int)(sommetsSample[i].X * Width * Width + sommetsSample[i].Y * Width + sommetsSample[i].Z)]);

		//plan X2
		float ybas = lerp(angles[0], angles[3], dy);
		float yhaut = lerp(angles[1], angles[2], dy);
		float mid2 = lerp(ybas, yhaut, dz);

		//plan X1
		ybas = lerp(angles[4], angles[7], dy);
		yhaut = lerp(angles[5], angles[6], dy);
		float mid1 = lerp(ybas, yhaut, dz);

		float res = lerp(mid1, mid2, dx);

		res = (res + 1) / 2.0f;

		//Milieu
		return min(1, max(0, res));
	}
};
