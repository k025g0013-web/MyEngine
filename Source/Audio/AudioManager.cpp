#include "AudioManager.h"

#include <cassert>

void AudioManager::Initialize() {
	// オーディオシステムを初期化
	audio_.Initialize();
}

void AudioManager::Finalize() {
	// オーディオシステムを終了
	audio_.Finalize();

	// 読み込み済み音声データを解放
	audioData_.clear();
}

void AudioManager::Load(const std::string &name, const std::string &filename) {
	// 同じ管理名が登録されていないことを確認
	assert(audioData_.find(name) == audioData_.end());

	
	// 音声ファイルを読み込み登録
	
	audioData_.emplace(name, loader_.LoadAudio(filename));
}

void AudioManager::Play(const std::string &name, bool loop, float volume) {
	// 音声を再生
	audio_.PlayAudio(GetAudioData(name), loop, volume);
}

void AudioManager::Stop(const std::string &name) {
	// 音声を停止
	audio_.StopAudio(GetAudioData(name));
}

void AudioManager::Pause(const std::string &name) {
	// 音声を一時停止
	audio_.PauseAudio(GetAudioData(name));
}

void AudioManager::Resume(const std::string &name) {
	// 一時停止した音声を再開
	audio_.ResumeAudio(GetAudioData(name));
}

void AudioManager::SetVolume(const std::string &name, float volume) {
	// 音量を変更
	audio_.SetAudioVolume(GetAudioData(name), volume);
}

bool AudioManager::IsPlaying(const std::string &name) const {
	// 音声の再生状態を取得
	return audio_.IsPlayingAudio(GetAudioData(name));
}

AudioData &AudioManager::GetAudioData(const std::string &name) {
	// 管理名に対応する音声データを検索
	auto it = audioData_.find(name);

	assert(it != audioData_.end());

	return it->second;
}

const AudioData &AudioManager::GetAudioData(const std::string &name) const {
	// 管理名に対応する音声データを検索
	auto it = audioData_.find(name);

	assert(it != audioData_.end());

	return it->second;
}