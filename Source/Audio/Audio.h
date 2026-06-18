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
struct AudioData {
	WAVEFORMATEXTENSIBLE wfex;	// 波形フォーマット
	BYTE *pBuffer;				// バッファの先頭アドレス
	UINT32 bufferSize;			// バッファのサイズ
};

class Audio {
public:
	void Initialize();
	void Finalize();

	// 音声データの読み込み
	AudioData LoadAudio(const std::string &filename);

	AudioData LoadWav(const char *filename);	// wavデータ
	AudioData LoadMp3(const wchar_t *filename);	// mp3データ

	// 音声データの解放
	void UnloadAudio(AudioData *soundData);

	// 音声データ制御
	void PlayAudio(const AudioData &soundData, int loopFlag = false, float volume = 1.0f);	// 再生
	void StopAudio(const AudioData &soundData);												// 停止
	void PauseAudio(const AudioData &soundData);											// 一時停止
	void ResumeAudio(const AudioData &soundData);											// 再開
	void SetAudioVolume(const AudioData &soundData, float volume);							// 音量設定
	bool IsPlayingAudio(const AudioData &soundData) const;									// 再生中か取得

private:
	// 再生済みの音声を破棄
	void ClearFinishedVoices();

private:
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
	IXAudio2MasteringVoice *masterVoice_ = nullptr;

	// 音声データ管理
	std::map<BYTE*, std::vector<IXAudio2SourceVoice*>> playVoices_;
};