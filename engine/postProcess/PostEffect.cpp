#include "PostEffect.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "WinApp.h" 

PostEffect::PostEffect() : Sprite(UVCHEKER)
{

}

void PostEffect::Initialize() {
	directXCommon_ = DirectXCommon::GetInstance();
	HRESULT result;

	// テクスチャリソースの設定
	CD3DX12_RESOURCE_DESC textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		WinApp::kClientWidth_,
		(UINT)WinApp::kClientHeight_,
		1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
	);

	// テクスチャバッファの生成
	D3D12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	D3D12_CLEAR_VALUE clearColor = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, clearColor_);
	result = directXCommon_->GetDevice()->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&textureDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&clearColor,
		IID_PPV_ARGS(&texBuff_)
	);
	assert(SUCCEEDED(result));

	// テクスチャの設定
	// 画素数(1280 x 720 = 921600)
	const UINT pixelCount = WinApp::kClientWidth_ * WinApp::kClientHeight_;
	// 画像1行分のデータサイズ
	const UINT rowPitch = sizeof(UINT) * WinApp::kClientWidth_;
	// 画像全体のデータサイズ
	const UINT depthPitch = rowPitch * WinApp::kClientHeight_;
	// 画像イメージ
	UINT* img = new UINT[pixelCount];
	for (UINT i = 0; i < pixelCount; i++) {
		img[i] = 0xff0000ff;
	}

	// テクスチャバッファにデータ転送
	result = texBuff_->WriteToSubresource(
		0, nullptr,
		img,
		rowPitch,
		depthPitch
	);
	assert(SUCCEEDED(result));
	delete[] img;

	// SRV用のデスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC srvDescHeapDesc = {};
	srvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvDescHeapDesc.NumDescriptors = 1;
	// SRV用のデスクリプタヒープを生成
	result = directXCommon_->GetDevice()->CreateDescriptorHeap(&srvDescHeapDesc, IID_PPV_ARGS(&descHeapSRV_));
	assert(SUCCEEDED(result));
	// SRV設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	// デスクリプタヒープにSRV作成
	uint32_t descriptorSizeSRV{};
	descriptorSizeSRV = DirectXCommon::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU{};
	srvHandleCPU_ = GetCPUDescriptorHandle(directXCommon_->GetSrvDescriptorHeap(), descriptorSizeSRV, 1);
	srvHandleGPU_ = GetGPUDescriptorHandle(directXCommon_->GetSrvDescriptorHeap(), descriptorSizeSRV, 1);

	directXCommon_->GetDevice()->CreateShaderResourceView(texBuff_.Get(),
		&srvDesc,
		srvHandleCPU_
	);

	// RTV用のデスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc{};
	rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescriptorHeapDesc.NumDescriptors = 1;
	// RTV用のデスクリプタヒープ
	result = directXCommon_->GetDevice()->CreateDescriptorHeap(&rtvDescriptorHeapDesc, IID_PPV_ARGS(&descHeapRTV_));
	assert(SUCCEEDED(result));
	// レンダーターゲットビューの設定
	D3D12_RENDER_TARGET_VIEW_DESC renderTargetViewDesc{};
	// シェーダの計算結果をSRGBに変換して書き込む
	renderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	renderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	// デスクリプタヒープにRTV作成
	directXCommon_->GetDevice()->CreateRenderTargetView(texBuff_.Get(),
		&renderTargetViewDesc,
		descHeapRTV_->GetCPUDescriptorHandleForHeapStart()
	);

	// 深度バッファリソース設定
	CD3DX12_RESOURCE_DESC depthResDesc =
		CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_D32_FLOAT,
			WinApp::kClientWidth_,
			WinApp::kClientHeight_,
			1, 0,
			1, 0,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
		);
	// 深度バッファの生成
	D3D12_HEAP_PROPERTIES depthHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_CLEAR_VALUE clearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0F, 0);
	result = directXCommon_->GetDevice()->CreateCommittedResource(
		&depthHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&clearValue,
		IID_PPV_ARGS(&depthBuff_)
	);
	assert(SUCCEEDED(result));

	// DSV用のデスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC DSVHeapDesc{};
	DSVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	DSVHeapDesc.NumDescriptors = 1;
	// DSV用デスクリプタヒープを作成
	result = directXCommon_->GetDevice()->CreateDescriptorHeap(&DSVHeapDesc, IID_PPV_ARGS(&descHeapDSV_));
	assert(SUCCEEDED(result));
	// デスクリプタヒープにDSV作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	directXCommon_->GetDevice()->CreateDepthStencilView(depthBuff_.Get(),
		&dsvDesc,
		descHeapDSV_->GetCPUDescriptorHandleForHeapStart()
	);
}

void PostEffect::Draw() {
	// ワールド座標の更新
	worldTransform_.UpdateMatrix();

	/// コマンドを積む
	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootSignature(psoManager_->GetRootSignature()[0].Get());
	DirectXCommon::GetInstance()->GetCommandList()->SetPipelineState(psoManager_->GetGraphicsPipelineState()[0].Get()); // PSOを設定
	DirectXCommon::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 描画用のDescriptorHeapの設定
	//Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> ppHeaps[] = { descHeapSRV_.Get() };
	//directXCommon_->GetCommandList()->SetDescriptorHeaps(1, ppHeaps->GetAddressOf());

	// VBVを設定
	directXCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	directXCommon_->GetCommandList()->IASetIndexBuffer(&indexBufferView_);

	// worldTransform
	directXCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, worldTransform_.constBuff_->GetGPUVirtualAddress());
	// viewProjection
	directXCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(4, viewProjection_.constBuff_->GetGPUVirtualAddress());
	directXCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(5, cameraPosResource_.Get()->GetGPUVirtualAddress());

	/// DescriptorTableの設定
	// texture
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(2, srvHandleGPU_);
	// material
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_.Get()->GetGPUVirtualAddress());
	// ライティング
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(3, DirectionalLight::GetInstance()->GetDirectionalLightResource()->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(6, PointLight::GetInstance()->GetPointLightResource()->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(7, SpotLight::GetInstance()->GetSpotLightResource()->GetGPUVirtualAddress());


	// 描画(DrawCall/ドローコール)。6頂点で1つのインスタンス
	DirectXCommon::GetInstance()->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void PostEffect::PreDrawScene() {
	// バリアを張る
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(texBuff_.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	directXCommon_->GetCommandList()->ResourceBarrier(
		1, &barrier
	);
	// レンダーターゲットビューのデスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = descHeapRTV_->GetCPUDescriptorHandleForHeapStart();
	// 深度ステンシルビューのデスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = descHeapDSV_->GetCPUDescriptorHandleForHeapStart();
	// レンダーターゲットをセット
	directXCommon_->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

	// ビューポートの設定
	D3D12_VIEWPORT viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, (float)WinApp::kClientWidth_, (float)WinApp::kClientHeight_);
	directXCommon_->GetCommandList()->RSSetViewports(1, &viewport);
	// シザー矩形の設定
	D3D12_RECT scissor = CD3DX12_RECT(0, 0, WinApp::kClientWidth_, WinApp::kClientHeight_);
	directXCommon_->GetCommandList()->RSSetScissorRects(1, &scissor);

	// 全画面クリア
	directXCommon_->GetCommandList()->ClearRenderTargetView(rtvHandle, clearColor_, 0, nullptr);
	// 深度バッファのクリア
	directXCommon_->GetCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void PostEffect::PostDrawScene() {
	// バリアの変更
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(texBuff_.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	directXCommon_->GetCommandList()->ResourceBarrier(1, &barrier);
}

D3D12_CPU_DESCRIPTOR_HANDLE PostEffect::GetCPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap.Get()->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE PostEffect::GetGPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap.Get()->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}

const float PostEffect::clearColor_[4] = { 0.25f, 0.5f, 0.1f, 0.0f };