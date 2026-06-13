#pragma once
#include <wrl.h>

#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")

#include <fstream>
#include <cstdint>
#include <vector>

// チャンクヘッダ
struct ChunkHeader {
	char id[4];
	int32_t size;
};

// RIFFヘッダチャンク
struct RiffHeader {
	ChunkHeader chunk;
	char type[4];
};

// FMT
struct FormatChunk {
	ChunkHeader chunk;
	WAVEFORMATEX fmt;
};

// 音声データ
struct SoundData {
	WAVEFORMATEX wfex;			// 波形フォーマット
	BYTE *pBuffer;				// バッファの先頭アドレス
	unsigned int bufferSize;	// バッファのサイズ
};

class Sound {
public:
	void Initialize();
	void Finalize();

	// 音声データの読み込み
	SoundData SoundLoadWave(const char* filename);
	// 音声データの解放
	void SoundUnload(SoundData* soundData);

	// サウンドの再生
	void SoundPlayWave(const SoundData &soundData);

private:
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice *masterVoice = nullptr;
};