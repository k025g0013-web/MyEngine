#pragma once

#include "Math/Matrix.h"

namespace Kizuna {
	/// <summary>
	/// シェーダーへ渡す変換行列を保持する構造体
	/// </summary>
	/// <remarks>
	/// 定数バッファ(Constant Buffer)としてGPUへ転送され、
	/// オブジェクトの座標変換に使用される。
	/// </remarks>
	struct TransformationMatrix {

		/// ワールド・ビュー・プロジェクション行列
		Matrix4x4 WVP;

		/// ワールド行列
		Matrix4x4 World;
	};
}