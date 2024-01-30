#pragma once
#include "RailCamera.h"
#include "ImGuiManager.h"

void RailCamera::Initialize(WorldTransform worldTransform, const Vector3& radian) {
	// ワールドトランスフォームの初期設定
	// 引数で受け取った初期座標をセット
	worldTransform_ = worldTransform;
	worldTransform_.translation_.z = -10;

	// 引数で受け取った初期座標をセット
	worldTransform_.rotation_ = radian;

	// ビュープロジェクションの初期化
	viewProjection_.Initialize();
}

void RailCamera::Update() {
	const Vector3 kSpeed{0.0f, 0.0f, 0.02f};
	Vector3 radian{0.0f, 0.001f, 0.0f};
	worldTransform_.translation_ = Add(worldTransform_.translation_, kSpeed);

	// 行列の更新
	worldTransform_.UpdateMatrix();
	// カメラオブジェクトのワールド行列からビュー行列を計算する
	viewProjection_.matView = Inverse(worldTransform_.matWorld_);
}