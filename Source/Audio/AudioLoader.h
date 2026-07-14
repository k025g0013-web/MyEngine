#pragma once

#include <string>
#include <fstream>
#include <wrl.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

#include "AudioData.h"

/// <summary>
/// 音声ファイル読み込みクラス
/// </summary>
/// <remarks>
/// wav・mp3ファイルを読み込み、
/// AudioDataを生成する。
/// 再生処理は行わない。
/// </remarks>
class AudioLoader {
public:

	/// <summary>
	/// 音声ファイルを読み込む
	/// </summary>
	/// <param name="filename">音声ファイル名</param>
	/// <returns>読み込んだ音声データ</returns>
	/// <remarks>
	/// 拡張子を判定し、
	/// WAVまたはMP3の読み込み処理へ振り分ける。
	/// </remarks>
	AudioData LoadAudio(const std::string &filename);

	/// <summary>
	/// WAVファイルを読み込む
	/// </summary>
	/// <param name="filename">WAVファイル名</param>
	/// <returns>読み込んだ音声データ</returns>
	/// <remarks>
	/// RIFF形式のWAVファイルを解析し、
	/// 波形データをAudioDataへ格納する。
	/// </remarks>
	AudioData LoadWav(const char *filename);

	/// <summary>
	/// MP3ファイルを読み込む
	/// </summary>
	/// <param name="filename">MP3ファイル名</param>
	/// <returns>読み込んだ音声データ</returns>
	/// <remarks>
	/// Media Foundationを利用して
	/// PCMデータへ変換し、
	/// AudioDataを生成する。
	/// </remarks>
	AudioData LoadMp3(const wchar_t *filename);

private:

	//=========================
	// WAV
	//=========================

	/// <summary>
	/// RIFFヘッダを読み込む
	/// </summary>
	/// <param name="file">入力ファイル</param>
	/// <remarks>
	/// WAVファイルがRIFF形式であることを確認する。
	/// </remarks>
	void ReadRiffHeader(std::ifstream &file);

	/// <summary>
	/// フォーマットチャンクを読み込む
	/// </summary>
	/// <param name="file">入力ファイル</param>
	/// <returns>波形フォーマット情報</returns>
	/// <remarks>
	/// サンプリング周波数やチャンネル数など、
	/// 音声フォーマット情報を取得する。
	/// </remarks>
	WAVEFORMATEXTENSIBLE ReadFormatChunk(std::ifstream &file);

	/// <summary>
	/// 波形データを読み込む
	/// </summary>
	/// <param name="file">入力ファイル</param>
	/// <returns>PCMデータ</returns>
	/// <remarks>
	/// dataチャンクを読み込み、
	/// PCMデータをメモリへ格納する。
	/// </remarks>
	std::vector<BYTE> ReadWaveData(std::ifstream &file);

	//=========================
	// MP3
	//=========================

	/// <summary>
	/// SourceReaderを生成する
	/// </summary>
	/// <param name="filename">MP3ファイル名</param>
	/// <returns>生成したSourceReader</returns>
	/// <remarks>
	/// Media Foundationを利用して
	/// MP3ファイルを読み込むための
	/// SourceReaderを生成する。
	/// </remarks>
	Microsoft::WRL::ComPtr<IMFSourceReader> CreateSourceReader(const wchar_t *filename);

	/// <summary>
	/// 出力フォーマットを設定する
	/// </summary>
	/// <param name="reader">SourceReader</param>
	/// <remarks>
	/// MP3データをPCM形式で取得できるよう
	/// 出力フォーマットを設定する。
	/// </remarks>
	void ConfigureOutputFormat(IMFSourceReader *reader);

	/// <summary>
	/// PCMデータを読み込む
	/// </summary>
	/// <param name="reader">SourceReader</param>
	/// <returns>読み込んだ音声データ</returns>
	/// <remarks>
	/// SourceReaderからPCMデータを取得し、
	/// AudioDataを生成する。
	/// </remarks>
	AudioData ReadMp3Samples(IMFSourceReader *reader);
};