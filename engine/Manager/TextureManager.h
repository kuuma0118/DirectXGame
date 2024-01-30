#pragma once
#include "../../externals/DirectXTex/d3dx12.h"
#include "../../externals/DirectXTex/DirectXTex.h"
#include "DirectXCommon.h"
#include "MathStructs.h"
#include "ModelStructs.h"
#include <string>
#include <d3d12.h>
#include <wrl.h>

enum TextureName {
	UVCHEKER,
	MONSTERBALL,
	ENEMY,
	PLAYER,
	BULLET,
	SKYDOME,
	RETICLE,
	PARTICLE,
	BACKGROUND,
	ZERO,
	ONE,
	TWO,
	THREE,
	FOUR,
	FIVE,
	SIX,
	SEVEN,
	EIGHT,
	NINE,
	UI_CLEAR,
	UI_GAMEOVER,
	UI_TITLE,
	UI_PAD_A,
	UI_PAD_RB,
	UI_SHOT,
	COUNTTEXTURE	// テクスチャ数を数える用
};

class TextureManager
{
public:
	// シングルトン
	static TextureManager* GetInstance();

	// デストラクタ
	~TextureManager() = default;

	// 初期化
	void Initialize();

	// textureを読んで転送する
	void TransferTexture();

	// 解放処理
	void Finalize();

	// COMの終了処理
	void ComUninit();
	// COMの初期化
	void ComInit();

	/// Getter
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE* GetTextureSrvHandleGPU() { return textureSrvHandleGPU_; }
	// textureResource
	Microsoft::WRL::ComPtr<ID3D12Resource> GetTextureResource(uint32_t index) { return textureResource_[index].Get(); }

	// Textureを読む
	DirectX::ScratchImage LoadTexture(const std::string& filePath);

	// DirectX12のTextureResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);

	// 中間リソースの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device, size_t sizeInBytes);

	// TextureResourceにデータを転送する
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(const Microsoft::WRL::ComPtr<ID3D12Resource>& texture, const DirectX::ScratchImage& mipImages);

private:
	static const uint32_t kMaxImages = COUNTTEXTURE;
	DirectX::ScratchImage mipImages_[kMaxImages];
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU_[kMaxImages];
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_[kMaxImages];
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource_[kMaxImages];
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource_[kMaxImages];

	// objデータ
	static const int32_t kMaxObjModelData = 1;
	ModelData* modelData_;
};
