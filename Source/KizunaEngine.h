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
#include "Graphics/Resource/Texture.h"

// === Audio ===
#include "Audio/Audio.h"

// === Input ===
#include "Input/Keyboard.h"
#include "Input/Mouse.h"
#include "Graphics/Pipeline/PipelineManager.h"

class KizunaEngine {
public:
	// シングルトンパターンによるアクセス
	static KizunaEngine *GetInstance();

	// ライフサイクル関数
	void Initialize(const std::wstring &title, int32_t width, int32_t height);
	void Finalize();

	// フレーム制御
	void BeginFrame();
	void EndFrame();

	// メッセージ処理
	bool ProcessMessage();

	// 入力デバイス更新
	void UpdateInput();

	// 外部から各システムを利用するためのゲッター群
	WinApp *GetWinApp() const { return winApp_.get(); }
	DirectXCommon *GetDirectXCommon() const { return directXCommon_.get(); }
	ID3D12Device *GetDevice() const { return directXCommon_->GetDevice(); }
	ID3D12GraphicsCommandList *GetCommandList() const { return directXCommon_->GetCommandList(); }

	Texture *GetTextureManager() const { return textureManager_.get(); }
	Audio *GetAudioManager() const { return audioManager_.get(); }

	Keyboard *GetKeyboard() const { return keyboard_.get(); }
	Mouse *GetMouse() const { return mouse_.get(); }
	PipelineManager *GetPipelineManager() const { return pipelineManager_.get(); }

private:
	KizunaEngine();
	~KizunaEngine();
	KizunaEngine(const KizunaEngine &) = delete;
	KizunaEngine &operator=(const KizunaEngine &) = delete;

private:
	std::unique_ptr<DebugManager> debugManager_;
	std::unique_ptr<Logger> logger_;
	std::unique_ptr<WinApp> winApp_;
	std::unique_ptr<DirectXCommon> directXCommon_;

	std::unique_ptr<Keyboard> keyboard_;
	std::unique_ptr<Mouse> mouse_;
	std::unique_ptr<PipelineManager> pipelineManager_;

	std::unique_ptr<Texture> textureManager_;
	std::unique_ptr<Audio> audioManager_;
};