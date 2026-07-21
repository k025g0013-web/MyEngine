#include "Audio.h"
#include <cassert>
#include <cstring>
#include <algorithm>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

namespace Kizuna {
	void Audio::Initialize() {
		// XAudio2エンジンのインスタンスを作成
		HRESULT hr = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
		assert(SUCCEEDED(hr));

		// マスターボイス（最終出力ボイス）を生成
		hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
		assert(SUCCEEDED(hr));

		// Media Foundationの初期化
		hr = MFStartup(MF_VERSION);
		assert(SUCCEEDED(hr));
	}

	void Audio::Finalize() {
		// すべての再生中ボイスを停止・破棄
		for (auto &pair : playVoices_) {
			for (auto *pSourceVoice : pair.second) {
				if (pSourceVoice) {
					pSourceVoice->Stop(0);
					pSourceVoice->DestroyVoice();
				}
			}
		}
		playVoices_.clear();

		// マスターボイスの破棄
		if (masterVoice_) {
			masterVoice_->DestroyVoice();
			masterVoice_ = nullptr;
		}

		// XAudio2オブジェクトの解放
		xAudio2_.Reset();

		// Media Foundationの終了処理
		MFShutdown();
	}

	// 音データ再生 (多重再生対応)
	void Audio::PlayAudio(const AudioData &soundData, bool loopFlag, float volume) {
		// ループ再生かつ、すでに再生中の場合は、二重に再生されないように処理を抜ける
		if (loopFlag && IsPlayingAudio(soundData)) {
			return;
		}

		// 再生済みの音声を破棄
		ClearFinishedVoices();

		// 波形フォーマットを基にSourceVoiceの生成
		IXAudio2SourceVoice *pSourceVoice = nullptr;
		HRESULT result = xAudio2_->CreateSourceVoice(
			&pSourceVoice, reinterpret_cast<const WAVEFORMATEX *>(&soundData.waveFormat)
		);
		assert(SUCCEEDED(result));

		// 再生する波形データの設定
		XAUDIO2_BUFFER buf{};
		buf.pAudioData = soundData.buffer.data();
		buf.AudioBytes = static_cast<UINT32>(soundData.buffer.size());
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

		// 管理マップに登録（キーにはバッファの先頭アドレスを使用）
		playVoices_[soundData.buffer.data()].push_back(pSourceVoice);
	}

	// 音データ一括停止
	void Audio::StopAudio(const AudioData &soundData) {
		auto it = playVoices_.find(soundData.buffer.data());
		if (it != playVoices_.end()) {
			// 配列内のすべてのボイスをループで停止・破棄
			for (auto *pSourceVoice : it->second) {
				if (pSourceVoice) {
					pSourceVoice->Stop(0);
					pSourceVoice->FlushSourceBuffers();
					pSourceVoice->DestroyVoice();
				}
			}
			// マップから削除
			playVoices_.erase(it);
		}
	}

	// 音データ一括一時停止
	void Audio::PauseAudio(const AudioData &soundData) {
		auto it = playVoices_.find(soundData.buffer.data());
		if (it != playVoices_.end()) {
			// 配列内のすべてのボイスを一時停止
			for (auto *pSourceVoice : it->second) {
				if (pSourceVoice) {
					pSourceVoice->Stop(0);
				}
			}
		}
	}

	// 音データ一括再開
	void Audio::ResumeAudio(const AudioData &soundData) {
		auto it = playVoices_.find(soundData.buffer.data());
		if (it != playVoices_.end()) {
			// 配列内のすべてのボイスを再開
			for (auto *pSourceVoice : it->second) {
				if (pSourceVoice) {
					pSourceVoice->Start(0);
				}
			}
		}
	}

	// 音データ一括音量設定
	void Audio::SetAudioVolume(const AudioData &soundData, float volume) {
		auto it = playVoices_.find(soundData.buffer.data());
		if (it != playVoices_.end()) {
			// 配列内のすべてのボイスの音量を変更
			for (auto *pSourceVoice : it->second) {
				if (pSourceVoice) {
					pSourceVoice->SetVolume(volume);
				}
			}
		}
	}

	// 再生中か取得 (1つでも鳴っていれば再生中と判定)
	bool Audio::IsPlayingAudio(const AudioData &soundData) const {
		auto it = playVoices_.find(soundData.buffer.data());
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

	void Audio::ClearFinishedVoices() {
		// 登録されているすべての音声データを走査
		for (auto it = playVoices_.begin(); it != playVoices_.end();) {
			auto &voiceVector = it->second;

			//=================================================================
			// 再生が終了したSourceVoiceを削除
			//=================================================================
			voiceVector.erase(
				std::remove_if(
					voiceVector.begin(),
					voiceVector.end(),
					[](IXAudio2SourceVoice *pSourceVoice) {
						// nullptrは不要なので削除対象
						if (!pSourceVoice) {
							return true;
						}

						// 現在の再生状態を取得
						XAUDIO2_VOICE_STATE state;
						pSourceVoice->GetState(&state);

						// キューに再生データが残っていなければ再生終了
						if (state.BuffersQueued == 0) {
							// 念のため停止してからボイスを破棄する
							pSourceVoice->Stop(0);
							pSourceVoice->DestroyVoice();
							return true;
						}

						// まだ再生中なので保持
						return false;
					}),
				voiceVector.end());

			//=================================================================
			// この音声データに紐づくボイスがすべて無くなった場合は、
			// 管理用のmapからも削除する。
			//=================================================================
			if (voiceVector.empty()) {
				it = playVoices_.erase(it);
			} else {
				++it;
			}
		}
	}
}