#pragma once

#pragma comment(lib, "xaudio2.lib")

#include <xaudio2.h>
#include <cstdint>

namespace Kizuna {
	/// <summary>
	/// RIFFチャンク共通ヘッダ
	/// </summary>
	/// <remarks>
	/// チャンクIDとデータサイズを保持する。
	/// WAVファイル解析時に利用される。
	/// </remarks>
	struct ChunkHeader {
		char id[4]{};
		int32_t size = 0;
	};

	/// <summary>
	/// RIFFヘッダ
	/// </summary>
	/// <remarks>
	/// WAVファイルの先頭に存在するヘッダ。
	/// ファイル種別を判定するために使用する。
	/// </remarks>
	struct RiffHeader {
		ChunkHeader chunk{};
		char type[4]{};
	};

	/// <summary>
	/// WAVフォーマット情報
	/// </summary>
	/// <remarks>
	/// サンプリング周波数やチャンネル数など、
	/// 波形データのフォーマット情報を保持する。
	/// </remarks>
	struct FormatChunk {
		ChunkHeader chunk{};
		WAVEFORMATEX fmt{};
	};
}