#pragma once

#pragma comment(lib, "xaudio2.lib")

#include <xaudio2.h>
#include <vector>
#include <cstdint>

/// <summary>
/// 読み込んだ音声データ
/// </summary>
/// <remarks>
/// 波形フォーマットとPCMデータを保持する。
/// AudioLoaderで生成され、
/// Audioクラスで再生に使用される。
/// </remarks>
struct AudioData {
	
	/// 波形フォーマット
	WAVEFORMATEXTENSIBLE waveFormat{};

	/// PCMデータ
	std::vector<BYTE> buffer;
};