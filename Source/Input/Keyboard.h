#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include "Core/WinApp.h"

namespace Kizuna {
	/// <summary>
	/// キーボード入力を管理するクラス
	/// </summary>
	/// <remarks>
	/// DirectInputを用いてキーボードの入力状態を取得し、
	/// 押下・離した瞬間・押し続けている状態などを判定する。
	/// 前フレームの入力状態も保持することでトリガー判定を実現している。
	/// </remarks>
	class Keyboard {
	public:

		/// <summary>
		/// キーボード入力システムを初期化する
		/// </summary>
		/// <param name="winApp">ウィンドウ情報</param>
		void Initialize(WinApp *winApp);

		/// <summary>
		/// キーボード入力状態を更新する
		/// </summary>
		/// <remarks>
		/// 現在の入力状態を取得し、前フレームの状態を保存する。
		/// 毎フレーム呼び出すことを前提としている。
		/// </remarks>
		void Update();

		/// <summary>
		/// DirectInputのリソースを解放する
		/// </summary>
		void Finalize();

		/// <summary>
		/// キーが離されているか取得する
		/// </summary>
		/// <param name="keyNum">DIKキーコード</param>
		/// <returns>離されていればtrue</returns>
		bool FreeKey(uint8_t keyNum) const { return !(key_[keyNum] & 0x80); }

		/// <summary>
		/// キーが押されているか取得する
		/// </summary>
		/// <param name="keyNum">DIKキーコード</param>
		/// <returns>押されていればtrue</returns>
		bool PushKey(uint8_t keyNum) const { return key_[keyNum] & 0x80; }

		/// <summary>
		/// キーが離された瞬間か取得する
		/// </summary>
		/// <param name="keyNum">DIKキーコード</param>
		/// <returns>離した瞬間ならtrue</returns>
		bool ReleaseKey(uint8_t keyNum) const {
			return !(key_[keyNum] & 0x80) && (preKey_[keyNum] & 0x80);
		}

		/// <summary>
		/// キーが押された瞬間か取得する
		/// </summary>
		/// <param name="keyNum">DIKキーコード</param>
		/// <returns>押した瞬間ならtrue</returns>
		bool TriggerKey(uint8_t keyNum) const {
			return (key_[keyNum] & 0x80) && !(preKey_[keyNum] & 0x80);
		}

	private:

		/// DirectInput本体
		IDirectInput8 *directInput_ = nullptr;

		/// キーボードデバイス
		IDirectInputDevice8 *keyboard_ = nullptr;

		/// 現在の入力状態
		BYTE key_[256] = {};

		/// 前フレームの入力状態
		BYTE preKey_[256] = {};
	};
}