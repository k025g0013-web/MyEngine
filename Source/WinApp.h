#pragma once
#include <windows.h>

#include <cstdint>

class Mouse;

class WinApp {
public:
    void Initialize(LPCWSTR title, uint32_t width, uint32_t height);
    void Finalize();
    bool ProcessMessage();

    // getter
    HWND GetHWND() const { return hwnd_; }
    WNDCLASS GetWC() const { return wc_; }

    void SetInputMouse(Mouse *inputMouse) { inputMouse_ = inputMouse; }

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

    HWND hwnd_ = nullptr;
    WNDCLASS wc_{};

    Mouse *inputMouse_ = nullptr;
};