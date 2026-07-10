#include "WinApp.h"
#include "Input/Mouse.h"

#ifdef USE_IMGUI
#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx12.h>
#include <imgui/imgui_impl_win32.h>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

// ウィンドウプロシージャ
LRESULT CALLBACK WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    // ImGuiが有効な場合は、先にImGui側で入力メッセージを処理する
#ifdef USE_IMGUI
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
        return true;
    }
#endif

    // ウィンドウ生成時にWinAppインスタンスをウィンドウへ関連付ける
    if (msg == WM_NCCREATE) {
        LPCREATESTRUCT createStruct = reinterpret_cast<LPCREATESTRUCT>(lparam);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(createStruct->lpCreateParams));
    }

    // 登録済みのWinAppインスタンスを取得する
    WinApp *winApp = reinterpret_cast<WinApp *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

    // メッセージの種類に応じた処理を行う
    switch (msg) {

        // マウスホイールの入力をMouseクラスへ通知する
    case WM_MOUSEWHEEL:
        if (winApp && winApp->inputMouse_) {
            int delta = GET_WHEEL_DELTA_WPARAM(wparam);
            winApp->inputMouse_->SetWheelDelta(delta);
        }
        return 0;

        // ウィンドウが閉じられたとき
    case WM_DESTROY:
        // アプリケーション終了メッセージを送信する
        PostQuitMessage(0);
        return 0;
    }

    // 上記以外はWindows標準のメッセージ処理へ渡す
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

// ウィンドウ生成
void WinApp::Initialize(LPCWSTR title, uint32_t width, uint32_t height) {
    // ウィンドウクラス情報を設定する
    wc_.lpfnWndProc = WinApp::WindowProc;           // ウィンドウプロシージャ
    wc_.lpszClassName = L"CG2WindowClass";          // ウィンドウクラス名
    wc_.hInstance = GetModuleHandle(nullptr);       // アプリケーションインスタンス
    wc_.hCursor = LoadCursor(nullptr, IDC_ARROW);   // 標準カーソル

    // ウィンドウクラスをWindowsへ登録する
    RegisterClass(&wc_);

    // クライアント領域のサイズを設定する
    const int32_t kClientWidth = width;
    const int32_t kClientHeight = height;

    // クライアントサイズを保持する矩形を作成する
    RECT wrc = { 0, 0, kClientWidth, kClientHeight };

    // タイトルバーや枠を含めたウィンドウサイズへ変換する
    AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

    // ウィンドウを生成する
    hwnd_ = CreateWindow(
        wc_.lpszClassName,
        title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        wrc.right - wrc.left,
        wrc.bottom - wrc.top,
        nullptr,
        nullptr,
        wc_.hInstance,
        this
    );

    // 作成したウィンドウを表示する
    ShowWindow(hwnd_, SW_SHOW);
}

// ウィンドウ終了
void WinApp::Finalize() {
    // ウィンドウを閉じる
    CloseWindow(hwnd_);
}

// Windowsメッセージ処理
bool WinApp::ProcessMessage() {
    MSG msg{};

    // メッセージキューに存在するメッセージを処理する
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        // 終了メッセージを受信したらアプリケーション終了
        if (msg.message == WM_QUIT) {
            return false;
        }
    }

    // 継続して実行する
    return true;
}