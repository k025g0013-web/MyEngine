#include "KizunaEngine.h"

#include "External/ImGuiManager.h"
#include "Graphics/Pipeline/RootSignature.h"
#include "Graphics/Pipeline/GraphicsPipeline.h"

namespace Kizuna {

    void KizunaEngine::Initialize(
        const std::wstring &title,
        int32_t width,
        int32_t height
    ) {
        //-------------------------------------------------------------------------
        // 最優先システムの構築
        //-------------------------------------------------------------------------

        debugManager_ = std::make_unique<DebugManager>();

        logger_ = std::make_unique<Logger>();
        logger_->Initialize();

        //-------------------------------------------------------------------------
        // アプリケーションウィンドウの生成
        //-------------------------------------------------------------------------

        winApp_ = std::make_unique<WinApp>();
        winApp_->Initialize(
            title.c_str(),
            width,
            height
        );

#ifdef _DEBUG
        // DirectX12のデバッグレイヤーを有効化
        debugManager_->EnableDebugLayer();
#endif

        //-------------------------------------------------------------------------
        // DirectX12 コアグラフィックス環境の構築
        //-------------------------------------------------------------------------

        directXCommon_ = std::make_unique<DirectXCommon>();

        directXCommon_->Initialize(
            winApp_.get(),
            logger_.get(),
            width,
            height
        );

        ID3D12Device *device =
            directXCommon_->GetDevice();

        HWND hwnd =
            winApp_->GetHWND();

        //-------------------------------------------------------------------------
        // 各種リソース・情報マネージャの初期化
        //-------------------------------------------------------------------------

        // テクスチャ
        textureManager_ = std::make_unique<TextureManager>();

        textureManager_->Initialize(
            device,
            directXCommon_->GetSRVHeap()
        );

        // オーディオ
        audioManager_ = std::make_unique<AudioManager>();
        audioManager_->Initialize();

        // 頂点生成オブジェクト
        primitiveManager_ = std::make_unique<PrimitiveManager>();
        primitiveManager_->Initialize(device, GetCommandList());

        // モデルオブジェクト
        modelManager_ = std::make_unique<ModelManager>();
        modelManager_->Initialize(device, GetCommandList(), textureManager_.get());

        //-------------------------------------------------------------------------
        // 入力デバイスの初期化
        //-------------------------------------------------------------------------

        // キーボード
        keyboard_ = std::make_unique<Keyboard>();
        keyboard_->Initialize(winApp_.get());

        // マウス
        mouse_ = std::make_unique<Mouse>();
        mouse_->Initialize(hwnd);

        winApp_->SetInputMouse(
            mouse_.get()
        );

        // GamePad
        gamePad_ = std::make_unique<GamePad>();
        gamePad_->Initialize(winApp_.get());

        //-------------------------------------------------------------------------
        // レンダリング環境・パイプラインの構築
        //-------------------------------------------------------------------------

        pipelineManager_ =
            std::make_unique<PipelineManager>();

        pipelineManager_->Initialize(
            device,
            logger_.get()
        );

        //-------------------------------------------------------------------------
        // グローバルライト
        //-------------------------------------------------------------------------

        lighting_ = std::make_unique<Lighting>();

        lighting_->Initialize(device);

        //-------------------------------------------------------------------------
        // ImGui
        //-------------------------------------------------------------------------

#ifdef USE_IMGUI

        ImGuiManager::GetInstance()->Initialize(
            hwnd,
            device,
            directXCommon_->GetRenderOutput()->GetBufferCount(),
            directXCommon_->GetRenderOutput()->GetRTVDesc().Format,
            directXCommon_->GetSRVHeap()->GetDescriptorHeap(),
            directXCommon_->GetSRVHeap()->GetCPUDescriptorHandle(0),
            directXCommon_->GetSRVHeap()->GetGPUDescriptorHandle(0)
        );

        ImGuiIO &io = ImGui::GetIO();

        io.ConfigFlags |=
            ImGuiConfigFlags_DockingEnable;

#endif
    }


    void KizunaEngine::Finalize() {

#ifdef USE_IMGUI

        ImGuiManager::GetInstance()->Finalize();

#endif

        //-------------------------------------------------------------------------
        // テクスチャ
        //-------------------------------------------------------------------------

        if (textureManager_) {
            textureManager_->Finalize();
        }

        //-------------------------------------------------------------------------
        // オーディオ
        //-------------------------------------------------------------------------

        if (audioManager_) {
            audioManager_->Finalize();
        }

        //-------------------------------------------------------------------------
        // ウィンドウ
        //-------------------------------------------------------------------------

        if (winApp_) {
            winApp_->Finalize();
        }
    }


    bool KizunaEngine::ProcessMessage() {

        return winApp_->ProcessMessage();
    }


    void KizunaEngine::BeginFrame() {

        directXCommon_->BeginFrame();
    }


    void KizunaEngine::EndFrame() {

#ifdef USE_IMGUI

        ImGuiManager::GetInstance()->Draw(
            directXCommon_->GetCommandList()
        );

#endif

        directXCommon_->EndFrame();

        if (mouse_) {
            mouse_->EndFrame();
        }
    }


    void KizunaEngine::UpdateInput() {

        if (keyboard_) {
            keyboard_->Update();
        }

        if (mouse_) {
            mouse_->Update();
        }

        if (gamePad_) {
            gamePad_->Update();
        }
    }


    void KizunaEngine::SetPipeline(
        PipelineType type
    ) {
        // PipelineManagerにパイプライン取得を任せる
        const PipelineSet *pipeline =
            pipelineManager_->GetPipeline(type);

        // コマンドリストを取得
        ID3D12GraphicsCommandList *commandList =
            directXCommon_->GetCommandList();

        //-------------------------------------------------------------------------
        // RootSignature
        //-------------------------------------------------------------------------

        commandList->SetGraphicsRootSignature(
            pipeline->rootSignature->GetRootSignature()
        );

        //-------------------------------------------------------------------------
        // Pipeline State
        //-------------------------------------------------------------------------

        commandList->SetPipelineState(
            pipeline->pipeline->GetPipelineState()
        );

        //-------------------------------------------------------------------------
        // Primitive Topology
        //-------------------------------------------------------------------------

        commandList->IASetPrimitiveTopology(
            D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST
        );
    }

    PipelineConfig &KizunaEngine::GetPipelineConfig(
        PipelineType type
    ) {
        return pipelineManager_->GetConfig(type);
    }


    void KizunaEngine::RebuildPipeline(
        PipelineType type
    ) {
        pipelineManager_->RebuildPipeline(type);
    }
}