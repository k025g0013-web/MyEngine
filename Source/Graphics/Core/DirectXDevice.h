#pragma once
#include <wrl.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#include <d3d12.h>
#include <dxgi1_6.h>

namespace Kizuna {
    class Logger;

    /// <summary>
    /// DirectX12デバイス環境を管理するクラス
    /// </summary>
    /// <remarks>
    /// DirectX12の描画処理に必要なDevice、DXGI Factory、
    /// 使用するGPUアダプタの生成および管理を行う。
    /// DirectX12初期化処理の基盤となるクラス。
    /// </remarks>
    class DirectXDevice {
    public:

        /// <summary>
        /// DirectX12デバイスを初期化する
        /// </summary>
        /// <remarks>
        /// デバッグレイヤー設定、DXGI Factory生成、
        /// 使用するGPUアダプタの選択、DirectX Device生成を行う。
        /// </remarks>
        /// <param name="logger">ログ出力管理クラス</param>
        void Initialize(Logger *logger);


        /// <summary>
        /// DirectX12デバイスを取得する
        /// </summary>
        /// <returns>DirectX12デバイス</returns>
        ID3D12Device *GetDevice() const { return device_.Get(); }


        /// <summary>
        /// DXGI Factoryを取得する
        /// </summary>
        /// <returns>DXGI Factory</returns>
        IDXGIFactory7 *GetDXGIFactory() const { return dxgiFactory_.Get(); }


        /// <summary>
        /// 使用中のGPUアダプタを取得する
        /// </summary>
        /// <returns>GPUアダプタ</returns>
        IDXGIAdapter4 *GetAdapter() const { return useAdapter_.Get(); }


    private:

        /// <summary>
        /// 使用するGPUアダプタを選択する
        /// </summary>
        /// <remarks>
        /// 利用可能なGPUを列挙し、
        /// DirectX12に対応したアダプタを決定する。
        /// </remarks>
        void SelectAdapter();


        /// <summary>
        /// DirectX12 Deviceを生成する
        /// </summary>
        /// <remarks>
        /// 選択したGPUアダプタを使用して、
        /// GPU操作を行うためのDirectX12 Deviceを作成する。
        /// </remarks>
        void CreateDevice();


        /// <summary>
        /// DirectX12デバッグレイヤーを設定する
        /// </summary>
        /// <remarks>
        /// デバッグビルド時にDirectX12の警告やエラー検出を有効化し、
        /// 開発中の問題発見を補助する。
        /// </remarks>
        void SetupDebugLayer();


    private:

        /// ログ出力管理クラス
        Logger *logger_ = nullptr;


        /// DirectX12デバイス
        Microsoft::WRL::ComPtr<ID3D12Device> device_;


        /// DXGI Factory管理
        Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;


        /// 使用するGPUアダプタ
        Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter_;
    };
}