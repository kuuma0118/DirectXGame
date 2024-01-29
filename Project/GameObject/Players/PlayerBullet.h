#pragma once
#include "Utility/CollisionManager/Collider.h"
#include "3D/Model/Model.h"
#include "Engine/3D/Matrix/WorldTransform.h"

/// <summary>
/// 自キャラの弾
/// </summary>
class PlayerBullet : public Collider {
public: // メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="pos">初期座標</param>
	void Initialize(const Vector3& pos, const Vector3& velocity);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection">ビュープロジェクション</param>
	void Draw(const ViewProjection& viewProjection);

	// Getter
	bool IsDead() const { return isDead_; }
	// ワールド行列の平行移動成分を取得
	Vector3 GetWorldPosition() override;
	
	bool GetIsHit() { return IsHit_; };

private: // メンバ変数
	// ワールド変換データ
	WorldTransform bulletworldTransform_;
	// モデル
	std::unique_ptr<Model> bulletModel_ = nullptr;
	// テクスチャハンドル
	uint32_t bulletTexture_ = 0u;

	// 寿命<frm>
	static const int32_t kLifeTime = 60 * 5;
	// 死亡タイマー
	int32_t deathTimer_ = kLifeTime;
	// 死亡フラグ
	bool isDead_ = false;

	bool IsHit_ = false;

	// 速度
	Vector3 velocity_;
};