#pragma once

#include "Math/Transform.h"

namespace Kizuna {

	class CameraManager;

	/// <summary>
	/// カメラ状態の基底クラス
	/// </summary>
	/// <remarks>
	/// 現在使用するカメラの更新処理および、
	/// 状態遷移を定義する。
	/// </remarks>
	class CameraState {
	public:
		virtual ~CameraState() = default;

		/// <summary>
		/// 現在の状態を更新する
		/// </summary>
		/// <param name="manager">
		/// カメラマネージャ
		/// </param>
		/// <param name="transform">
		/// カメラで使用するTransform
		/// </param>
		virtual void Update(
			CameraManager &manager,
			const Transform &transform) = 0;

		/// <summary>
		/// 次の状態へ切り替える
		/// </summary>
		/// <param name="manager">
		/// カメラマネージャ
		/// </param>
		virtual void Toggle(CameraManager &manager) = 0;

		/// <summary>
		/// 使用しているカメラの名前を取得
		/// </summary>
		virtual const char *GetName() const = 0;
	};
}