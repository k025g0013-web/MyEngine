#pragma once
#include <cstdint>
#include <memory>
#include <string>

// === Core ===
#include "Core/WinApp.h"
#include "Core/Logger.h"
#include "Core/DebugManager.h"

// === Graphics ===
#include "Graphics/DirectXCommon.h"
#include "Graphics/Pipeline/PipelineManager.h"
#include "Graphics/Resource/Texture.h"

// === RenderCore ===
#include "RenderCore/Lighting.h"

// === Audio ===
#include "Audio/AudioManager.h"

// === Input ===
#include "Input/Keyboard.h"
#include "Input/Mouse.h"
#include "Input/GamePad.h"

namespace Kizuna {
	/// <summary>
	/// KizunaEngine全体を管理するエンジンクラス
	/// </summary>
	/// <remarks>
	/// ウィンドウ、DirectX12、入力、描画、オーディオなどの
	/// 各サブシステムの生成・初期化・更新・終了処理を統括する。
	/// アプリケーションは本クラスを介して各機能へアクセスする。
	/// </remarks>
	class KizunaEngine {
	public:
		/// <summary>
		/// エンジンを初期化する
		/// </summary>
		/// <param name="title">ウィンドウタイトル</param>
		/// <param name="width">クライアント領域の横幅</param>
		/// <param name="height">クライアント領域の縦幅</param>
		void Initialize(const std::wstring &title, int32_t width, int32_t height);

		/// <summary>
		/// エンジンを終了する
		/// </summary>
		/// <remarks>
		/// 生成した各サブシステムおよびリソースを解放する。
		/// </remarks>
		void Finalize();

		/// <summary>
		/// フレーム開始処理を行う
		/// </summary>
		void BeginFrame();

		/// <summary>
		/// フレーム終了処理を行う
		/// </summary>
		void EndFrame();

		/// <summary>
		/// Windowsメッセージを処理する
		/// </summary>
		/// <returns>
		/// アプリケーションを継続する場合はtrue、
		/// 終了要求を受けた場合はfalseを返す。
		/// </returns>
		bool ProcessMessage();

		/// <summary>
		/// 入力デバイスの状態を更新する
		/// </summary>
		/// <remarks>
		/// キーボード、マウス、ゲームパッドの入力状態を更新する。
		/// </remarks>
		void UpdateInput();

		/// <summary>
		/// 描画パイプラインを設定する
		/// </summary>
		/// <param name="type">設定するパイプラインの種類</param>
		void SetPipeline(PipelineType type);

		/// <summary>
		/// WinAppクラスを取得する
		/// </summary>
		/// <returns>WinAppクラス</returns>
		WinApp *GetWinApp() const { return winApp_.get(); }

		/// <summary>
		/// DirectXCommonクラスを取得する
		/// </summary>
		/// <returns>DirectXCommonクラス</returns>
		DirectXCommon *GetDirectXCommon() const { return directXCommon_.get(); }

		/// <summary>
		/// DirectX12デバイスを取得する
		/// </summary>
		/// <returns>DirectX12デバイス</returns>
		ID3D12Device *GetDevice() const { return directXCommon_->GetDevice(); }

		/// <summary>
		/// グラフィックスコマンドリストを取得する
		/// </summary>
		/// <returns>コマンドリスト</returns>
		ID3D12GraphicsCommandList *GetCommandList() const { return directXCommon_->GetCommandList(); }

		/// <summary>
		/// テクスチャマネージャを取得する
		/// </summary>
		/// <returns>Textureクラス</returns>
		Texture *GetTextureManager() const { return textureManager_.get(); }

		/// <summary>
		/// オーディオマネージャを取得する
		/// </summary>
		/// <returns>AudioManagerクラス</returns>
		AudioManager *GetAudioManager() const { return audioManager_.get(); }

		/// <summary>
		/// キーボード入力クラスを取得する
		/// </summary>
		/// <returns>Keyboardクラス</returns>
		Keyboard *GetKeyboard() const { return keyboard_.get(); }

		/// <summary>
		/// マウス入力クラスを取得する
		/// </summary>
		/// <returns>Mouseクラス</returns>
		Mouse *GetMouse() const { return mouse_.get(); }

		/// <summary>
		/// ゲームパッド入力クラスを取得する
		/// </summary>
		/// <returns>GamePadクラス</returns>
		GamePad *GetGamePad() const { return gamePad_.get(); }

		/// <summary>
		/// パイプライン管理クラスを取得する
		/// </summary>
		/// <returns>PipelineManagerクラス</returns>
		PipelineManager *GetPipelineManager() const { return pipelineManager_.get(); }

		/// <summary>
		/// ライト管理クラスを取得する
		/// </summary>
		/// <returns>Lightingクラス</returns>
		Lighting *GetLight() const { return lighting_.get(); }

	public:
		KizunaEngine() = default;
		~KizunaEngine() = default;

		// コピー禁止
		KizunaEngine(const KizunaEngine &) = delete;
		KizunaEngine &operator=(const KizunaEngine &) = delete;

	private:
		/// デバッグ管理クラス
		std::unique_ptr<DebugManager> debugManager_;

		/// ログ管理クラス
		std::unique_ptr<Logger> logger_;

		/// ウィンドウ管理クラス
		std::unique_ptr<WinApp> winApp_;

		/// DirectX12共通管理クラス
		std::unique_ptr<DirectXCommon> directXCommon_;

		/// キーボード入力管理クラス
		std::unique_ptr<Keyboard> keyboard_;

		/// マウス入力管理クラス
		std::unique_ptr<Mouse> mouse_;

		/// ゲームパッド入力管理クラス
		std::unique_ptr<GamePad> gamePad_;

		/// パイプライン管理クラス
		std::unique_ptr<PipelineManager> pipelineManager_;

		/// ライト管理クラス
		std::unique_ptr<Lighting> lighting_;

		/// テクスチャ管理クラス
		std::unique_ptr<Texture> textureManager_;

		/// オーディオ管理クラス
		std::unique_ptr<AudioManager> audioManager_;
	};
}