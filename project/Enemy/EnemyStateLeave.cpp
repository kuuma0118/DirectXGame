#include "EnemyStateLeave.h"
#include "Enemy.h"
#include "MathStructs.h"

void EnemyStateLv::Initialize(Enemy* enemy) { enemy_ = enemy; }

void EnemyStateLv::Update(Enemy* enemy) {
	// 移動速度
	const Vector3 kMoveSpeed = { -0.05f, 0.05f, -0.05f };

	// 移動処理
	enemy->Move(kMoveSpeed);
}