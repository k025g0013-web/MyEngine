#pragma once

#include <string>
#include <unordered_map>

#include "Audio.h"
#include "AudioLoader.h"

/// <summary>
/// オーディオ管理クラス
/// </summary>
/// <remarks>
/// 音声データの読み込み・管理を担当する。
/// AudioLoaderとAudioを仲介し、
/// 名前による音声の再生を提供する。
/// </remarks>
class AudioManager {
public:

	/// <summary>
	/// オーディオシステムを初期化する
	/// </summary>
	void Initialize();

	/// <summary>
	/// オーディオシステムを終了する
	/// </summary>
	void Finalize();

	/// <summary>
	/// 音声を読み込む
	/// </summary>
	/// <param name="name">管理名</param>
	/// <param name="filename">音声ファイル</param>
	void Load(
		const std::string &name,
		const std::string &filename);

	/// <summary>
	/// 音声を再生する
	/// </summary>
	/// <param name="name">管理名</param>
	/// <param name="loop">ループ再生するか</param>
	/// <param name="valume">再生音量</param>
	/// <remarks>
	/// SourceVoiceを生成し、
	/// 音声データを送信して再生する。
	/// 多重再生にも対応している。
	/// </remarks>
	void Play(
		const std::string &name,
		bool loop = false,
		float volume = 1.0f);

	/// <summary>
	/// 音声を停止する
	/// </summary>
	/// <param name="name">管理名</param>
	/// <remarks>
	/// 対応するすべてのSourceVoiceを停止・破棄する。
	/// </remarks>
	void Stop(const std::string &name);

	/// <summary>
	/// 音声を一時停止する
	/// </summary>
	/// <param name="name">管理名</param>
	/// <remarks>
	/// 再生位置を保持したまま停止する。
	/// </remarks>
	void Pause(const std::string &name);

	/// <summary>
	/// 音声を再開する
	/// </summary>
	/// <param name="name">管理名</param>
	/// <remarks>
	/// PauseAudioで停止した音声を再び再生する。
	/// </remarks>
	void Resume(const std::string &name);

	/// <summary>
	/// 音量を設定する
	/// </summary>
	/// <param name="name">管理名</param>
	/// <param name="valume">再生音量</param>
	/// <remarks>
	/// 再生中のすべてのボイスへ
	/// 同じ音量を設定する。
	/// </remarks>
	void SetVolume(
		const std::string &name,
		float volume);

	/// <summary>
	/// 再生中か取得
	/// </summary>
	/// <param name="name">管理名</param>
	/// <returns>
	/// 再生中ならtrue、それ以外はfalse
	/// </returns>
	/// <remarks>
	/// 1つ以上のSourceVoiceが再生中ならtrueを返す。
	/// </remarks>
	bool IsPlaying(const std::string &name) const;

private:
	// === Getter ===
	
	/// <summary>
	/// 音声データを取得する
	/// </summary>
	/// <param name="name">管理名</param>
	/// <returns>対応する音声データ</returns>
	/// <remarks>
	/// 管理名に対応するAudioDataを取得する。
	/// 登録されていない場合はアサートで停止する。
	/// </remarks>
	AudioData &GetAudioData(const std::string &name);

	/// <summary>
	/// 音声データを取得する（読み取り専用）
	/// </summary>
	/// <param name="name">管理名</param>
	/// <returns>対応する音声データ</returns>
	/// <remarks>
	/// 管理名に対応するAudioDataを取得する。
	/// 読み取り専用で利用する。
	/// 登録されていない場合はアサートで停止する。
	/// </remarks>
	const AudioData &GetAudioData(const std::string &name) const;

	/// <summary>
	/// 再生終了したボイスを破棄する (中継用)
	/// </summary>
	void ClearFinishedVoices() { audio_.ClearFinishedVoices(); }

private:
	/// 読み込み済み音声
	std::unordered_map<std::string, AudioData> audioData_;

	/// 音声読込
	AudioLoader loader_;

	/// 音声再生
	Audio audio_;
};