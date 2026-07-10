#pragma once

#pragma comment(lib, "xaudio2.lib")

#include <wrl.h>
#include <xaudio2.h>
#include <vector>
#include <map>
#include <cstdint>
#include <fstream>

/// <summary>
/// RIFFチャンク共通ヘッダ
/// </summary>
/// <remarks>
/// チャンクIDとデータサイズを保持する。
/// WAVファイル解析時に利用される。
/// </remarks>
struct ChunkHeader {
	char id[4]{};
	int32_t size = 0;
};

/// <summary>
/// RIFFヘッダ
/// </summary>
/// <remarks>
/// WAVファイルの先頭に存在するヘッダ。
/// ファイル種別を判定するために使用する。
/// </remarks>
struct RiffHeader {
	ChunkHeader chunk{};
	char type[4]{};
};

/// <summary>
/// WAVフォーマット情報
/// </summary>
/// <remarks>
/// サンプリング周波数やチャンネル数など、
/// 波形データのフォーマット情報を保持する。
/// </remarks>
struct FormatChunk {
	ChunkHeader chunk{};
	WAVEFORMATEX fmt{};
};

/// <summary>
/// 読み込んだ音声データ
/// </summary>
/// <remarks>
/// 波形フォーマット・PCMデータ・
/// バッファサイズを保持する。
/// Audioクラスの再生処理で使用される。
/// </remarks>
struct AudioData {
	WAVEFORMATEXTENSIBLE wfex{};	// 波形フォーマット
	BYTE *pBuffer = nullptr;		// バッファの先頭アドレス
	UINT32 bufferSize = 0;			// バッファのサイズ
};

/// <summary>
/// オーディオシステムを管理するクラス
/// </summary>
/// <remarks>
/// XAudio2とMedia Foundationを利用して、
/// wav・mp3ファイルの読み込みや再生、停止、
/// 一時停止、音量変更などを提供する。
/// 同一音声の多重再生にも対応している。
/// </remarks>
class Audio {
public:
	/// <summary>
	/// オーディオシステムを初期化する
	/// </summary>
	/// <remarks>
	/// XAudio2とMedia Foundationを初期化し、
	/// 音声再生を行える状態にする。
	/// </remarks>
	void Initialize();

	/// <summary>
	/// オーディオシステムを終了する
	/// </summary>
	/// <remarks>
	/// 再生中のボイスを破棄し、
	/// XAudio2・Media Foundationの終了処理を行う。
	/// </remarks>
	void Finalize();

	/// <summary>
	/// 音声ファイルを読み込む
	/// </summary>
	/// <param name="filename">音声ファイル名</param>
	/// <returns>読み込んだ音声データ</returns>
	/// <remarks>
	/// 拡張子を判定し、
	/// wavまたはmp3の読み込み処理へ振り分ける。
	/// </remarks>
	AudioData LoadAudio(const std::string &filename);

	/// <summary>
	/// WAVファイルを読み込む
	/// </summary>
	/// <param name="filename">wavファイル名</param>
	/// <returns>読み込んだ音声データ</returns>
	/// <remarks>
	/// RIFFヘッダを解析し、
	/// PCMデータをメモリへ読み込む。
	/// </remarks>
	AudioData LoadWav(const char *filename);	// wavデータ

	/// <summary>
	/// MP3ファイルを読み込む
	/// </summary>
	/// <param name="filename">mp3ファイル名</param>
	/// <returns>読み込んだ音声データ</returns>
	/// <remarks>
	/// Media Foundationを利用して
	/// PCMデータへ変換して読み込む。
	/// </remarks>
	AudioData LoadMp3(const wchar_t *filename);	// mp3データ

	/// <summary>
	/// 音声データを解放する
	/// </summary>
	/// <param name="soundData">解放する音声データ</param>
	/// <remarks>
	/// 再生を停止した後、
	/// メモリ上のPCMデータを解放する。
	/// </remarks>
	void UnloadAudio(AudioData *soundData);

	/// <summary>
	/// 音声を再生する
	/// </summary>
	/// <param name="soundData">再生する音声データ</param>
	/// <param name="loopFlag">ループ再生するか</param>
	/// <param name="volume">再生音量</param>
	/// <remarks>
	/// SourceVoiceを生成し、
	/// 音声データを送信して再生する。
	/// 多重再生にも対応している。
	/// </remarks>
	void PlayAudio(const AudioData &soundData, int loopFlag = false, float volume = 1.0f);

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
	/// 再生中のすべてのボイスへ
	/// 同じ音量を設定する。
	/// </remarks>
	void SetAudioVolume(const AudioData &soundData, float volume);

	/// <summary>
	/// 音声が再生中か取得する
	/// </summary>
	/// <param name="soundData">対象の音声データ</param>
	/// <returns>
	/// 再生中ならtrue、それ以外はfalse
	/// </returns>
	/// <remarks>
	/// 1つ以上のSourceVoiceが再生中ならtrueを返す。
	/// </remarks>
	bool IsPlayingAudio(const AudioData &soundData) const;

private:
	/// <summary>
	/// 再生終了したボイスを破棄する
	/// </summary>
	/// <remarks>
	/// バッファ再生が完了したSourceVoiceを検出し、
	/// 停止・破棄して管理コンテナから削除する。
	/// </remarks>
	void ClearFinishedVoices();

private:
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
	IXAudio2MasteringVoice *masterVoice_ = nullptr;

	// 音声データ管理
	std::map<BYTE*, std::vector<IXAudio2SourceVoice*>> playVoices_;
};