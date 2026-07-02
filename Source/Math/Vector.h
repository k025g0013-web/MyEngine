#pragma once

struct Vector2 {
	float x;
	float y;
};

struct Vector3 {
	float x;
	float y;
	float z;
};

struct Vector4 {
	float x;
	float y;
	float z;
	float w;
};

// イージング等の計算に必要な演算子の定義
//===============

// Vector2
inline Vector2 operator+(const Vector2 &lhs, const Vector2 &rhs) { return { lhs.x + rhs.x, lhs.y + rhs.y }; }
inline Vector2 operator-(const Vector2 &lhs, const Vector2 &rhs) { return { lhs.x - rhs.x, lhs.y - rhs.y }; }
inline Vector2 operator*(const Vector2 &lhs, float scalar) { return { lhs.x * scalar, lhs.y * scalar }; }
inline Vector2 operator*(float scalar, const Vector2 &rhs) { return { rhs.x * scalar, rhs.y * scalar }; }

// Vector3
inline Vector3 operator+(const Vector3 &lhs, const Vector3 &rhs) { return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z }; }
inline Vector3 operator-(const Vector3 &lhs, const Vector3 &rhs) { return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z }; }
inline Vector3 operator*(const Vector3 &lhs, float scalar) { return { lhs.x * scalar, lhs.y * scalar, lhs.z * scalar }; }
inline Vector3 operator*(float scalar, const Vector3 &rhs) { return { rhs.x * scalar, rhs.y * scalar, rhs.z * scalar }; }

// Vector4
inline Vector4 operator+(const Vector4 &lhs, const Vector4 &rhs) { return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w }; }
inline Vector4 operator-(const Vector4 &lhs, const Vector4 &rhs) { return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w }; }
inline Vector4 operator*(const Vector4 &lhs, float scalar) { return { lhs.x * scalar, lhs.y * scalar, lhs.z * scalar, lhs.w * scalar }; }
inline Vector4 operator*(float scalar, const Vector4 &rhs) { return { rhs.x * scalar, rhs.y * scalar, rhs.z * scalar, rhs.w * scalar }; }