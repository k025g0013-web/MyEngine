#pragma once

#pragma comment(lib, "xaudio2.lib")

#include <wrl.h>
#include <xaudio2.h>
#include <vector>
#include <map>
#include <cstdint>
#include <fstream>

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
	WAVEFORMATEXTENSIBLE wfex;	// 波形フォーマット
	BYTE *pBuffer;				// バッファの先頭アドレス
	UINT32 bufferSize;			// バッファのサイズ
};

class Audio {
public:
	void Initialize();
	void Finalize();

	// 音声データの読み込み
	SoundData LoadAudio(const std::string &filename);

	SoundData LoadWav(const char *filename);	// wavデータ
	SoundData LoadMp3(const wchar_t *filename);	// mp3データ

	// 音声データの解放
	void UnloadAudio(SoundData *soundData);

	// 音声データ制御
	void PlayAudio(const SoundData &soundData, int loopFlag = false, float volume = 1.0f);	// 再生
	void StopAudio(const SoundData &soundData);												// 停止
	void PauseAudio(const SoundData &soundData);											// 一時停止
	void ResumeAudio(const SoundData &soundData);											// 再開
	void SetAudioVolume(const SoundData &soundData, float volume);							// 音量設定
	bool IsPlayingAudio(const SoundData &soundData) const;									// 再生中か取得

private:
	// 再生済みの音声を破棄
	void ClearFinishedVoices();

private:
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
	IXAudio2MasteringVoice *masterVoice_ = nullptr;

	// 音声データ管理
	std::map<BYTE*, std::vector<IXAudio2SourceVoice*>> playVoices_;
};