#include "DebugManager.h"
#include <cassert>
#include <objbase.h>

namespace Kizuna {
    // コンストラクタ
    DebugManager::DebugManager() {
        // COMライブラリをマルチスレッドモードで初期化する
        HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
        assert(SUCCEEDED(hr));

        // 初期化に成功した場合のみ終了時に解放を行う
        if (SUCCEEDED(hr)) {
            isComInitialized_ = true;
        }

        (void)hr;
    }

    // デストラクタ
    DebugManager::~DebugManager() {
        // 初期化済みのCOMライブラリを解放する
        if (isComInitialized_) {
            CoUninitialize();
        }
    }

    // DirectX12のデバッグ機能を有効化する
    void DebugManager::EnableDebugLayer() {
#ifdef _DEBUG
        // デバッグインターフェースを取得する
        Microsoft::WRL::ComPtr<ID3D12Debug1> debugController = nullptr;

        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {

            // DirectX12のデバッグレイヤーを有効化し、不正なAPIの使用を検出する
            debugController->EnableDebugLayer();

            // GPUベースの検証を有効化し、GPU実行時の不正なリソースアクセスを検出する
            debugController->SetEnableGPUBasedValidation(TRUE);
        }
#endif
    }
}