#include "AudioLoader.h"
#include "AudioFormat.h"

#include <cassert>
#include <cstring>
#include <filesystem>

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

// === 定数定義 (このcppファイル内でのみ有効) ===
namespace {
	// チャンクIDのバイトサイズ
	constexpr size_t kChunkIdSize = 4;

	// サポートする拡張子リテラル
	const std::string kExtWav = ".wav";
	const std::string kExtMp3 = ".mp3";
}

AudioData AudioLoader::LoadAudio(const std::string &filename) {
	// ファイルの拡張子を判定
	std::filesystem::path path(filename);

	// WAVファイル
	if (path.extension() == kExtWav) {
		return LoadWav(filename.c_str());
	}

	// MP3ファイル
	if (path.extension() == kExtMp3) {
		std::wstring wpath(filename.begin(), filename.end());
		return LoadMp3(wpath.c_str());
	}

	assert(false && "対応していない音声形式");

	return {};
}

AudioData AudioLoader::LoadWav(const char *filename) {
	// WAVファイルを開く
	std::ifstream file(filename, std::ios::binary);
	assert(file.is_open());

	//==================================================
	// WAVファイルを解析
	//==================================================

	// RIFFヘッダ
	ReadRiffHeader(file);

	// フォーマット情報
	AudioData audioData;
	audioData.waveFormat = ReadFormatChunk(file);

	// PCMデータ
	audioData.buffer = ReadWaveData(file);

	return audioData;
}

void AudioLoader::ReadRiffHeader(std::ifstream &file) {
	// RIFFヘッダを読み込む
	RiffHeader riff{};
	file.read(reinterpret_cast<char *>(&riff), sizeof(riff));

	// RIFF形式であることを確認
	assert(std::strncmp(riff.chunk.id, "RIFF", kChunkIdSize) == 0);

	// WAVE形式であることを確認
	assert(std::strncmp(riff.type, "WAVE", kChunkIdSize) == 0);
}

WAVEFORMATEXTENSIBLE AudioLoader::ReadFormatChunk(std::ifstream &file) {
	//==================================================
	// fmtチャンクを読み込む
	//==================================================
	FormatChunk format{};
	file.read(reinterpret_cast<char *>(&format.chunk), sizeof(ChunkHeader));

	assert(std::strncmp(format.chunk.id, "fmt ", kChunkIdSize) == 0);

	file.read(reinterpret_cast<char *>(&format.fmt), format.chunk.size);

	//==================================================
	// WAVEFORMATEXTENSIBLEへ変換
	//==================================================
	WAVEFORMATEXTENSIBLE waveFormat{};
	
	memcpy(&waveFormat, &format.fmt, sizeof(format.fmt));

	return waveFormat;
}

std::vector<BYTE> AudioLoader::ReadWaveData(std::ifstream &file) {
	//==================================================
	// dataチャンクを検索
	//==================================================
	ChunkHeader data{};
	file.read(reinterpret_cast<char *>(&data), sizeof(data));

	// JUNKチャンクをスキップ
	if (std::strncmp(data.id, "JUNK", kChunkIdSize) == 0) {
		file.seekg(data.size, std::ios::cur);
		file.read(reinterpret_cast<char *>(&data), sizeof(data));
	}

	//==================================================
	// PCMデータを読み込む
	//==================================================
	std::vector<BYTE> buffer(data.size);
	file.read(reinterpret_cast<char *>(buffer.data()), data.size);

	return buffer;
}

AudioData AudioLoader::LoadMp3(const wchar_t *filename) {
	//==================================================
	// SourceReaderを生成
	//==================================================
	auto reader = CreateSourceReader(filename);

	// PCM形式へ変換
	ConfigureOutputFormat(reader.Get());

	// PCMデータを取得
	return ReadMp3Samples(reader.Get());
}

Microsoft::WRL::ComPtr<IMFSourceReader>
AudioLoader::CreateSourceReader(const wchar_t *filename) {
	//==================================================
	// SourceReaderを生成
	//==================================================
	Microsoft::WRL::ComPtr<IMFSourceReader> reader;

	HRESULT hr =
		MFCreateSourceReaderFromURL(filename, nullptr, &reader);

	assert(SUCCEEDED(hr));

	return reader;
}

void AudioLoader::ConfigureOutputFormat(IMFSourceReader *reader) {
	//==================================================
	// PCM形式のMediaTypeを生成
	//==================================================
	IMFMediaType *mediaType = nullptr;

	MFCreateMediaType(&mediaType);

	mediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);

	mediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);

	//==================================================
	// SourceReaderへ設定
	//==================================================
	reader->SetCurrentMediaType(
		static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), nullptr, mediaType);

	mediaType->Release();
}

AudioData AudioLoader::ReadMp3Samples(IMFSourceReader *reader) {
	//==================================================
	// PCMフォーマットを取得
	//==================================================
	IMFMediaType *currentType = nullptr;

	HRESULT hr = reader->GetCurrentMediaType(
		static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), &currentType);

	assert(SUCCEEDED(hr));

	WAVEFORMATEX *waveFormat = nullptr;
	UINT32 waveFormatSize = 0;

	hr = MFCreateWaveFormatExFromMFMediaType(currentType, &waveFormat, &waveFormatSize);

	assert(SUCCEEDED(hr));

	currentType->Release();

	//==================================================
	// PCMデータを読み込む
	//==================================================
	std::vector<BYTE> audioBuffer;

	while (true) {
		IMFSample *sample = nullptr;
		DWORD flags = 0;

		hr = reader->ReadSample(
			static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), 0, nullptr, &flags, nullptr, &sample);

		assert(SUCCEEDED(hr));

		// ファイル終端
		if (flags & MF_SOURCE_READERF_ENDOFSTREAM) {
			break;
		}

		// サンプルが取得できなければ次へ
		if (sample == nullptr) {
			continue;
		}

		IMFMediaBuffer *mediaBuffer = nullptr;

		hr = sample->ConvertToContiguousBuffer(&mediaBuffer);
		assert(SUCCEEDED(hr));

		BYTE *buffer = nullptr;
		DWORD maxLength = 0;
		DWORD currentLength = 0;

		mediaBuffer->Lock(
			&buffer, &maxLength, &currentLength);

		audioBuffer.insert(
			audioBuffer.end(), buffer, buffer + currentLength);

		mediaBuffer->Unlock();

		mediaBuffer->Release();
		sample->Release();
	}

	//==================================================
	// AudioDataを生成
	//==================================================
	AudioData audioData{};

	std::memcpy(
		&audioData.waveFormat, waveFormat, waveFormatSize);

	audioData.buffer = std::move(audioBuffer);

	CoTaskMemFree(waveFormat);

	return audioData;
}