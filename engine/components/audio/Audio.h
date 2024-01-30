#pragma once
#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")
#include <fstream>
#include <wrl.h>

//#include <Mfreadwrite.h>
//#include <Windows.h>
//#include <audioclient.h>
//#include <audiopolicy.h>
//#include <mmdeviceapi.h>
//#include <mfapi.h>
//#include <mfidl.h>
//
//#pragma comment(lib, "mfplat.lib")
//#pragma comment(lib, "mfuuid.lib")

// チャンクヘッダ
struct ChunkHeader {
	char id[4];	  // チャンク毎のID
	int32_t size; // チャンクサイズ
};

// RIFFヘッダチャンク
struct RiffHeader {
	ChunkHeader chunk;	// "RIFF"
	char type[4];		// "WAVE"
};

// FMTチャンク
struct FormatChunk {
	ChunkHeader chunk; // "fmt"
	WAVEFORMATEX fmt;  // 波形フォーマット
};

// 音声データ
struct SoundData {
	// 波形フォーマット
	WAVEFORMATEX wfex;
	// バッファの先頭アドレス
	BYTE* pBuffer;
	// バッファのサイズ
	unsigned int bufferSize;
};

class Audio
{
public:
	static Audio* GetInstance();

	void Initialize();

	// 音声データの読み込み
	SoundData SoundLoadWave(const char* filename);

	// 音声データ解放
	void SoundUnload(SoundData* soundData);

	// 音声再生
	void SoundPlayWave(IXAudio2* xAudio2, const SoundData& soundData, float volume = 0.5f);

	// 音声のループ再生
	void SoundPlayLoopingWave(IXAudio2* xAudio2, const SoundData& soundData, float volume = 0.5f);

	//template <class T>
	//void SafeRelease(T** ppT) {
	//	if (*ppT) {
	//		(*ppT)->Release();
	//		*ppT = NULL;
	//	}
	//}

	//HRESULT InitializeMediaFoundation();

	//void ShutdownMediaFoundation();
 //   HRESULT LoadMP3File(const wchar_t* mp3FilePath, IMFSourceReader** sourceReader) {
 //       HRESULT hr = S_OK;

 //       IMFSourceReader* pReader = NULL;

 //       hr = MFCreateSourceReaderFromURL(mp3FilePath, NULL, &pReader);
 //       if (SUCCEEDED(hr)) {
 //           *sourceReader = pReader;
 //       }
 //       else {
 //           SafeRelease(&pReader);
 //       }

 //       return hr;
 //   }

 //   HRESULT ConfigureAudioOutput(IMFSourceReader* sourceReader, const wchar_t* wavFilePath) {
 //       HRESULT hr = S_OK;

 //       IMFMediaType* pMediaType = NULL;
 //       hr = sourceReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pMediaType);

 //       if (SUCCEEDED(hr)) {
 //           WAVEFORMATEX* pWaveFormat = NULL;
 //           hr = MFCreateWaveFormatExFromMFMediaType(pMediaType, &pWaveFormat);
 //           if (SUCCEEDED(hr)) {
 //               // WAVファイルに書き出す
 //               hr = WriteWavHeader(wavFilePath, pWaveFormat);
 //               CoTaskMemFree(pWaveFormat);
 //           }
 //       }

 //       SafeRelease(&pMediaType);

 //       return hr;
 //   }

 //   HRESULT WriteWavHeader(const wchar_t* wavFilePath, WAVEFORMATEX* pWaveFormat) {
 //       // WAVヘッダーを書き出す
 //       std::ofstream wavFile(wavFilePath, std::ios::binary);
 //       if (!wavFile.is_open()) {
 //           return E_FAIL;
 //       }

 //       // WAVヘッダーの構築
 //       WAVEFORMATEXTENSIBLE wfx = {};
 //       wfx.Format = *pWaveFormat;
 //       wfx.Samples.wValidBitsPerSample = wfx.Format.wBitsPerSample;
 //       wfx.dwChannelMask = 0; // Assume mono

 //       // RIFF チャンク
 //       wavFile.write("RIFF", 4);
 //       DWORD dataSize = 0; // 後で埋める
 //       wavFile.write(reinterpret_cast<char*>(&dataSize), sizeof(DWORD));
 //       wavFile.write("WAVE", 4);

 //       // フォーマットチャンク
 //       wavFile.write("fmt ", 4);
 //       DWORD formatSize = sizeof(WAVEFORMATEXTENSIBLE);
 //       wavFile.write(reinterpret_cast<char*>(&formatSize), sizeof(DWORD));
 //       wavFile.write(reinterpret_cast<char*>(&wfx), sizeof(WAVEFORMATEXTENSIBLE));

 //       // データチャンク
 //       wavFile.write("data", 4);
 //       DWORD dataSizePos = static_cast<DWORD>(wavFile.tellp());
 //       dataSize = 0; // 後で埋める
 //       wavFile.write(reinterpret_cast<char*>(&dataSize), sizeof(DWORD));

 //       // ファイルを閉じる
 //       wavFile.close();

 //       return S_OK;
 //   }

 //   HRESULT DecodeAndWriteAudioData(IMFSourceReader* sourceReader, const wchar_t* wavFilePath) {
 //       HRESULT hr = S_OK;

 //       IMFMediaBuffer* pBuffer = NULL;
 //       DWORD dwFlags = 0;
 //       LONGLONG llTimestamp = 0;
 //       IMFSample* pSample = NULL;

 //       // ファイルをオープンしてバイナリデータを書き込む
 //       std::ofstream wavFile(wavFilePath, std::ios::binary | std::ios::app);
 //       if (!wavFile.is_open()) {
 //           return E_FAIL;
 //       }

 //       while (true) {
 //           hr = sourceReader->ReadSample(
 //               (DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM,
 //               0,
 //               NULL,
 //               &dwFlags,
 //               &llTimestamp,
 //               &pSample
 //           );

 //           if (FAILED(hr) || dwFlags & MF_SOURCE_READERF_ENDOFSTREAM) {
 //               break; // デコード終了またはエラー
 //           }

 //           if (SUCCEEDED(hr)) {
 //               hr = pSample->ConvertToContiguousBuffer(&pBuffer);
 //               if (SUCCEEDED(hr)) {
 //                   BYTE* pAudioData = NULL;
 //                   DWORD cbBuffer = 0;
 //                   hr = pBuffer->Lock(&pAudioData, NULL, &cbBuffer);
 //                   if (SUCCEEDED(hr)) {
 //                       // WAVファイルにバイナリデータを書き込む
 //                       wavFile.write(reinterpret_cast<char*>(pAudioData), cbBuffer);

 //                       pBuffer->Unlock();
 //                   }
 //               }
 //           }

 //           SafeRelease(&pSample);
 //           SafeRelease(&pBuffer);
 //       }

 //       // ファイルサイズを更新
 //       if (SUCCEEDED(hr)) {
 //           hr = UpdateWavFileSize(wavFilePath);
 //       }

 //       wavFile.close();

 //       return hr;
 //   }

 //   HRESULT UpdateWavFileSize(const wchar_t* wavFilePath);
};

