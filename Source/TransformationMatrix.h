#pragma once

#include "Math/Matrix.h"

struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
};