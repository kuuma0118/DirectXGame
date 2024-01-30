#include "Sphere.h"
#include "ImGuiManager.h"
#include "PipelineManager.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include <cassert>
#define _USE_MATH_DEFINES
#include <math.h>

void Sphere::Initialize() {
	CreateVertexResource();

	CreateMaterialResource();

	CreateWvpResource();

	CreateVertexBufferView();

	// 書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	uvTransform_ = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};

	const float kLonEvery = 2.0f * float(M_PI) / float(kSubdivision);//経度分割1つ分の角度
	const float kLatEvery = float(M_PI) / float(kSubdivision);//緯度分割1つ分の角度
	// 緯度の方向に分割
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = float(-M_PI) / 2.0f + kLatEvery * latIndex;
		// 経度の方向に分割しながら線を描く
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
			float lon = lonIndex * kLonEvery;
			// 頂点データを入力する。
#pragma region 1枚目
			// 基準点a
			vertexData_[start].position.x = cosf(lat) * cosf(lon);
			vertexData_[start].position.y = sinf(lat);
			vertexData_[start].position.z = cosf(lat) * sinf(lon);
			vertexData_[start].position.w = 1.0f;
			vertexData_[start].texcoord = { float(lonIndex) / float(kSubdivision) ,1.0f - float(latIndex) / float(kSubdivision) };
			vertexData_[start].normal.x = vertexData_[start].position.x;
			vertexData_[start].normal.y = vertexData_[start].position.y;
			vertexData_[start].normal.z = vertexData_[start].position.z;
			// b
			vertexData_[start + 1].position.x = cosf(lat + kLatEvery) * cosf(lon);
			vertexData_[start + 1].position.y = sinf(lat + kLatEvery);
			vertexData_[start + 1].position.z = cosf(lat + kLatEvery) * sinf(lon);
			vertexData_[start + 1].position.w = 1.0f;
			vertexData_[start + 1].texcoord = { vertexData_[start].texcoord.x,vertexData_[start].texcoord.y - (1.0f / kSubdivision) };
			vertexData_[start + 1].normal.x = vertexData_[start + 1].position.x;
			vertexData_[start + 1].normal.y = vertexData_[start + 1].position.y;
			vertexData_[start + 1].normal.z = vertexData_[start + 1].position.z;
			// c
			vertexData_[start + 2].position.x = cosf(lat) * cosf(lon + kLonEvery);
			vertexData_[start + 2].position.y = sinf(lat);
			vertexData_[start + 2].position.z = cosf(lat) * sinf(lon + kLonEvery);
			vertexData_[start + 2].position.w = 1.0f;
			vertexData_[start + 2].texcoord = { vertexData_[start].texcoord.x + (1.0f / (float)kSubdivision),vertexData_[start].texcoord.y };
			vertexData_[start + 2].normal.x = vertexData_[start + 2].position.x;
			vertexData_[start + 2].normal.y = vertexData_[start + 2].position.y;
			vertexData_[start + 2].normal.z = vertexData_[start + 2].position.z;

#pragma endregion

#pragma region 2枚目

			// b
			vertexData_[start + 3].position.x = cosf(lat + kLatEvery) * cosf(lon);
			vertexData_[start + 3].position.y = sinf(lat + kLatEvery);
			vertexData_[start + 3].position.z = cosf(lat + kLatEvery) * sinf(lon);
			vertexData_[start + 3].position.w = 1.0f;
			vertexData_[start + 3].texcoord = { vertexData_[start].texcoord.x,vertexData_[start].texcoord.y - (1.0f / (float)kSubdivision) };
			vertexData_[start + 3].normal.x = vertexData_[start + 3].position.x;
			vertexData_[start + 3].normal.y = vertexData_[start + 3].position.y;
			vertexData_[start + 3].normal.z = vertexData_[start + 3].position.z;
			// d
			vertexData_[start + 4].position.x = cosf(lat + kLatEvery) * cosf(lon + kLonEvery);
			vertexData_[start + 4].position.y = sinf(lat + kLatEvery);
			vertexData_[start + 4].position.z = cosf(lat + kLatEvery) * sinf(lon + kLonEvery);
			vertexData_[start + 4].position.w = 1.0f;
			vertexData_[start + 4].texcoord = { vertexData_[start].texcoord.x + (1.0f / (float)kSubdivision),vertexData_[start].texcoord.y - (1.0f / (float)kSubdivision) };
			vertexData_[start + 4].normal.x = vertexData_[start + 4].position.x;
			vertexData_[start + 4].normal.y = vertexData_[start + 4].position.y;
			vertexData_[start + 4].normal.z = vertexData_[start + 4].position.z;
			// c
			vertexData_[start + 5].position.x = cosf(lat) * cosf(lon + kLonEvery);
			vertexData_[start + 5].position.y = sinf(lat);
			vertexData_[start + 5].position.z = cosf(lat) * sinf(lon + kLonEvery);
			vertexData_[start + 5].position.w = 1.0f;
			vertexData_[start + 5].texcoord = { vertexData_[start].texcoord.x + (1.0f / (float)kSubdivision),vertexData_[start].texcoord.y };
			vertexData_[start + 5].normal.x = vertexData_[start + 5].position.x;
			vertexData_[start + 5].normal.y = vertexData_[start + 5].position.y;
			vertexData_[start + 5].normal.z = vertexData_[start + 5].position.z;

#pragma endregion

		}
	}

	materialData_->color = { 1.0f,1.0f,1.0f,1.0f };
	// Lightingするか
	materialData_->enableLighting = true;
	// uvTransform行列の初期化
	materialData_->uvTransform = MakeIdentity4x4();
	materialData_->shininess = 20;

	// particleの座標指定
	vertexIndex;
}

void Sphere::Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection) {
	uvTransformMatrix_ = MakeScaleMatrix(uvTransform_.scale);
	uvTransformMatrix_ = Multiply(uvTransformMatrix_, MakeRotateZMatrix(uvTransform_.rotate.z));
	uvTransformMatrix_ = Multiply(uvTransformMatrix_, MakeTranslateMatrix(uvTransform_.translate));
	materialData_->uvTransform = uvTransformMatrix_;

	cameraPosData_ = viewProjection.translation_;

	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootSignature(PipelineManager::GetInstance()->GetRootSignature()[1].Get());
	DirectXCommon::GetInstance()->GetCommandList()->SetPipelineState(PipelineManager::GetInstance()->GetGraphicsPipelineState()[1].Get()); // PSOを設定

	// コマンドを積む
	DirectXCommon::GetInstance()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_); // VBVを設定

	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(1, worldTransform.constBuff_->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(4, viewProjection.constBuff_->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(5, cameraPosResource_.Get()->GetGPUVirtualAddress());

	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	DirectXCommon::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// DescriptorTableの設定
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(2, useMonsterBall_ ? TextureManager::GetInstance()->GetTextureSrvHandleGPU()[MONSTERBALL] : TextureManager::GetInstance()->GetTextureSrvHandleGPU()[UVCHEKER]);

	// マテリアルCBufferの場所を設定
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_.Get()->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(3, DirectionalLight::GetInstance()->GetDirectionalLightResource()->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(6, PointLight::GetInstance()->GetPointLightResource()->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(7, SpotLight::GetInstance()->GetSpotLightResource()->GetGPUVirtualAddress());

	DirectXCommon::GetInstance()->GetCommandList()->DrawInstanced(vertexIndex, 1, 0, 0);
}

//void Sphere::Release() {
//
//}

void Sphere::ImGuiAdjustParameter() {
	ImGui::CheckboxFlags("isLighting", &materialData_->enableLighting, 1);
	ImGui::Checkbox("useMonsterBall", &useMonsterBall_);
	ImGui::Text("UvTransform");
	ImGui::SliderFloat2("UvTranslate", &uvTransform_.translate.x, -5, 5);
	ImGui::SliderFloat2("UvScale", &uvTransform_.scale.x, -5, 5);
	ImGui::SliderAngle("UvRotate.z", &uvTransform_.rotate.z);
	ImGui::DragFloat("shininess", &materialData_->shininess, 0.01f, 0, 50);
}

Microsoft::WRL::ComPtr<ID3D12Resource> Sphere::CreateBufferResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device, size_t sizeInBytes) {
	HRESULT hr;
	// 頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; // UploadHeapを使う
	// 頂点リソースの設定
	D3D12_RESOURCE_DESC vertexResourceDesc{};
	// バッファソース。テクスチャの場合はまた別の設定をする
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeInBytes; // リソースのサイズ。今回はVector4を3頂点分
	// バッファの場合はこれからは1にする決まり
	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;
	// バッファの場合はこれにする決まり
	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	// 実際に頂点リソースを作る
	hr = device.Get()->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexResource));
	assert(SUCCEEDED(hr));

	return vertexResource;
}

void Sphere::CreateVertexResource() {
	vertexResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(VertexData) * vertexIndex).Get();
}

void Sphere::CreateVertexBufferView() {
	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_.Get()->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * vertexIndex;
	// 1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
}

void Sphere::CreateMaterialResource() {
	materialResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(Material)).Get();
	// マテリアルにデータを書き込む
	materialData_ = nullptr;
	// 書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
}

void Sphere::CreateWvpResource() {
	// 1つ分のサイズを用意する
	cameraPosResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(Vector3)).Get();
	// 書き込むためのアドレスを取得
	cameraPosResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraPosData_));
}