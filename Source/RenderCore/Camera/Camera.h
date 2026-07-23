#pragma once

#include "Math/Matrix.h"
#include "Math/Transform.h"

namespace Kizuna {
	/// <summary>
	/// シーンを描画するためのカメラを管理するクラス
	/// </summary>
	/// <remarks>
	/// 通常カメラとデバッグカメラの2種類を切り替えて使用できる。
	/// ビュー行列・射影行列の生成および、入力デバイスによる
	/// デバッグ操作を担当する。
	/// </remarks>
	class Camera {
	public:
		virtual ~Camera() = default;

		/// <summary>
		/// カメラを初期化する
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
		/// カメラに使うMatrix群を更新する
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