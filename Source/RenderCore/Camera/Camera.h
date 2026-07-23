#pragma once

#include "Math/Matrix.h"
#include "Math/Transform.h"

namespace Kizuna {
	/// <summary>
	/// カメラの基底クラス
	/// </summary>
	/// <remarks>
	/// すべてのカメラで共通となる画面サイズ、ビュー行列、
	/// 射影行列、ViewProjection行列の管理を行う。
	/// 派生クラスはUpdate()を実装し、カメラ固有の更新処理を行う。
	/// </remarks>
	class Camera {
	public:
		virtual ~Camera() = default;

		/// <summary>
		/// 描画に使用する画面サイズを設定する
		/// </summary>
		/// <param name="width">画面幅</param>
		/// <param name="height">画面高さ</param>
		virtual void SetScreenSize(float width, float height);

		/// <summary>
		/// カメラを更新する
		/// </summary>
		/// <param name="transform">
		/// カメラで使用するTransform
		/// </param>
		virtual void Update(const Transform &transform) = 0;

		/// <summary>
		/// ビュー行列を取得する
		/// </summary>
		const Matrix4x4 &GetViewMatrix() const { return viewMatrix_; }

		/// <summary>
		/// 射影行列を取得する
		/// </summary>
		const Matrix4x4 &GetProjectionMatrix() const { return projectionMatrix_; }

		/// <summary>
		/// ViewProjection行列を取得する
		/// </summary>
		const Matrix4x4 &GetViewProjectionMatrix() const { return viewProjectionMatrix_; }

	protected:
		/// <summary>
		/// Transformからビュー行列・射影行列・ViewProjection行列を更新する
		/// </summary>
		/// <param name="transform">
		/// カメラで使用するTransform
		/// </param>
		void UpdateMatrices(const Transform &transform);

	protected:
		/// ビュー行列
		Matrix4x4 viewMatrix_{};
		/// 射影行列
		Matrix4x4 projectionMatrix_{};
		/// ViewProjection行列
		Matrix4x4 viewProjectionMatrix_{};

		/// 画面サイズ
		float width_ = 1280.0f;
		float height_ = 720.0f;

		/// カメラ設定
		float fovY_ = 0.45f;
		float nearClip_ = 0.1f;
		float farClip_ = 100.0f;
	};
}