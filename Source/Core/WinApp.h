#pragma once
#include <windows.h>

#include <cstdint>

namespace Kizuna {
    class Mouse;

    /// <summary>
    /// アプリケーションウィンドウを管理するクラス
    /// </summary>
    /// <remarks>
    /// ウィンドウの生成・破棄やメッセージ処理を行う。
    /// また、Windowsのウィンドウプロシージャを管理し、
    /// 必要に応じて入力クラスへイベントを通知する。
    /// </remarks>
    class WinApp {
    public:
        /// <summary>
        /// ウィンドウを生成して初期化する
        /// </summary>
        /// <param name="title">ウィンドウタイトル</param>
        /// <param name="width">ウィンドウの横幅</param>
        /// <param name="height">ウィンドウの高さ</param>
        void Initialize(LPCWSTR title, uint32_t width, uint32_t height);

        /// <summary>
        /// ウィンドウを終了する
        /// </summary>
        /// <remarks>
        /// ウィンドウの破棄や登録したクラスの解放を行う。
        /// </remarks>
        void Finalize() const;

        /// <summary>
        /// Windowsメッセージを処理する
        /// </summary>
        /// <returns>
        /// アプリケーションを継続する場合はtrue、
        /// 終了要求を受けた場合はfalseを返す。
        /// </returns>
        bool ProcessMessage();

        /// <summary>
        /// ウィンドウハンドルを取得する
        /// </summary>
        /// <returns>ウィンドウハンドル</returns>
        HWND GetHWND() const { return hwnd_; }

        /// <summary>
        /// ウィンドウクラス情報を取得する
        /// </summary>
        /// <returns>ウィンドウクラス</returns>
        WNDCLASS GetWC() const { return wc_; }

        /// <summary>
        /// マウス入力クラスを登録する
        /// </summary>
        /// <param name="inputMouse">マウス入力クラス</param>
        void SetInputMouse(Mouse *inputMouse) { inputMouse_ = inputMouse; }

    private:
        /// <summary>
        /// Windowsメッセージを処理するコールバック関数
        /// </summary>
        /// <param name="hwnd">対象ウィンドウのハンドル</param>
        /// <param name="msg">受信したメッセージ</param>
        /// <param name="wparam">メッセージの追加情報</param>
        /// <param name="lparam">メッセージの追加情報</param>
        /// <returns>メッセージ処理結果</returns>
        static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

    private:
        /// ウィンドウハンドル
        HWND hwnd_ = nullptr;

        /// ウィンドウクラス情報
        WNDCLASS wc_{};

        /// 登録されたマウス入力クラス
        Mouse *inputMouse_ = nullptr;
    };
}