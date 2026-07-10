#include "Input/Keyboard.h"
#include <cassert>

void Keyboard::Initialize(WinApp *winApp) {
	HRESULT result;
	// DirectInputオブジェクトを生成する
	result = DirectInput8Create(
		winApp->GetWC().hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void **)&directInput_, nullptr
	);

	// キーボードデバイスを取得する
	result = directInput_->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
	assert(SUCCEEDED(result));

	// 標準的なキーボード入力フォーマットを設定する
	result = keyboard_->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));

	// フォアグラウンドかつ非排他的に入力を取得する
	// DISCL_NOWINKEYによりWindowsキーの誤入力を抑制する
	result = keyboard_->SetCooperativeLevel(
		winApp->GetHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY
	);
	assert(SUCCEEDED(result));
}

void Keyboard::Finalize() {
	// キーボードデバイスを解放
	if (keyboard_) {
		keyboard_->Unacquire();   // 入力取得を終了
		keyboard_->Release();
		keyboard_ = nullptr;
	}

	// DirectInput本体を解放
	if (directInput_) {
		directInput_->Release();
		directInput_ = nullptr;
	}
}

void Keyboard::Update() {
	// トリガー・リリース判定のため前フレームの状態を保存する
	memcpy(preKey_, key_, sizeof(key_));

	// フォーカスを取得して入力受付状態にする
	keyboard_->Acquire();

	// 現在のキーボード入力状態を取得する
	keyboard_->GetDeviceState(sizeof(key_), key_);
}