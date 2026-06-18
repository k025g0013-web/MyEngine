#include "Audio.h"
#include <cassert>
#include <cstring>
#include <algorithm>
#include <vector>
#include <filesystem>

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

void Audio::Initialize() {
	XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	xAudio2_->CreateMasteringVoice(&masterVoice_);

	MFStartup(MF_VERSION);
}

void Audio::Finalize() {
	// すべてのボイスを停止
	for (auto &pair : playVoices_) {
		for (auto *pSourceVoice : pair.second) {
			if (pSourceVoice) {
				pSourceVoice->Stop(0);
				pSourceVoice->DestroyVoice();
			}
		}
	}
	playVoices_.clear();

	if (masterVoice_) {
		masterVoice_->DestroyVoice();
		masterVoice_ = nullptr;
	}
	xAudio2_.Reset();

	MFShutdown();
}

AudioData Audio::LoadAudio(const std::string &filename) {
	std::filesystem::path path(filename);

	if (path.extension() == ".wav") {
		return LoadWav(filename.c_str());
	}

	if (path.extension() == ".mp3") {
		std::wstring wpath(
			filename.begin(),
			filename.end()
		);
		return LoadMp3(wpath.c_str());
	}

	assert(false && "対応していない音声形式");

	return {};
}

AudioData Audio::LoadWav(const char *filename) {
	// ファイルオープン
	//====================
	// ファイル入力ストリームのインスタンス
	std::ifstream file;
	// .wavファイルのバイナリモードで開く
	file.open(filename, std::ios_base::binary);
	// ファイルオープン失敗を検出する
	assert(file.is_open() && "音声ファイルの読み込みに失敗しました");

	// .waveデータ読み込み
	//====================
	// RIFFヘッダーの読み込み
	RiffHeader riff{};
	file.read((char *)&riff, sizeof(riff));
	// ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) { assert(0); }
	// タイプがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0) { assert(0); }

	// Formatチャンクの読み込み
	FormatChunk format = {};
	// チャンクヘッダ―の確認
	file.read((char *)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) { assert(0); }

	// チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char *)&format.fmt, format.chunk.size);

	// Dataチャンクの読み込み
	ChunkHeader data{};
	file.read((char *)&data, sizeof(data));
	// JUNKチャンクを検出した場合
	if (strncmp(data.id, "JUNK", 4) == 0) {
		// 読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		// 再読み込み
		file.read((char *)&data, sizeof(data));
	}

	// Dataチャンクのデータ部（波形データ）の読み込み
	char *pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	// Waveファイルを閉じる
	file.close();

	// 読み込んだ音声データをreturn
	//====================
	AudioData soundData = {};
	std::memset(&soundData.wfex, 0, sizeof(soundData.wfex));
	std::memcpy(&soundData.wfex, &format.fmt, sizeof(format.fmt));
	soundData.pBuffer =
		reinterpret_cast<BYTE *>(pBuffer);
	soundData.bufferSize = data.size;

	return soundData;
}

AudioData Audio::LoadMp3(const wchar_t *filename) {
	Microsoft::WRL::ComPtr<IMFSourceReader> reader = nullptr;

	HRESULT hr = MFCreateSourceReaderFromURL(
		filename, nullptr, &reader);
	assert(SUCCEEDED(hr));

	// PCMへ変換
	IMFMediaType *mediaType = nullptr;
	MFCreateMediaType(&mediaType);
	mediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	mediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);

	reader->SetCurrentMediaType(
		static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), nullptr, mediaType);
	mediaType->Release();

	// 実際のフォーマット取得
	IMFMediaType *currentType = nullptr;
	reader->GetCurrentMediaType(
		static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), &currentType);

	WAVEFORMATEX *waveFormat = nullptr;
	UINT32 waveFormatSize = 0;
	MFCreateWaveFormatExFromMFMediaType(
		currentType, &waveFormat, &waveFormatSize);
	currentType->Release();

	// 音声データの読み込み
	std::vector<BYTE> audioData;
	while (true) {
		IMFSample *sample = nullptr;
		DWORD flags = 0;

		hr = reader->ReadSample(
			static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM),
			0, nullptr, &flags, nullptr, &sample);

		if (flags & MF_SOURCE_READERF_ENDOFSTREAM) break;
		if (!sample) continue;

		IMFMediaBuffer *buffer = nullptr;
		sample->ConvertToContiguousBuffer(&buffer);

		BYTE *data = nullptr;
		DWORD maxLength = 0;
		DWORD currentLength = 0;

		buffer->Lock(&data, &maxLength, &currentLength);

		// データをバッファに追加
		audioData.insert(audioData.end(), data, data + currentLength);

		buffer->Unlock();
		buffer->Release();
		sample->Release();
	}

	// 読み込んだデータをヒープへコピー
	BYTE *pcmBuffer = new BYTE[audioData.size()];
	memcpy(pcmBuffer, audioData.data(), audioData.size());

	AudioData soundData{};
	std::memcpy(&soundData.wfex, waveFormat, waveFormatSize);

	soundData.pBuffer = pcmBuffer;
	soundData.bufferSize = static_cast<UINT32>(audioData.size());

	CoTaskMemFree(waveFormat);

	return soundData;
}

void Audio::UnloadAudio(AudioData *soundData) {
	StopAudio(*soundData);

	// バッファのメモリを解放
	delete[] soundData->pBuffer;

	soundData->pBuffer = nullptr;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

// 音データ再生 (多重再生対応)
void Audio::PlayAudio(const AudioData &soundData, int loopFlag, float volume) {
	// ループ再生かつ、すでに再生中の場合は、二重に再生されないように処理を抜ける
	if (loopFlag && IsPlayingAudio(soundData)) {
		return;
	}

	// 再生済みの音声を破棄
	ClearFinishedVoices();

	// 波形フォーマットを基にSourceVoiceの生成
	IXAudio2SourceVoice *pSourceVoice = nullptr;
	HRESULT result = xAudio2_->CreateSourceVoice(
		&pSourceVoice, reinterpret_cast<const WAVEFORMATEX *>(&soundData.wfex)
	);
	assert(SUCCEEDED(result));

	// 再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.pBuffer;
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	if (loopFlag) {
		buf.LoopCount = XAUDIO2_LOOP_INFINITE;
	}

	// 波形データの送信
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	assert(SUCCEEDED(result));

	// 音量の設定
	pSourceVoice->SetVolume(volume);

	result = pSourceVoice->Start(0);
	assert(SUCCEEDED(result));

	// 管理配列に登録
	playVoices_[soundData.pBuffer].push_back(pSourceVoice);
}

// 音データ一括停止
void Audio::StopAudio(const AudioData &soundData) {
	auto it = playVoices_.find(soundData.pBuffer);
	if (it != playVoices_.end()) {
		// 配列内のすべてのボイスをループで停止・破棄
		for (auto *pSourceVoice : it->second) {
			if (pSourceVoice) {
				pSourceVoice->Stop(0);
				pSourceVoice->FlushSourceBuffers();
				// メモリ解放
				pSourceVoice->DestroyVoice();
			}
		}
		// マップから削除
		playVoices_.erase(it);
	}
}

// 音データ一括一時停止
void Audio::PauseAudio(const AudioData &soundData) {
	auto it = playVoices_.find(soundData.pBuffer);
	if (it != playVoices_.end()) {
		// 配列内のすべてのボイスを一時停止
		for (auto *pSourceVoice : it->second) {
			if (pSourceVoice) { pSourceVoice->Stop(0); }
		}
	}
}

// 音データ一括再開
void Audio::ResumeAudio(const AudioData &soundData) {
	auto it = playVoices_.find(soundData.pBuffer);
	if (it != playVoices_.end()) {
		// 配列内のすべてのボイスを再開
		for (auto *pSourceVoice : it->second) {
			if (pSourceVoice) { pSourceVoice->Start(0); }
		}
	}
}

// 音データ一括音量設定
void Audio::SetAudioVolume(const AudioData &soundData, float volume) {
	auto it = playVoices_.find(soundData.pBuffer);
	if (it != playVoices_.end()) {
		// 配列内のすべてのボイスの音量を変更
		for (auto *pSourceVoice : it->second) {
			if (pSourceVoice) { pSourceVoice->SetVolume(volume); }
		}
	}
}

// 再生中か取得 (1つでも鳴っていれば再生中と判定)
bool Audio::IsPlayingAudio(const AudioData &soundData) const {
	auto it = playVoices_.find(soundData.pBuffer);
	if (it == playVoices_.end()) {
		return false;
	}

	for (auto *pSourceVoice : it->second) {
		if (pSourceVoice) {
			XAUDIO2_VOICE_STATE state;
			pSourceVoice->GetState(&state);
			if (state.BuffersQueued > 0) {
				return true;
			}
		}
	}
	return false;
}

// 再生済みの音声を破棄
void Audio::ClearFinishedVoices() {
	for (auto it = playVoices_.begin(); it != playVoices_.end();) {
		auto &voiceVector = it->second;

		voiceVector.erase(
			std::remove_if(voiceVector.begin(), voiceVector.end(), [](IXAudio2SourceVoice *pSourceVoice) {
				if (!pSourceVoice) return true;

				XAUDIO2_VOICE_STATE state;
				pSourceVoice->GetState(&state);
				if (state.BuffersQueued == 0) {
					pSourceVoice->Stop(0);
					pSourceVoice->DestroyVoice(); // XAudio2のボイス実体を安全に削除
					return true;
				}
				return false;
				}),
			voiceVector.end()
		);

		// この音データに対応するボイス配列が空になったら、mapの要素自体も消す
		if (voiceVector.empty()) {
			it = playVoices_.erase(it);
		} else {
			++it;
		}
	}
}