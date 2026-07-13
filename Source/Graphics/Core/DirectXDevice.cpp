#include "DirectXDevice.h"

#include "Core/Logger.h"
#include "Utils/ConvertString.h"

#include <cassert>
#include <format>

void DirectXDevice::Initialize(Logger *logger) {
    // Loggerを保持し、以降のDirectX初期化処理で発生した情報を出力できるようにする
    logger_ = logger;

    // DXGI Factoryを生成する
    // Adapter列挙やSwapChain生成など、DXGI関連機能を利用するために必要
    HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
    assert(SUCCEEDED(hr));
    (void)hr;

    // 使用するGPUを決定する
    // Device生成前に利用可能なAdapterを選択する必要がある
    SelectAdapter();

    // 選択したGPUからDirectX12 Deviceを生成する
    CreateDevice();

#ifdef _DEBUG
    // デバッグ環境ではGPU検証機能を有効化する
    // 描画エラーやリソース使用ミスを検出しやすくするため
    SetupDebugLayer();
#endif
}


// 使用するアダプタ(GPU)の選択
void DirectXDevice::SelectAdapter() {

    // 高性能GPU優先でAdapterを列挙する
    // 複数GPU環境(内蔵GPU・外部GPUなど)でも、
    // 可能な限り性能の高いGPUを利用するため
    for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(
        i,
        DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
        IID_PPV_ARGS(&useAdapter_)) != DXGI_ERROR_NOT_FOUND;
        ++i) {

        // Adapterの詳細情報を取得する
        DXGI_ADAPTER_DESC3 adapterDesc{};
        HRESULT hr = useAdapter_->GetDesc3(&adapterDesc);
        assert(SUCCEEDED(hr));
        (void)hr;


        // Software Adapter(WARPなど)は実際のGPUではないため除外する
        // Hardware GPUを利用することで本来の描画性能を使用できる
        if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {

            // 採用したGPU情報をログへ出力する
            logger_->Log(
                ConvertString(
                    std::format(L"UseAdapter : {}\n", adapterDesc.Description)
                )
            );

            break;
        }

        // Software Adapterだった場合は次のAdapterを検索する
        useAdapter_ = nullptr;
    }

    // 利用可能なGPUが存在しない場合は描画できないため終了する
    assert(useAdapter_ != nullptr);
}


// Deviceの生成
void DirectXDevice::CreateDevice() {

    // 対応可能なFeatureLevelを高い順に定義する
    // 新しいDirectX機能を優先的に利用し、
    // 対応していない環境では低いFeatureLevelへフォールバックする
    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_12_2,
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0
    };

    const char *featureLevelStrings[] = {
        "12.2",
        "12.1",
        "12.0"
    };


    // GPUが対応しているFeatureLevelを確認しながらDevice生成を試行する
    // PC環境によって対応するDirectX12機能が異なるため、
    // 最も高性能な設定から順番に確認する
    for (size_t i = 0; i < _countof(featureLevels); ++i) {

        HRESULT hr = D3D12CreateDevice(
            useAdapter_.Get(),
            featureLevels[i],
            IID_PPV_ARGS(&device_)
        );


        // Device生成に成功した場合は採用して終了する
        if (SUCCEEDED(hr)) {

            logger_->Log(
                std::format("FeatureLevel : {}\n", featureLevelStrings[i])
            );

            break;
        }
    }


    // Device生成失敗時はDirectX12を利用できないため終了する
    assert(device_ != nullptr);

    logger_->Log("Complete create D3D12Device!!!\n");
}


// デバッグレイヤー生成
void DirectXDevice::SetupDebugLayer() {

    ID3D12InfoQueue *infoQueue = nullptr;

    // InfoQueueを取得できる場合のみデバッグ設定を行う
    // GPUやAPI使用時の警告・エラーを検出するために利用する
    if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {


        // 致命的な問題発生時はデバッグ実行を停止する
        // 原因追跡を容易にするため、重要度の高いエラーをブレーク対象にする
        infoQueue->SetBreakOnSeverity(
            D3D12_MESSAGE_SEVERITY_CORRUPTION,
            true
        );

        infoQueue->SetBreakOnSeverity(
            D3D12_MESSAGE_SEVERITY_ERROR,
            true
        );


        // 無視する警告メッセージを指定する
        // DirectX12では環境依存で発生する既知の警告が存在するため、
        // 本当に必要な警告を確認しやすくするため抑制する
        D3D12_MESSAGE_ID denyIds[] = {
            D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
        };


        // 抑制対象となるメッセージレベル
        D3D12_MESSAGE_SEVERITY severities[] = {
            D3D12_MESSAGE_SEVERITY_INFO
        };


        D3D12_INFO_QUEUE_FILTER filter{};

        filter.DenyList.NumIDs = _countof(denyIds);
        filter.DenyList.pIDList = denyIds;

        filter.DenyList.NumSeverities = _countof(severities);
        filter.DenyList.pSeverityList = severities;


        // 指定したメッセージをログ表示対象から除外する
        infoQueue->PushStorageFilter(&filter);


        // QueryInterfaceで取得したインターフェースを解放する
        infoQueue->Release();
    }
}