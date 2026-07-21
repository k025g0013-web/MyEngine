#pragma once
#include <cstdlib>

namespace Kizuna {
	/// <summary>
	/// 指定した範囲の乱数(float)を生成する
	/// </summary>
	/// <param name="min">
	/// 最小値
	/// </param>
	/// <param name="max">
	/// 最大値
	/// </param>
	/// <returns>
	/// min～maxの範囲の浮動小数点乱数
	/// </returns>
	/// <remarks>
	/// エフェクトやオブジェクト配置など、
	/// ランダム性が必要な場面で利用する。
	/// </remarks>
	inline float RandHelperFloat(float min, float max) {
		return min + (max - min) * (static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
	}

	/// <summary>
	/// 指定した範囲の整数乱数を生成する
	/// </summary>
	/// <param name="min">
	/// 最小値
	/// </param>
	/// <param name="max">
	/// 最大値
	/// </param>
	/// <returns>
	/// min～maxの範囲の整数乱数
	/// </returns>
	/// <remarks>
	/// 最大値が最小値以下の場合は異常な範囲と判断し、
	/// 最小値をそのまま返す。
	/// </remarks>
	inline int RandHelperInt(int min, int max) {
		if (max <= min) return min;

		return min + (rand() % (max - min + 1));
	}
}