#pragma once

namespace Kizuna {
	/// <summary>
	/// 壁越し描画を行う際のスタイル
	/// </summary>
	/// <remarks>
	/// 壁越し描画時に使用する描画表現を指定する。
	/// </remarks>
	enum class Style {
		/// ソリッドパターン表示
		Solid,

		/// ドットパターン表示
		Dot,

		/// ストライプパターン表示
		Stripe,
	};
}