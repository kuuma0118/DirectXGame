#pragma once
#include "IScene.h"
// エンジンの基本機能
#include "Input.h"

// User
#include "Model.h"
#include "ViewProjection.h"
#include "Particles.h"
#include "Player.h"
#include "PlayerBullet.h"
#include "Enemy.h"
#include "EnemyBullet.h"
#include "Skydome.h"
#include "RailCamera.h"
#include "CollisionManager.h"

#include <list>

class GameManager;
class GameScene : public IScene
{
public:
	///
	/// Default Method
	/// 

	// 初期化
	void Initialize()override;

	// 更新処理
	void Update()override;

	// 描画
	void Draw()override;

	// 解放処理
	void Finalize()override;

	/// 
	/// User Method
	/// 
	
	// 弾リストを取得
	const std::list<PlayerBullet*>& GetPlayerBullets() const { return playerBullets_; }

	Player* GetPlayer() { return player_; }

	// 弾リストを取得
	const std::list<EnemyBullet*>& GetEnemyBullets() const { return enemyBullets_; }

	// 敵を発生させる
	void SpawnEnemy(Vector3 pos);

	void SpawnParticles(Vector3 pos);

	// CSVを読み込む
	void LoadEnemyPopData();

	// 
	void UpdateEnemyPopCommands();

	void AddPlayerBullet(PlayerBullet* playerBullet);

	void AddEnemyBullet(EnemyBullet* enemyBullet);
private:
	// 基本機能
	Input* input_;

	// モデル
	Model* playerModel_ = nullptr;
	Model* enemyModel_ = nullptr;
	Model* skydomeModel_ = nullptr;

	// カメラ
	ViewProjection viewProjection_;
	// 座標
	WorldTransform worldTransform_;

	// 自キャラ
	Player* player_ = nullptr;
	// 自弾
	std::list<PlayerBullet*> playerBullets_;
	// 敵
	std::list<Enemy*> enemy_;
	// 敵弾
	std::list<EnemyBullet*> enemyBullets_;
	// 衝突マネージャー
	CollisionManager* collisionManager_ = nullptr;
	// 天球
	Skydome* skydome_ = nullptr;
	// カメラレール
	RailCamera* railCamera_ = nullptr;
	// パーティクル
	std::list<Particles*> particles_;

	// 敵の発生時間や座標などのCSVファイル
	std::stringstream enemyPopCommands_;
	// 敵が発生待機中か
	bool isWait_ = false;
	// 敵が発生するまでの時間
	int32_t waitTime_ = 0;

	// 倒した敵の数
	int killCount_;
	int eachCount_[3];
	int boxCount_;

	int endCount_;
	int isEnd_;

	Sprite* UI_[2];
};

