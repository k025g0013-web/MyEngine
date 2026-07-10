#pragma once

#include "Vector.h"
#include "Matrix.h"

//=====================================================================
// オブジェクトのTransform情報
//=====================================================================
// Scale・Rotate・Translateを1つにまとめた構造体。
// モデルやカメラなど位置を持つオブジェクトで共通利用する。
struct Transform {

	// 拡大縮小率
	Vector3 scale{ 1.0f, 1.0f, 1.0f };

	// 回転量（ラジアン）
	Vector3 rotate{ 0.0f,0.0f,0.0f };

	// ワールド座標
	Vector3 translate{ 0.0f,0.0f,0.0f };
};