#include "DirectXDevice.h"

#include "Logger.h"
#include "Utils/ConvertString.h"

#include <cassert>
#include <format>

void DirectXDevice::Initialize(Logger* logger) {
    logger_ = logger;

    HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
    assert(SUCCEEDED(hr));
    (void)hr;

    // 使用するアダプタ(GPU)の選択
    SelectAdapter();

    // Deviceの生成
    CreateDevice();

#ifdef _DEBUG
    // デバッグレイヤー生成
    SetupDebugLayer();
#endif
}

// 使用するアダプタ(GPU)の選択
void DirectXDevice::SelectAdapter() {
    for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i,
        DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter_)) !=
        DXGI_ERROR_NOT_FOUND; ++i) {

        // アダプタの情報を取得する
        DXGI_ADAPTER_DESC3 adapterDesc{};
        HRESULT hr = useAdapter_->GetDesc3(&adapterDesc);
        assert(SUCCEEDED(hr));
        (void)hr;

        // ソフトウェアアダプタでなければ採用！
        if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
            // 採用したアダプタの情報をログに出力。wstringの方になるので注意
            logger_->Log(ConvertString(std::format(L"UseAdapter : {}\n", adapterDesc.Description)));
            break;
        }
        useAdapter_ = nullptr; 
        // ソフトウェアアダプタの場合は見なかったことにする
    }
    // 適切なアダプタが見つからなかったので起動できない
    assert(useAdapter_ != nullptr);
}

// Deviceの生成
void DirectXDevice::CreateDevice() {
    // 機能レベルとログ出力用の文字列
    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0
    };
    const char* featureLevelStrings[] = { "12.2", "12.1", "12.0" };
    
    // 高い順に生成出来るか試していく
    for (size_t i = 0; i < _countof(featureLevels); ++i) {
        // 採用したアダプターでデバイスを生成
        HRESULT hr = D3D12CreateDevice(useAdapter_.Get(), featureLevels[i], IID_PPV_ARGS(&device_));
    
        // 指定した機能レベルでデバイスが生成できたかを確認
        if (SUCCEEDED(hr)) {
            // 生成できたのでログ出力を行ってループを抜ける
            logger_->Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
            break;
        }
    }
    
    // デバイスの生成がうまくいかなかったので起動できない
    assert(device_ != nullptr);
    logger_->Log("Complete create D3D12Device!!!\n");   // ログ出力
}

// デバッグレイヤー生成
void DirectXDevice::SetupDebugLayer() {
    ID3D12InfoQueue* infoQueue = nullptr;
    if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
        // 致命的なエラー時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        // エラー時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);

        // 抑制するメッセージのID
        D3D12_MESSAGE_ID denyIds[] = {
            // Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
            D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
        };

        // 抑制するレベル
        D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
        D3D12_INFO_QUEUE_FILTER filter{};
        filter.DenyList.NumIDs = _countof(denyIds);
        filter.DenyList.pIDList = denyIds;
        filter.DenyList.NumSeverities = _countof(severities);
        filter.DenyList.pSeverityList = severities;
        
        // 指定したメッセージの表示を抑制する
        infoQueue->PushStorageFilter(&filter);

        // 解放
        infoQueue->Release();
    }
}