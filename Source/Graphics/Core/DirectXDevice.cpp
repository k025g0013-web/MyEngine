#include "DirectXDevice.h"

#include "Core/Logger.h"
#include "Utils/ConvertString.h"
#include "Utils/AssertHelper.h"
#include <cassert>
#include <format>

namespace Kizuna {
    void DirectXDevice::Initialize(Logger *logger) {
        // Loggerを保持し、以降のDirectX初期化処理で発生した情報を出力できるようにする
        logger_ = logger;

        // DXGI Factoryを生成する
        // Adapter列挙やSwapChain生成など、DXGI関連機能を利用するために必要
        HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
        CheckHRESULT(hr, "CreateDXGIFactory");

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
            CheckHRESULT(hr, "IDXGIAdapter4::GetDesc3");


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

        // 対応するFeatureLevelを確認しながらDevice生成を試行する
        for (size_t i = 0; i < _countof(featureLevels); ++i) {

            HRESULT hr = D3D12CreateDevice(
                useAdapter_.Get(),
                featureLevels[i],
                IID_PPV_ARGS(&device_)
            );

            // Device生成に成功した場合は採用して終了する
            if (SUCCEEDED(hr)) {
                logger_->Log(
                    std::format(
                        "FeatureLevel : {}\n",
                        featureLevelStrings[i]
                    )
                );

                break;
            }
        }

        // すべてのFeatureLevelで失敗した場合は終了する
        CheckCondition(device_ != nullptr, "DirectX12 Deviceの生成に失敗しました。");

        logger_->Log("Complete create D3D12Device!!!\n");
    }


    // デバッグレイヤー生成
    void DirectXDevice::SetupDebugLayer() {
        // InfoQueueをComPtrで管理する
        Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue;

        // InfoQueueを取得できる場合のみデバッグ設定を行う
        if (SUCCEEDED(
            device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {

            // 重大なエラーが発生した場合はデバッグ実行を停止する
            infoQueue->SetBreakOnSeverity(
                D3D12_MESSAGE_SEVERITY_CORRUPTION,
                true
            );

            infoQueue->SetBreakOnSeverity(
                D3D12_MESSAGE_SEVERITY_ERROR,
                true
            );

            // 無視するメッセージを指定する
            D3D12_MESSAGE_ID denyIds[] = {
                D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
            };

            D3D12_MESSAGE_SEVERITY severities[] = {
                D3D12_MESSAGE_SEVERITY_INFO
            };

            D3D12_INFO_QUEUE_FILTER filter{};

            filter.DenyList.NumIDs = _countof(denyIds);
            filter.DenyList.pIDList = denyIds;

            filter.DenyList.NumSeverities = _countof(severities);
            filter.DenyList.pSeverityList = severities;

            // 指定したメッセージを除外する
            infoQueue->PushStorageFilter(&filter);
        }
    }
}