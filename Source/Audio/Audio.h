#pragma once

#pragma comment(lib, "xaudio2.lib")

#include <wrl.h>
#include <xaudio2.h>
#include <vector>
#include <unordered_map>
#include "AudioData.h"

/// <summary>
/// オーディオシステムを管理するクラス (XAudio2制御専用)
/// </summary>
/// <remarks>
/// XAudio2の初期化・終了、およびAudioManagerから受け取った
/// AudioDataに基づく音声の再生・停止・一時停止・音量変更などを制御する。
/// </remarks>
class Audio {
public:
	/// <summary>
	/// オーディオシステムを初期化する
	/// </summary>
	/// <remarks>
	/// XAudio2エンジンを初期化し、マスターボイスを生成する。
	/// </remarks>
	void Initialize();

	/// <summary>
	/// オーディオシステムを終了する
	/// </summary>
	/// <remarks>
	/// 再生中のボイスをすべて破棄し、XAudio2の終了処理を行う。
	/// </remarks>
	void Finalize();

	/// <summary>
	/// 音声を再生する
	/// </summary>
	/// <param name="soundData">再生する音声データ</param>
	/// <param name="loopFlag">ループ再生するか</param>
	/// <param name="volume">再生音量</param>
	/// <remarks>
	/// SourceVoiceを生成し、音声データを送信して再生する。
	/// 多重再生に対応。
	/// </remarks>
	void PlayAudio(const AudioData &soundData, bool loopFlag = false, float volume = 1.0f);

	/// <summary>
	/// 音声の再生を停止する
	/// </summary>
	/// <param name="soundData">停止する音声データ</param>
	/// <remarks>
	/// 対応するすべてのSourceVoiceを停止・破棄する。
	/// </remarks>
	void StopAudio(const AudioData &soundData);

	/// <summary>
	/// 音声を一時停止する
	/// </summary>
	/// <param name="soundData">対象の音声データ</param>
	/// <remarks>
	/// 再生位置を保持したまま停止する。
	/// </remarks>
	void PauseAudio(const AudioData &soundData);

	/// <summary>
	/// 一時停止した音声を再開する
	/// </summary>
	/// <param name="soundData">対象の音声データ</param>
	/// <remarks>
	/// PauseAudioで停止した音声を再び再生する。
	/// </remarks>
	void ResumeAudio(const AudioData &soundData);

	/// <summary>
	/// 音量を変更する
	/// </summary>
	/// <param name="soundData">対象の音声データ</param>
	/// <param name="volume">設定する音量</param>
	/// <remarks>
	/// 再生中のすべてのボイスへ同じ音量を設定する。
	/// </remarks>
	void SetAudioVolume(const AudioData &soundData, float volume);

	/// <summary>
	/// 音声が再生中か取得する
	/// </summary>
	/// <param name="soundData">対象の音声データ</param>
	/// <returns>再生中ならtrue、それ以外はfalse</returns>
	/// <remarks>
	/// 1つ以上のSourceVoiceが再生中ならtrueを返す。
	/// </remarks>
	bool IsPlayingAudio(const AudioData &soundData) const;

	/// <summary>
	/// 再生終了したボイスを破棄する
	/// </summary>
	/// <remarks>
	/// メインループなどで毎フレーム呼び出し、
	/// 再生が完了したボイスを安全にクリーンアップする。
	/// </remarks>
	void ClearFinishedVoices();

private:
	// XAudio2 基本インターフェース
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
	IXAudio2MasteringVoice *masterVoice_ = nullptr;

	// 多重再生ボイスの管理マップ
	// キー: AudioDataが持つデータバッファの先頭アドレス (const BYTE*)
	// 値: 対象のデータバッファから生成されたアクティブなSourceVoiceのリスト
	std::unordered_map<const BYTE *, std::vector<IXAudio2SourceVoice *>> playVoices_;
};