#include "KizunaEngine.h"
#include "External/ImGuiManager.h"
#include "Graphics/Pipeline/RootSignature.h"
#include "Graphics/Pipeline/GraphicsPipeline.h"

void KizunaEngine::Initialize(const std::wstring &title, int32_t width, int32_t height) {
    //-------------------------------------------------------------------------
    // 最優先システムの構築
    //-------------------------------------------------------------------------
    // デバッグ・ログを真っ先に起動し以降の初期化エラーを確実にキャッチ・ログ出力できるようにする
    debugManager_ = std::make_unique<DebugManager>();   // デバッグマネージャ
    logger_ = std::make_unique<Logger>();   // ログ
    logger_->Initialize();

    //-------------------------------------------------------------------------
    // アプリケーションウィンドウの生成
    //-------------------------------------------------------------------------
    // DirectX12の初期化にHWNDが必要なためここで生成しておく
    winApp_ = std::make_unique<WinApp>();
    winApp_->Initialize(title.c_str(), width, height);

#ifdef _DEBUG
    // DirectX12のAPI不正呼び出しを即座に検出する
    debugManager_->EnableDebugLayer();
#endif

    //-------------------------------------------------------------------------
    // DirectX12 コアグラフィックス環境の構築
    //-------------------------------------------------------------------------
    // 以降のすべてのグラフィックスサブシステムがデバイスを必要とする
    directXCommon_ = std::make_unique<DirectXCommon>();
    directXCommon_->Initialize(winApp_.get(), logger_.get(), width, height);

    // 各マネージャの初期化用に参照を抽出
    ID3D12Device *device = directXCommon_->GetDevice();
    HWND hwnd = winApp_->GetHWND();

    //-------------------------------------------------------------------------
    // 各種リソース・情報マネージャの初期化
    //-------------------------------------------------------------------------
    // テクスチャ（SRVヒープを共有するため共通ヒープを渡す）
    textureManager_ = std::make_unique<Texture>();
    textureManager_->Initialize(device, directXCommon_->GetSRVHeap());

    // オーディオ
    audioManager_ = std::make_unique<AudioManager>();
    audioManager_->Initialize();

    //-------------------------------------------------------------------------
    // 入力デバイスの初期化と相互接続
    //-------------------------------------------------------------------------
    // キーボード初期化
    keyboard_ = std::make_unique<Keyboard>();
    keyboard_->Initialize(winApp_.get());

    // マウス初期化
    mouse_ = std::make_unique<Mouse>();
    mouse_->Initialize(hwnd);
    // ホイール回転を直接インスタンスに伝達できるようにする
    winApp_->SetInputMouse(mouse_.get());
    
    // GamePad初期化
    gamePad_ = std::make_unique<GamePad>();
    gamePad_->Initialize(winApp_.get());

    //-------------------------------------------------------------------------
    // レンダリング環境・パイプラインの構築
    //-------------------------------------------------------------------------
    // パイプライン状態（PSO/RootSignature）の全バリエーションを生成・コンパイル
    pipelineManager_ = std::make_unique<PipelineManager>();
    pipelineManager_->Initialize(device, logger_.get());

    // グローバルライトの定数バッファ生成
    lighting_ = std::make_unique<Lighting>();
    lighting_->Initialize(device);

    //-------------------------------------------------------------------------
    // 開発用デバッグUI（ImGui）の統合
    //-------------------------------------------------------------------------
#ifdef USE_IMGUI
    // DirectX12環境でのImGui描画に必要な情報をフォワードして初期化する
    ImGuiManager::GetInstance()->Initialize(
        hwnd,
        device,
        directXCommon_->GetRenderOutput()->GetBufferCount(),
        directXCommon_->GetRenderOutput()->GetRTVDesc().Format,
        directXCommon_->GetSRVHeap()->GetDescriptorHeap(),
        directXCommon_->GetSRVHeap()->GetCPUDescriptorHandle(0),
        directXCommon_->GetSRVHeap()->GetGPUDescriptorHandle(0)
    );

    // ドッキング機能の有効化
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
#endif
}

void KizunaEngine::Finalize() {
#ifdef USE_IMGUI
    // ImGuiが保持するGPUリソースを解放する
    ImGuiManager::GetInstance()->Finalize();
#endif

    // テクスチャマネージャが保持するGPUリソースを解放
    textureManager_->Finalize();

    // オーディオデバイスおよび読み込み済みサウンドを解放
    if (audioManager_) {
        audioManager_->Finalize();
    }

    // ウィンドウを閉じる前に各種DirectXリソースが安全に解放されている必要があるため、winAppの解放を最後に行う
    if (winApp_) {
        winApp_->Finalize();
    }
}

bool KizunaEngine::ProcessMessage() {
    // Windowsメッセージを処理し、終了要求の有無を返す
    return winApp_->ProcessMessage();
}

void KizunaEngine::BeginFrame() {
    // レンダリングターゲットのクリアや描画開始準備を行う
    directXCommon_->BeginFrame();
}

void KizunaEngine::EndFrame() {
#ifdef USE_IMGUI
    // 画面切り替えの直前にImGuiの描画コマンドを確定させる
    ImGuiManager::GetInstance()->Draw(directXCommon_->GetCommandList());
#endif

    directXCommon_->EndFrame();

    // 1フレーム中のマウス移動量をリセットし、次フレームの正確な移動量を計測する
    if (mouse_) mouse_->EndFrame();
}

void KizunaEngine::UpdateInput() {
    // 各入力デバイスを最新状態へ更新する
    if (keyboard_) keyboard_->Update();
    if (mouse_) mouse_->Update();
    if (gamePad_) gamePad_->Update();
}

void KizunaEngine::SetPipeline(PipelineType type) {
    // 指定された用途に対応するパイプライン構成を取得する
    auto *pipelineData = pipelineManager_->GetPipeline(type);
    auto *commandList = directXCommon_->GetCommandList();

    // DirectX12のパイプラインに対して、RootSignatureをバインド
    commandList->SetGraphicsRootSignature(pipelineData->rootSignature->GetRootSignature());

    // 描画時のBlend、Depth、Rasterizer、Shader等の状態を一括適用
    commandList->SetPipelineState(pipelineData->pipeline->GetPipelineState());

    // 頂点データをどのように解釈してプリミティブを組み立てるかを指定（標準的な三角形リスト）
    commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}