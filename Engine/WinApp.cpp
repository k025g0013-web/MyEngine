#include "WinApp.h"
#include "InputMouse.h"

#ifdef USE_IMGUI
#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx12.h>
#include <imgui/imgui_impl_win32.h>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

// ウィンドウプロシ―ジャ
LRESULT CALLBACK WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    // マウス操作を可能とさせる
#ifdef USE_IMGUI
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
        return true;
    }
#endif

    // ウィンドウ作成時に、WinAppのインスタンス（this）をウィンドウのデータ領域に保存する
    if (msg == WM_NCCREATE) {
        LPCREATESTRUCT createStruct = reinterpret_cast<LPCREATESTRUCT>(lparam);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(createStruct->lpCreateParams));
    }

    // 保存しておいた WinApp のインスタンスを取得
    WinApp *winApp = reinterpret_cast<WinApp *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

    // メッセージに応じてゲーム固有の処理を行う
    switch (msg) {
    case WM_MOUSEWHEEL:
        if (winApp && winApp->inputMouse_) {
            int delta = GET_WHEEL_DELTA_WPARAM(wparam);
            winApp->inputMouse_->SetWheelDelta(delta);
        }
        return 0;

        // ウィンドウが破壊された
    case WM_DESTROY:
        // OSに対して、アプリの終了を伝える
        PostQuitMessage(0);
        return 0;
    }

    // 標準のメッセージ処理を行う
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

// ウィンドウ生成
void WinApp::Initialize(LPCWSTR title, uint32_t width, uint32_t height) {
    // ウィンドウクラスを定義する
    wc_.lpfnWndProc = WinApp::WindowProc;           // ウィンドウプロシ―ジャ
    wc_.lpszClassName = L"CG2WindowClass";          // ウィンドウクラス名
    wc_.hInstance = GetModuleHandle(nullptr);       // インスタントハンドル
    wc_.hCursor = LoadCursor(nullptr, IDC_ARROW);   // カーソル

    // ウィンドウクラスを登録する
    RegisterClass(&wc_);

    // クライアント領域のサイズ
    const int32_t kClientWidth = width;
    const int32_t kClientHeight = height;

    // ウィンドウサイズを表す構造体にクライアント領域を入れる。
    RECT wrc = {0, 0, kClientWidth, kClientHeight};

    // クライアント領域を基に実際のサイズにwrcを変更してもらう
    AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

    // ウィンドウの生成
    hwnd_ = CreateWindow(
        wc_.lpszClassName,		// 利用するクラス名
        title,					// タイトルバーの文字
        WS_OVERLAPPEDWINDOW,	// よく見るウィンドウスタイル
        CW_USEDEFAULT,			// 表示X座標(Windowsに任せる)
        CW_USEDEFAULT,			// 表示Y座標(WindowsOSに任せる)
        wrc.right - wrc.left,	// ウィンドウX幅
        wrc.bottom - wrc.top,	// ウィンドウY幅
        nullptr,				// 親ウィンドウハンドル
        nullptr,				// メニューハンドル
        wc_.hInstance,			// インスタントハンドル
        this					// オプション
    );

    ShowWindow(hwnd_, SW_SHOW);
}

void WinApp::Finalize() {
    CloseWindow(hwnd_);
}

bool WinApp::ProcessMessage() {
    MSG msg{};
    // Windowにメッセージが来てたら最優先で処理させる
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (msg.message == WM_QUIT) {
            return false;
        }
    }

    return true;
}