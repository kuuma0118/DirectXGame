#include "EnemyStateproximity.h"
#include "EnemyStateLeave.h"
#include "Enemy.h"
#include "MathStructs.h"

EnemyStateproximity::~EnemyStateproximity() {
	for (TimedCall* timedCall : timedCalls_) {
		delete timedCall;
	}
}

void EnemyStateproximity::FireAndResetTimer() {
	// 弾を発射する
	enemy_->Fire();
	// 発射タイマーをセットする
	timedCalls_.push_back(
		new TimedCall(std::bind(&EnemyStateproximity::FireAndResetTimer, this), kFireInterval));
}

void EnemyStateproximity::Initialize(Enemy* enemy) {
	enemy_ = enemy;
	FireAndResetTimer();
}

void EnemyStateproximity::Update(Enemy* enemy) {
	// 移動速度
	const Vector3 kMoveSpeed = { 0, 0, -0.10f };

	// 移動処理
	enemy->Move(kMoveSpeed);

	// 終了したタイマーを削除
	timedCalls_.remove_if([](TimedCall* timedCall) {
		if (timedCall->IsFinished()) {
			delete timedCall;
			return true;
		}
		return false;
	});
	// 範囲forでリストの全要素について回す
	for (TimedCall* timedCall : timedCalls_) {
		timedCall->Update();
	}

	// 既定の位置に到達したら離脱
	if (enemy->GetEnemyPos().z < 60.0f) {
		timedCalls_.clear();
		enemy->ChangeState(new EnemyStateLv());
	}
}
