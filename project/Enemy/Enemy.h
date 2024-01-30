#pragma once
#include "IEnemyState.h"
#include "EnemyBullet.h"
#include "Collider.h"
#include "Model.h"
#include "WorldTransform.h"
#include "TimedCall.h"

class Player;
class GameScene;

class Enemy : public Collider {
public:
	// メンバ関数

	Enemy();
	~Enemy();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const Vector3& pos);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// stateの変更
	/// </summary>
	/// <param name="pState">state</param>
	void ChangeState(IEnemyState* pState);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ViewProjection& viewProjection);

	void Move(const Vector3 velocity);

	/// <summary>
	/// 発射処理
	/// </summary>
	void Fire();

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision() override;

	// Getter
	Vector3 GetEnemyPos() { return model_->worldTransform.translation_; }

	Vector3 GetWorldPosition() override;

	// 弾リストを取得
	const std::list<EnemyBullet*>& GetBullets() const { return bullets_; }

	void SetPlayer(Player* player) { player_ = player; }
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

	// 完了ならtrueを返す
	bool IsDead() const { return isDead_; }

	void SetIsDead() { isDead_ = true; }

	bool GetDeadSignal() { return deadSignal_; }

private:
	// 状態遷移
	IEnemyState* state_;

	// ワールド変換データ
	//WorldTransform worldTransform_;

	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t enemyTexture_ = 0u;

	// 弾
	std::list<EnemyBullet*> bullets_;
	// 自キャラ
	Player* player_ = nullptr;

	// ゲームシーン
	GameScene* gameScene_ = nullptr;

	bool isDead_ = false;

	// 倒されたときに信号を送る
	bool deadSignal_;
};

