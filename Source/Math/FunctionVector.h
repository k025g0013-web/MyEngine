#pragma once

#define _USE_MATH_DEFINES
#include <random>
#include <algorithm>
#include <cassert>

#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Math/Transform.h"

namespace Math {
#ifndef MATH_EPSILON_DEFINED
#define MATH_EPSILON_DEFINED
	inline constexpr float kEpsilon = 1e-4f;
#endif

	// トレイト定義
	template <typename T> struct VectorTraits;
	template <> struct VectorTraits<Vector2> { static constexpr size_t Dimensions = 2; };
	template <> struct VectorTraits<Vector3> { static constexpr size_t Dimensions = 3; };
	template <> struct VectorTraits<Vector4> { static constexpr size_t Dimensions = 4; };

	// 計算関数群
	//===============
	// 加算
	template <typename VectorN>
	inline VectorN AddVector(const VectorN &v1, const VectorN &v2) {
		return v1 + v2;
	}

	// 減算
	template <typename VectorN>
	inline VectorN SubtractVector(const VectorN &v1, const VectorN &v2) {
		return v1 - v2;
	}

	// スカラー倍
	template <typename VectorN>
	inline VectorN Multiply(float scalar, const VectorN &v) {
		return scalar * v;
	}

	// 内積
	template <typename VectorN>
	inline float Dot(const VectorN &v1, const VectorN &v2) {
		constexpr size_t dim = VectorTraits<VectorN>::Dimensions;

		if constexpr (dim == 2) {
			return v1.x * v2.x + v1.y * v2.y;
		} else if constexpr (dim == 3) {
			return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
		} else if constexpr (dim == 4) {
			return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
		}
	}

	// 長さの二乗
	template <typename VectorN>
	inline float LengthSquare(const VectorN &v) {
		return Dot(v, v);
	}

	// 長さ
	template <typename VectorN>
	inline float Length(const VectorN &v) {
		return std::sqrtf(LengthSquare(v));
	}

	// 正規化
	template <typename VectorN>
	inline VectorN Normalize(const VectorN &v) {
		float length = Length(v);

		if (length < kEpsilon) {
			return {};
		}

		return Multiply(1.0f / length, v);
	}

	// 特定次元専用関数群
	//===============
	// クロス積 (Vector3)
	template <typename Vector3T>
	inline Vector3T Cross(const Vector3T &v1, const Vector3T &v2) {
		static_assert(VectorTraits<Vector3T>::Dimensions == 3, "Cross product is only supported for Vector3.");

		Vector3T result = {};
		result.x = v1.y * v2.z - v1.z * v2.y;
		result.y = v1.z * v2.x - v1.x * v2.z;
		result.z = v1.x * v2.y - v1.y * v2.x;
		return result;
	}

	// 座標変換 (Vector3)
	inline Vector3 TransformVector3(const Vector3 &vector, const Matrix4x4 &matrix) {
		Vector3 result = {};

		result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + matrix.m[3][0];
		result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + matrix.m[3][1];
		result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + matrix.m[3][2];

		float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + matrix.m[3][3];
		assert(w != 0.0f);
		result.x /= w;
		result.y /= w;
		result.z /= w;

		return result;
	}
}