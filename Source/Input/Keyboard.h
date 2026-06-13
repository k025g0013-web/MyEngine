#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include "WinApp.h"

class Keyboard {
public:
	void Initialize(WinApp* winApp);
	void Update();
	void Finalize();

	// 離された状態
	bool FreeKey(uint8_t keyNum) const { return !(key_[keyNum] & 0x80); }

	// 押された状態
	bool PushKey(uint8_t keyNum) const { return key_[keyNum] & 0x80; }

	// 離した瞬間
	bool ReleaseKey(uint8_t keyNum) const {
		return !(key_[keyNum] & 0x80) && (preKey_[keyNum] & 0x80);}
	
	// 押した瞬間
	bool TriggerKey(uint8_t keyNum) const {
		return (key_[keyNum] & 0x80) && !(preKey_[keyNum] & 0x80);}

private:
	IDirectInput8 *directInput_ = nullptr;
	IDirectInputDevice8 *keyboard_ = nullptr;

	BYTE key_[256] = {};
	BYTE preKey_[256] = {};
};