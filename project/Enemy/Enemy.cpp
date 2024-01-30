#include "Enemy.h"
#include "EnemyStateApproach.h"
#include "EnemyStateLeave.h"

#include "Player.h"

#include "CollisionConfig.h"

#include "Lerp.h"
#include "MathStructs.h"

#include "WorldTransform.h"
#include "Model.h"

#include "GameScene.h"

#include <cassert>
#include <stdio.h>

Enemy::Enemy() {
	state_ = new EnemyStateApproach();
}

Enemy::~Enemy() {
	for (EnemyBullet* bullet : bullets_) {
		delete bullet;
	}
}

void Enemy::Initialize(const Vector3& pos) {
	// objモデル割り当て
	model_ = Model::CreateModelFromObj(CUBE);
	// NULLポインタチェック
	assert(model_);
	// テクスチャ読み込み
	enemyTexture_ = ENEMY;

	// 衝突属性を設定
	SetCollisionAttribute(kCollisionAttributeEnemy);
	// 衝突対象を自分の属性以外に設定
	SetCollisionMask(~kCollisionAttributeEnemy);

	// ワールド変換の初期化
	model_->worldTransform.Initialize();
	// 引数で受け取った初期座標をセット
	model_->worldTransform.translation_ = pos;

	// 状態遷移
	state_->Initialize(this);

	deadSignal_ = false;
}

void Enemy::Move(const Vector3 velocity) {
	model_->worldTransform.translation_ = Add(model_->worldTransform.translation_, velocity);
}

void Enemy::Update() {
	// 状態遷移
	state_->Update(this);
	// 行列の更新
	model_->worldTransform.UpdateMatrix();
	// 行列を定数バッファに転送
	model_->worldTransform.TransferMatrix();

	if (deadSignal_) {
		deadSignal_ = false;
	}
}

void Enemy::ChangeState(IEnemyState* pState) {
	delete state_;
	state_ = pState;
}

void Enemy::Draw(ViewProjection& viewProjection) {
	// enemy
	model_->Draw(viewProjection, enemyTexture_);
}

void Enemy::Fire() {
	assert(player_);

	// 弾の速度(正の数だと敵の後ろから弾が飛ぶ)
	const float kBulletSpeed = -0.5f;
	Vector3 velocity{ 0, 0, kBulletSpeed };

	// 自キャラのワールド座標を取得する
	player_->GetWorldPosition();

	// 弾を生成し、初期化
	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->Initialize(model_->worldTransform.translation_, velocity);
	newBullet->SetPlayer(player_);

	// 弾を登録
	gameScene_->AddEnemyBullet(newBullet);
}

void Enemy::OnCollision() {
	isDead_ = true;
	deadSignal_ = true;
}

Vector3 Enemy::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos{};
	// ワールド行列の平行移動成分を取得
	worldPos.x = model_->worldTransform.matWorld_.m[3][0];
	worldPos.y = model_->worldTransform.matWorld_.m[3][1];
	worldPos.z = model_->worldTransform.matWorld_.m[3][2];

	return worldPos;
}
