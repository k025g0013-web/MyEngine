#pragma once
#include <windows.h>

#include <cstdint>

namespace Kizuna {
    class Mouse;

    /// <summary>
    /// アプリケーションウィンドウを管理するクラス
    /// </summary>
    class WinApp {
    public:
        // ウィンドウ設定
        static constexpr LPCWSTR kWindowClassName = L"CG2WindowClass";
        static constexpr uint32_t kClientWidth = 1280;
        static constexpr uint32_t kClientHeight = 720;

        /// <summary>
        /// ウィンドウを生成して初期化する
        /// </summary>
        void Initialize(
            LPCWSTR title,
            uint32_t width = kClientWidth,
            uint32_t height = kClientHeight
        );

        /// <summary>
        /// ウィンドウとウィンドウクラスを解放する
        /// </summary>
        void Finalize();

        /// <summary>
        /// Windowsメッセージを処理する
        /// </summary>
        bool ProcessMessage();

        HWND GetHWND() const { return hwnd_; }
        WNDCLASS GetWC() const { return wc_; }

        void SetInputMouse(Mouse *inputMouse) {
            inputMouse_ = inputMouse;
        }

    private:
        static LRESULT CALLBACK WindowProc(
            HWND hwnd,
            UINT msg,
            WPARAM wparam,
            LPARAM lparam
        );

        HWND hwnd_ = nullptr;
        WNDCLASS wc_{};
        Mouse *inputMouse_ = nullptr;
    };
}