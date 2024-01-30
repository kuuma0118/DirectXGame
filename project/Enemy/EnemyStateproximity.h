#pragma once
#include "IEnemyState.h"
#include "TimedCall.h"
#include <list>

class Enemy; // 前方宣言


class EnemyStateproximity : public IEnemyState {
public:
	~EnemyStateproximity();

	/// <summary>
	/// 弾を発射してタイマーをリセット
	/// </summary>
	void FireAndResetTimer();

	// 初期化
	void Initialize(Enemy* enemy);

	// 更新処理
	void Update(Enemy* enemy);

public:
	Enemy* enemy_;
	// 発射間隔
	static const int kFireInterval = 240;
	// 時限発動
	std::list<TimedCall*> timedCalls_;
};