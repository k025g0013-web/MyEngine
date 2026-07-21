#pragma once

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cstdint>
#include <array>

#include "Graphics/Core/DescriptorHeap.h"

namespace Kizuna {
    class DirectXDevice;
    class CommandContext;
    class WinApp;


    /// <summary>
    /// 描画出力に関するリソースを管理するクラス
    /// </summary>
    /// <remarks>
    /// SwapChain、BackBuffer、RenderTargetView、
    /// DepthStencilViewなど画面描画に必要なリソースを管理する。
    ///
    /// フレームごとの描画対象となるバックバッファや、
    /// 深度情報を保持するDepthStencilを生成・提供することで、
    /// DirectX12の描画処理を支える役割を持つ。
    /// </remarks>
    class RenderOutput {
    public:

        /// バックバッファ数
        static const uint32_t kBufferCount = 2;


        /// <summary>
        /// 描画出力環境を初期化する
        /// </summary>
        /// <remarks>
        /// SwapChainの生成、BackBuffer取得、RTV作成、
        /// DepthStencil生成など描画に必要なリソースを準備する。
        /// </remarks>
        /// <param name="device">DirectX12デバイス</param>
        /// <param name="commandContext">コマンド管理クラス</param>
        /// <param name="winApp">ウィンドウ管理クラス</param>
        /// <param name="rtvDescriptorHeap">RTV用DescriptorHeap</param>
        /// <param name="width">画面幅</param>
        /// <param name="height">画面高さ</param>
        void Initialize(
            DirectXDevice *device, CommandContext *commandContext, WinApp *winApp,
            DescriptorHeap *rtvDescriptorHeap, uint32_t width, uint32_t height
        );


        /// <summary>
        /// バックバッファを表示用へ切り替える
        /// </summary>
        /// <remarks>
        /// GPUで描画した結果をOS側へ表示するため、
        /// SwapChainのPresent処理を実行する。
        /// </remarks>
        void Present();


        // === getter ===

        /// <summary>
        /// 使用するバックバッファ数を取得する
        /// </summary>
        /// <returns>バックバッファ数</returns>
        uint32_t GetBufferCount() const { return kBufferCount; }


        // SwapChain

        /// <summary>
        /// SwapChainを取得する
        /// </summary>
        /// <returns>SwapChain</returns>
        IDXGISwapChain4 *GetSwapChain() const { return swapChain_.Get(); }


        /// <summary>
        /// 現在描画対象となるBackBuffer番号を取得する
        /// </summary>
        /// <returns>BackBufferインデックス</returns>
        UINT GetCurrentBackBufferIndex() const { return swapChain_->GetCurrentBackBufferIndex(); }


        /// <summary>
        /// SwapChain設定を取得する
        /// </summary>
        /// <returns>SwapChain設定</returns>
        const DXGI_SWAP_CHAIN_DESC1 &GetDesc() const { return swapChainDesc_; }


        /// <summary>
        /// 指定したBackBufferを取得する
        /// </summary>
        /// <param name="index">BackBuffer番号</param>
        /// <returns>BackBufferリソース</returns>
        ID3D12Resource *GetBackBuffer(uint32_t index) const { return backBuffers_[index].Get(); }


        /// <summary>
        /// 現在描画対象のBackBufferを取得する
        /// </summary>
        /// <returns>BackBufferリソース</returns>
        ID3D12Resource *GetCurrentBackBuffer() const { return backBuffers_[GetCurrentBackBufferIndex()].Get(); }



        // RenderTargetView

        /// <summary>
        /// 指定したRTVハンドルを取得する
        /// </summary>
        /// <param name="index">BackBuffer番号</param>
        /// <returns>RTV CPUハンドル</returns>
        D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandle(uint32_t index) const { return rtvHandles_[index]; }


        /// <summary>
        /// RTV設定を取得する
        /// </summary>
        /// <returns>RTV設定</returns>
        const D3D12_RENDER_TARGET_VIEW_DESC &GetRTVDesc() const { return rtvDesc_; }


        /// <summary>
        /// 現在描画対象のRTVハンドルを取得する
        /// </summary>
        /// <returns>RTV CPUハンドル</returns>
        D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRTVHandle() const { return rtvHandles_[GetCurrentBackBufferIndex()]; }



        // DepthStencil

        /// <summary>
        /// DepthStencilリソースを取得する
        /// </summary>
        /// <returns>DepthStencilリソース</returns>
        ID3D12Resource *GetResource() const { return depthStencilResource_.Get(); }


        /// <summary>
        /// DepthStencilViewのハンドルを取得する
        /// </summary>
        /// <returns>DSV CPUハンドル</returns>
        D3D12_CPU_DESCRIPTOR_HANDLE GetDSVHandle() const { return dsvHeap_.GetCPUDescriptorHandle(0); }


    private:

        /// <summary>
        /// DepthStencilリソースを生成する
        /// </summary>
        /// <remarks>
        /// 奥行き情報を保持するDepthStencilBufferと、
        /// それを参照するDSVを作成する。
        /// </remarks>
        /// <param name="device">DirectX12デバイス</param>
        /// <param name="width">画面幅</param>
        /// <param name="height">画面高さ</param>
        void CreateDepthStencil(ID3D12Device *device, uint32_t width, uint32_t height);


    private:

        // === SwapChain ===
        // 画面表示を管理するSwapChainと、
        // 描画対象となる複数のBackBufferを保持する。
        Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};
        std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, kBufferCount> backBuffers_;


        // === RenderTargetView ===
        // BackBufferを描画先として使用するためのRTV情報を保持する。
        std::array<D3D12_CPU_DESCRIPTOR_HANDLE, kBufferCount> rtvHandles_{};
        D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};


        // === DepthStencil ===
        // 奥行き判定に使用するDepthStencilリソースとDSVを保持する。
        Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_;
        DescriptorHeap dsvHeap_;
        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc_{};
    };
}