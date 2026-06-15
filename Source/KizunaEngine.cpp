#include "KizunaEngine.h"
#include "External/ImGuiManager.h"

KizunaEngine::KizunaEngine() = default;
KizunaEngine::~KizunaEngine() = default;

KizunaEngine *KizunaEngine::GetInstance() {
    static KizunaEngine instance;
    return &instance;
}

void KizunaEngine::Initialize(const std::wstring &title, int32_t width, int32_t height) {
    // デバッグレイヤーとログの初期化
    debugManager_ = std::make_unique<DebugManager>();
    logger_ = std::make_unique<Logger>();
    logger_->Initialize();

    // ウィンドウ生成
    winApp_ = std::make_unique<WinApp>();
    winApp_->Initialize(title.c_str(), width, height);

#ifdef _DEBUG
    // デバッグレイヤーを有効化
    debugManager_->EnableDebugLayer();
#endif

    // DirectXCommon初期化
    directXCommon_ = std::make_unique<DirectXCommon>();
    directXCommon_->Initialize(winApp_.get(), logger_.get(), width, height);

    ID3D12Device *device = directXCommon_->GetDevice();
    HWND hwnd = winApp_->GetHWND();

    // 入力マネージャ初期化
    keyboard_ = std::make_unique<Keyboard>();
    keyboard_->Initialize(winApp_.get());

    mouse_ = std::make_unique<Mouse>();
    mouse_->Initialize(hwnd);
    winApp_->SetInputMouse(mouse_.get());

    // PSO初期化
    pipelineManager_ = std::make_unique<PipelineManager>();
    pipelineManager_->Initialize(device, logger_.get());
}

void KizunaEngine::Finalize() {
#ifdef USE_IMGUI
    ImGuiManager::GetInstance()->Finalize();
#endif

    if (winApp_) {
        winApp_->Finalize();
    }
}

bool KizunaEngine::ProcessMessage() {
    return winApp_->ProcessMessage();
}

void KizunaEngine::UpdateInput() {
    if (keyboard_) keyboard_->Update();
    if (mouse_) mouse_->Update();
}

void KizunaEngine::BeginFrame() {
    directXCommon_->BeginFrame();
}

void KizunaEngine::EndFrame() {
    directXCommon_->EndFrame();

    if (mouse_) mouse_->EndFrame();
}