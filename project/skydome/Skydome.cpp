#include "Skydome.h"
#include "TextureManager.h"
#include <cassert>

void Skydome::Initialize(const Vector3& pos) {
	model_ = Model::CreateModelFromObj(SPHERE);
	// NULLポインタチェック
	assert(model_);

	// ワールド変換の初期化
	model_->worldTransform.Initialize();
	// 引数で受け取った初期座標をセット
	model_->worldTransform.translation_ = pos;

	// 天球の大きさ(大きさによってはカメラのfarZをSetterで設定しよう)
	model_->worldTransform.scale_.x = 1000.0f;
	model_->worldTransform.scale_.y = 1000.0f;
	model_->worldTransform.scale_.z = 1000.0f;
}

void Skydome::Update() {
	// 行列の更新
	model_->worldTransform.UpdateMatrix();
	// 行列を定数バッファに転送
	model_->worldTransform.TransferMatrix();
}

void Skydome::Draw(ViewProjection& viewProjection) {
	model_->Draw(viewProjection, SKYDOME);
}