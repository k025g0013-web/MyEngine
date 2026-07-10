#pragma once

//=====================================================================
// ベクトル構造体
//=====================================================================

// 2次元ベクトル
struct Vector2 {
	float x;
	float y;
};

// 3次元ベクトル
struct Vector3 {
	float x;
	float y;
	float z;
};

// 4次元ベクトル（RGBAカラーや同次座標などに使用）
struct Vector4 {
	float x;
	float y;
	float z;
	float w;
};

//=====================================================================
// イージング・補間・ベクトル計算で使用する演算子オーバーロード
//=====================================================================

//====================
// Vector2
//====================

// 加算
inline Vector2 operator+(const Vector2 &lhs, const Vector2 &rhs) {
	return { lhs.x + rhs.x, lhs.y + rhs.y };
}

// 減算
inline Vector2 operator-(const Vector2 &lhs, const Vector2 &rhs) {
	return { lhs.x - rhs.x, lhs.y - rhs.y };
}

// スカラー倍
inline Vector2 operator*(const Vector2 &lhs, float scalar) {
	return { lhs.x * scalar, lhs.y * scalar };
}

// スカラー倍（左辺がfloat）
inline Vector2 operator*(float scalar, const Vector2 &rhs) {
	return { rhs.x * scalar, rhs.y * scalar };
}

//====================
// Vector3
//====================

// 加算
inline Vector3 operator+(const Vector3 &lhs, const Vector3 &rhs) {
	return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
}

// 減算
inline Vector3 operator-(const Vector3 &lhs, const Vector3 &rhs) {
	return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
}

// スカラー倍
inline Vector3 operator*(const Vector3 &lhs, float scalar) {
	return { lhs.x * scalar, lhs.y * scalar, lhs.z * scalar };
}

// スカラー倍（左辺がfloat）
inline Vector3 operator*(float scalar, const Vector3 &rhs) {
	return { rhs.x * scalar, rhs.y * scalar, rhs.z * scalar };
}

//====================
// Vector4
//====================

// 加算
inline Vector4 operator+(const Vector4 &lhs, const Vector4 &rhs) {
	return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w };
}

// 減算
inline Vector4 operator-(const Vector4 &lhs, const Vector4 &rhs) {
	return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w };
}

// スカラー倍
inline Vector4 operator*(const Vector4 &lhs, float scalar) {
	return { lhs.x * scalar, lhs.y * scalar, lhs.z * scalar, lhs.w * scalar };
}

// スカラー倍（左辺がfloat）
inline Vector4 operator*(float scalar, const Vector4 &rhs) {
	return { rhs.x * scalar, rhs.y * scalar, rhs.z * scalar, rhs.w * scalar };
}