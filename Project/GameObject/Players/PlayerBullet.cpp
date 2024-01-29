#include "PlayerBullet.h"
#include "Utility/CollisionManager/CollisionConfig.h"
#include "Utility/CollisionManager/Collider.h"
#include "Engine/3D/Matrix/WorldTransform.h" 
#include "Base/TextureManager.h"
#include <cassert>

void PlayerBullet::Initialize(const Vector3& pos, const Vector3& velocity) {

	bulletModel_.reset(Model::CreateFromOBJ("Resources", "cube.obj"));

	// テクスチャ読み込み
	bulletTexture_ = TextureManager::Load("Resources/uvChecker.png");

	// 衝突属性を設定
	SetCollisionAttribute(kCollisionAttributePlayer);
	// 衝突対象を自分の属性以外に設定
	SetCollisionMask(~kCollisionAttributePlayer);

	// ワールド変換の初期化
	bulletModel_->worldTransform.Initialize();
	// 引数で受け取った初期座標をセット
	bulletModel_->worldTransform.translation_ = pos;

	// 引数で受け取った速度をメンバ変数に代入
	velocity_ = velocity;
}

void PlayerBullet::Update() {
	// Y軸周り角度(θy)
	bulletModel_->worldTransform.rotation_.y = std::atan2(velocity_.x, velocity_.z);
	// 横軸方向の長さを求める
	float velocityXZ;
	velocityXZ = sqrt(velocity_.x * velocity_.x + velocity_.z * velocity_.z);
	// X軸周りの角度(θx)
	bulletModel_->worldTransform.rotation_.x = std::atan2(-velocity_.y, velocityXZ);

	// 座標を移動させる
	bulletModel_->worldTransform.translation_ = Add(bulletModel_->worldTransform.translation_, velocity_);

	// 行列を更新
	bulletModel_->worldTransform.UpdateMatrix();

	// 時間経過で死ぬ
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}
}

void PlayerBullet::Draw(const ViewProjection& viewProjection) {
	// モデルの描画
	bulletModel_->Draw(bulletworldTransform_,viewProjection);
}

Vector3 PlayerBullet::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos{};
	// ワールド行列の平行移動成分を取得
	worldPos.x = bulletModel_->worldTransform.matWorld_.m[3][0];
	worldPos.y = bulletModel_->worldTransform.matWorld_.m[3][1];
	worldPos.z = bulletModel_->worldTransform.matWorld_.m[3][2];

	return worldPos;
}