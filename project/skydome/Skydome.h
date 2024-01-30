#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "MathStructs.h"

class Skydome {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const Vector3& pos);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ViewProjection& viewProjection);

private:
	// ワールド変換データ
	//WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
};
