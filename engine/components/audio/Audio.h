#pragma once
#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")
#include <fstream>
#include <wrl.h>

// チャンクヘッダ
struct ChunkHeader {
	char id[4];	  
	int32_t size; 
};

// RIFFヘッダチャンク
struct RiffHeader {
	ChunkHeader chunk;
	char type[4];		
};

// FMTチャンク
struct FormatChunk {
	ChunkHeader chunk; 
	WAVEFORMATEX fmt;  
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

};

