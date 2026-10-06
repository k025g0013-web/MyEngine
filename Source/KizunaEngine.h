#pragma once

#include <cstdint>
#include <memory>
#include <string>

// === Core ===
#include "Core/WinApp.h"
#include "Core/Logger.h"
#include "Manager/DebugManager.h"

// === Graphics ===
#include "Graphics/DirectXCommon.h"
#include "Graphics/Pipeline/PipelineManager.h"
#include "Manager/TextureManager.h"

// === RenderCore ===
#include "RenderCore/Lighting.h"

// === Audio ===
#include "Manager/AudioManager.h"

// === Input ===
#include "Input/Keyboard.h"
#include "Input/Mouse.h"
#include "Input/GamePad.h"

#include "Manager/PrimitiveManager.h"
#include "Manager/ModelManager.h"

namespace Kizuna {

    /// <summary>
    /// KizunaEngine全体を管理するエンジンクラス
    /// </summary>
    class KizunaEngine {
    public:
        /// <summary>
        /// エンジンを初期化する
        /// </summary>
        void Initialize(
            const std::wstring &title,
            int32_t width,
            int32_t height
        );

        /// <summary>
        /// エンジンを終了する
        /// </summary>
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
        bool ProcessMessage();

        /// <summary>
        /// 入力デバイスの状態を更新する
        /// </summary>
        void UpdateInput();

        /// <summary>
        /// 描画パイプラインを設定する
        /// </summary>
        void SetPipeline(PipelineType type);

        PipelineConfig &GetPipelineConfig(
            PipelineType type
        );

        void RebuildPipeline(
            PipelineType type
        );

        WinApp *GetWinApp() const {
            return winApp_.get();
        }

        DirectXCommon *GetDirectXCommon() const {
            return directXCommon_.get();
        }

        ID3D12Device *GetDevice() const {
            return directXCommon_->GetDevice();
        }

        ID3D12GraphicsCommandList *GetCommandList() const {
            return directXCommon_->GetCommandList();
        }

        TextureManager *GetTextureManager() const {
            return textureManager_.get();
        }

        AudioManager *GetAudioManager() const {
            return audioManager_.get();
        }

        Keyboard *GetKeyboard() const {
            return keyboard_.get();
        }

        Mouse *GetMouse() const {
            return mouse_.get();
        }

        GamePad *GetGamePad() const {
            return gamePad_.get();
        }

        PipelineManager *GetPipelineManager() const {
            return pipelineManager_.get();
        }

        Lighting *GetLight() const {
            return lighting_.get();
        }

        PrimitiveManager *GetPrimitiveManager() const {
            return primitiveManager_.get();
        }

        ModelManager *GetModelManager() const {
            return modelManager_.get();
        }

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
        std::unique_ptr<TextureManager> textureManager_;

        /// オーディオ管理クラス
        std::unique_ptr<AudioManager> audioManager_;

        std::unique_ptr<PrimitiveManager> primitiveManager_;
        std::unique_ptr<ModelManager> modelManager_;
    };

}