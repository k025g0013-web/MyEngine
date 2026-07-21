#pragma once

#define _USE_MATH_DEFINES
#include <random>
#include <algorithm>
#include <cassert>

#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Math/Transform.h"

namespace Kizuna {
	namespace Math {
#ifndef MATH_EPSILON_DEFINED
#define MATH_EPSILON_DEFINED
		// 浮動小数点の比較に使用する許容誤差
		inline constexpr float kEpsilon = 1e-4f;
#endif

		//==============================
		// ベクトル型ごとの次元情報
		//==============================

		// ベクトルの次元数を取得するためのトレイト
		template <typename T> struct VectorTraits;

		// Vector2は2次元ベクトル
		template <> struct VectorTraits<Vector2> {
			static constexpr size_t Dimensions = 2;
		};

		// Vector3は3次元ベクトル
		template <> struct VectorTraits<Vector3> {
			static constexpr size_t Dimensions = 3;
		};

		// Vector4は4次元ベクトル
		template <> struct VectorTraits<Vector4> {
			static constexpr size_t Dimensions = 4;
		};

		//==============================
		// 計算関数群
		//==============================

		// ベクトル同士を加算する
		template <typename VectorN>
		inline VectorN AddVector(const VectorN &v1, const VectorN &v2) {
			return v1 + v2;
		}

		// ベクトル同士を減算する
		template <typename VectorN>
		inline VectorN SubtractVector(const VectorN &v1, const VectorN &v2) {
			return v1 - v2;
		}

		// ベクトルをスカラー倍する
		template <typename VectorN>
		inline VectorN Multiply(float scalar, const VectorN &v) {
			return scalar * v;
		}

		// ベクトル同士の内積を求める
		template <typename VectorN>
		inline float Dot(const VectorN &v1, const VectorN &v2) {

			// ベクトルの次元数を取得する
			constexpr size_t dim = VectorTraits<VectorN>::Dimensions;

			// Vector2
			if constexpr (dim == 2) {
				return v1.x * v2.x + v1.y * v2.y;

				// Vector3
			} else if constexpr (dim == 3) {
				return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;

				// Vector4
			} else if constexpr (dim == 4) {
				return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
			}
		}

		// ベクトルの長さの二乗を求める
		template <typename VectorN>
		inline float LengthSquare(const VectorN &v) {

			// 自分自身との内積を利用する
			return Dot(v, v);
		}

		// ベクトルの長さ(ノルム)を求める
		template <typename VectorN>
		inline float Length(const VectorN &v) {

			// 長さの二乗の平方根を求める
			return std::sqrtf(LengthSquare(v));
		}

		// ベクトルを正規化する
		template <typename VectorN>
		inline VectorN Normalize(const VectorN &v) {

			// ベクトルの長さを取得する
			float length = Length(v);

			// 長さがほぼ0なら0ベクトルを返す
			if (length < kEpsilon) {
				return {};
			}

			// 長さが1になるよう各成分を割る
			return Multiply(1.0f / length, v);
		}

		//==============================
		// 特定次元専用関数群
		//==============================

		// 3次元ベクトル同士のクロス積を求める
		template <typename Vector3T>
		inline Vector3T Cross(const Vector3T &v1, const Vector3T &v2) {

			// Vector3以外では使用できないようにする
			static_assert(VectorTraits<Vector3T>::Dimensions == 3, "Cross product is only supported for Vector3.");

			Vector3T result = {};

			// x成分
			result.x = v1.y * v2.z - v1.z * v2.y;

			// y成分
			result.y = v1.z * v2.x - v1.x * v2.z;

			// z成分
			result.z = v1.x * v2.y - v1.y * v2.x;

			return result;
		}

		// Vector3を4×4行列で座標変換する
		inline Vector3 TransformVector3(const Vector3 &vector, const Matrix4x4 &matrix) {
			Vector3 result = {};

			// x座標を変換する
			result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + matrix.m[3][0];

			// y座標を変換する
			result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + matrix.m[3][1];

			// z座標を変換する
			result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + matrix.m[3][2];

			// 同次座標系のw成分を計算する
			float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + matrix.m[3][3];

			// wが0でないことを確認する
			assert(w != 0.0f);

			// 同次座標から3次元座標へ戻す
			result.x /= w;
			result.y /= w;
			result.z /= w;

			return result;
		}
	}
}