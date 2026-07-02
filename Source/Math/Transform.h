#pragma once
#include "Vector.h"
#include "Matrix.h"

struct Transform {
	Vector3 scale{ 1.0f, 1.0f, 1.0f };
	Vector3 rotate{ 0.0f,0.0f,0.0f };
	Vector3 translate{ 0.0f,0.0f,0.0f };
};