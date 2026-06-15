#include "Audio.h"
#include <cassert>
#include <cstring>
#include <algorithm>

void Audio::Initialize() {
	XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	xAudio2_->CreateMasteringVoice(&masterVoice_);
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
}

SoundData Audio::LoadAudio(const char *filename) {
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
	SoundData soundData = {};
	soundData.wfex = format.fmt;
	soundData.pBuffer =
		reinterpret_cast<BYTE *>(pBuffer);
	soundData.bufferSize = data.size;

	return soundData;
}

void Audio::UnloadAudio(SoundData *soundData) {
	StopAudio(*soundData);

	// バッファのメモリを解放
	delete[] soundData->pBuffer;

	soundData->pBuffer = nullptr;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

// 音データ再生 (多重再生対応)
void Audio::PlayAudio(const SoundData &soundData, int loopFlag, float volume) {
	// ループ再生かつ、すでに再生中の場合は、二重に再生されないように処理を抜ける
	if (loopFlag && IsPlayingAudio(soundData)) {
		return;
	}

	// 再生済みの音声を破棄
	ClearFinishedVoices();

	// 波形フォーマットを基にSourceVoiceの生成
	IXAudio2SourceVoice *pSourceVoice = nullptr;
	HRESULT result = xAudio2_->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
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
void Audio::StopAudio(const SoundData &soundData) {
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
void Audio::PauseAudio(const SoundData &soundData) {
	auto it = playVoices_.find(soundData.pBuffer);
	if (it != playVoices_.end()) {
		// 配列内のすべてのボイスを一時停止
		for (auto *pSourceVoice : it->second) {
			if (pSourceVoice) { pSourceVoice->Stop(0); }
		}
	}
}

// 音データ一括再開
void Audio::ResumeAudio(const SoundData &soundData) {
	auto it = playVoices_.find(soundData.pBuffer);
	if (it != playVoices_.end()) {
		// 配列内のすべてのボイスを再開
		for (auto *pSourceVoice : it->second) {
			if (pSourceVoice) { pSourceVoice->Start(0); }
		}
	}
}

// 音データ一括音量設定
void Audio::SetAudioVolume(const SoundData &soundData, float volume) {
	auto it = playVoices_.find(soundData.pBuffer);
	if (it != playVoices_.end()) {
		// 配列内のすべてのボイスの音量を変更
		for (auto *pSourceVoice : it->second) {
			if (pSourceVoice) { pSourceVoice->SetVolume(volume); }
		}
	}
}

// 再生中か取得 (1つでも鳴っていれば再生中と判定)
bool Audio::IsPlayingAudio(const SoundData &soundData) const {
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