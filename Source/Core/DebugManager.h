#pragma once

#pragma comment(lib, "Dbghelp.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")

#include <wrl.h>
#include <d3d12.h>
#include <dxgidebug.h>
#include <dxgi1_6.h>

namespace Kizuna {
    /// <summary>
    /// DirectX12のデバッグ機能を管理するクラス
    /// </summary>
    /// <remarks>
    /// COMライブラリの初期化・終了処理を行うほか、
    /// デバッグレイヤーやGPUベースの検証機能を有効化する。
    /// デバッグビルドでは終了時にLiveObjectを出力し、
    /// リソースリークの検出を支援する。
    /// </remarks>
    class DebugManager {
    public:
        /// <summary>
        /// DebugManagerを生成する
        /// </summary>
        /// <remarks>
        /// COMライブラリを初期化する。
        /// </remarks>
        DebugManager();

        /// <summary>
        /// DebugManagerを破棄する
        /// </summary>
        /// <remarks>
        /// 初期化したCOMライブラリを終了する。
        /// </remarks>
        ~DebugManager();

        /// <summary>
        /// DirectX12のデバッグレイヤーを有効化する
        /// </summary>
        /// <remarks>
        /// デバッグビルド時のみデバッグレイヤーおよび
        /// GPUベースの検証機能を有効にする。
        /// </remarks>
        void EnableDebugLayer();

    private:
        /// COMライブラリを初期化済みかどうか
        bool isComInitialized_ = false;

#ifdef _DEBUG
        /// <summary>
        /// DirectXリソースリークを検出するクラス
        /// </summary>
        /// <remarks>
        /// デバッグビルド終了時にLiveObjectを出力し、
        /// 解放漏れのあるDirectXリソースを確認できる。
        /// </remarks>
        struct LeakChecker {
            ~LeakChecker() {
                Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
                if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
                    debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
                    debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
                    debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
                }
            }
        };

        /// デバッグ終了時のリークチェックを行うオブジェクト
        LeakChecker leakChecker_;
#endif
    };
}