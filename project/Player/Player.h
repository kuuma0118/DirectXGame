#pragma once
#include "PlayerBullet.h"
#include "Input.h"
#include "Collider.h"
#include "Model.h"
#include "WorldTransform.h"
#include "MathStructs.h"
#include "Sprite.h"
#include <list>

class GameScene;

class Player : public Collider {
public:
	Player();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Player();

	/// <summary>
	/// 初期化
	/// <summary>
	void Initialize(uint32_t textureHandle, const Vector3& pos);

	/// <summary>
	/// 更新
	/// <summary>
	void Update(const ViewProjection& viewProjection);

	/// <summary>
	/// 描画
	/// <summary>
	void Draw(ViewProjection& viewProjection);

	///
	/// User Method
	/// 

	/// <summary>
	/// UI描画
	/// </summary>
	void DrawUI();

	/// <summary>
	/// レティクルの配置
	/// </summary>
	void Deploy3DReticle();

	/// <summary>
	/// 2Dレティクルの配置
	/// </summary>
	void Deploy2DReticle(const ViewProjection& viewProjection);

	// playerの回転
	void Rotate();

	/// <summary>
	/// 攻撃
	/// </summary>
	void Attack();

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision() override;

	// ワールド行列の平行移動成分を取得
	Vector3 GetWorldPosition() override;
	// 弾リストを取得
	const std::list<PlayerBullet*>& GetBullets() const { return bullets_; }
	// レティクルの座標
	Vector3 GetWorld3DReticlePosition();

	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

	/// <summary>
	/// 親となるワールドトランスフォームをセット
	/// </summary>
	/// <param name="parent">親となるワールドトランスフォーム</param>
	void SetParent(const WorldTransform* parent);

public:
	// キーボード入力
	Input* input_ = nullptr;

	// ワールド変換データ
	//WorldTransform worldTransform_;
	// 3Dレティクル用ワールドトランスフォーム
	WorldTransform worldTransform3DReticle_;

	// モデル
	Model* model_ = nullptr;
	// 2Dレティクル用のスプライト
	Sprite* sprite2DReticle_ = nullptr;

	// テクスチャハンドル
	uint32_t playerTexture_ = 0u;
	// レティクルハンドル
	uint32_t reticleTexture_ = 0u;

	// 弾
	std::list<PlayerBullet*> bullets_;

	GameScene* gameScene_;

	bool isDead_ = true;

	Matrix4x4 matViewport_;
	Vector2 spritePosition_;
};