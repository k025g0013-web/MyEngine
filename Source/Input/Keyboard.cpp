#include "Input/Keyboard.h"
#include <cassert>

void Keyboard::Initialize(WinApp *winApp) {
	HRESULT result;
	// DirectInputの初期化
	result = DirectInput8Create(
		winApp->GetWC().hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void **)&directInput_, nullptr
	);

	// キーボードデバイスの生成
	result = directInput_->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
	assert(SUCCEEDED(result));

	// 入力データ形式のセット
	result = keyboard_->SetDataFormat(&c_dfDIKeyboard);	// 標準形式
	assert(SUCCEEDED(result));

	// 排他制御レベルのセット
	result = keyboard_->SetCooperativeLevel(
		winApp->GetHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY
	);
	assert(SUCCEEDED(result));
}

void Keyboard::Finalize() {
	if (keyboard_) {
		keyboard_->Unacquire();
		keyboard_->Release();
		keyboard_ = nullptr;
	}

	if (directInput_) {
		directInput_->Release();
		directInput_ = nullptr;
	}
}

void Keyboard::Update() {
	memcpy(preKey_, key_, sizeof(key_));

	// キーボード情報の取得開始
	keyboard_->Acquire();

	// 全キーの入力状態を取得する
	keyboard_->GetDeviceState(sizeof(key_), key_);
}