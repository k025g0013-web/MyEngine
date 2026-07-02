#pragma once
#include <cstdlib>

inline float RandHelperFloat(float min, float max) {
	return min + (max - min) * (static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
}

inline int RandHelperInt(int min, int max) {
	if (max <= min) return min;

	return min + (rand() % (max - min + 1));
}