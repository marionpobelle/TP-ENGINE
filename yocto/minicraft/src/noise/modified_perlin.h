#pragma once

#include "engine/noise/perlin.h"

class MPerlin : public YPerlin
{
public:

	float ZDecay_ZMax;
	float ZDecay_Cut;
	bool DoPenaltyBottom;
	bool DoPenaltyMiddle;
	bool DoPenaltySky;

	MPerlin() : YPerlin()
	{
		ZDecay_ZMax = (float)Width;
		ZDecay_Cut = 1;
		Freq = 1;
		DoPenaltyBottom = true;
		DoPenaltyMiddle = true;
		DoPenaltySky = true;
	}

public:
	void setZDecay(float zMax, float cut)
	{
		ZDecay_ZMax = zMax;
		ZDecay_Cut = min(max(0, cut), 1);
	}

	float sample(float xBase, float yBase, float zBase)
	{
		float sample = YPerlin::sample(xBase, yBase, zBase);

		//Plus light plus on est haut
		float zDecay_Norm = zBase / ZDecay_ZMax;
		float penalty = 0;
		if (ZDecay_Cut < 1 && ZDecay_Cut >= 0) {
			//Cut au dessus 
			if (DoPenaltySky)
				penalty = (max(0, zDecay_Norm - ZDecay_Cut));

			//Remplis en dessous
			if (DoPenaltyMiddle && zDecay_Norm <= ZDecay_Cut) {
				penalty = (1 - (ZDecay_Cut - zDecay_Norm));
				penalty = -pow(penalty, 20);
				penalty /= 2;
			}

			//Remplis au sol
			if (DoPenaltyBottom && zDecay_Norm < 0.1f) {
				penalty = (0.1f - zDecay_Norm) / 0.1f;
				penalty = -pow(penalty, 3);
				penalty /= 4;
			}
		}

		sample -= (penalty / 4.0f);

		return min(1, max(0, sample));
	}
};