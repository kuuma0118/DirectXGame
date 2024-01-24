#pragma once
#include "IScene.h"
/// Engine↓
// Components
#include "Engine/Components/Input.h"
#include "Engine/Components/Audio.h"
#include "Engine/Components/PostProcess.h"
// 3D
#include "Engine/3D/Model/Model.h"
// 2D
#include "Engine/2D/Sprite.h"
// Utility
#include "Engine/Utility/CollisionManager/CollisionManager.h"

/// Project↓
// GameObject

class GameTitleScene : public IScene {
public:
	//トランジションの時間
	static const int kTransitionTime = 60;
	/// <summary>
	/// コンストラクタ
	/// </summary>
	GameTitleScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameTitleScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(GameManager* gameManager) override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update(GameManager* gameManager) override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(GameManager* gameManager) override;

private:// メンバ変数
#pragma region エンジンの基本機能
	//TextureManager
	TextureManager* textureManager_ = nullptr;
	//Input
	Input* input_ = nullptr;
	//オーディオクラス
	Audio* audio_ = nullptr;
	//ポストプロセス
	PostProcess* postProcess_ = nullptr;
	// 当たり判定
	CollisionManager* collisionManager_ = nullptr;

#pragma endregion

#pragma region ゲームオブジェクト

	//ビュープロジェクション(カメラ)
	ViewProjection viewProjection_;
	//ワールドトランスフォーム(ブロックの発生場所)
	WorldTransform worldTransform_;

	//サウンド
	uint32_t soundHandle_ = 0u;
	uint32_t titleSoundHandle_ = 0u;

	int soundCount_ = 0;

	//タイトル用のスプライト
	std::unique_ptr<Sprite>  titleSprite_ = nullptr;
	//タイトルのテクスチャ
	uint32_t titleTextureHandle_ = 0;
	//トランジション用のスプライト
	std::unique_ptr<Sprite> transitionSprite_ = nullptr;
	//トランジションのテクスチャ
	uint32_t transitionTextureHandle_ = 0;
	//トランジションの色
	Vector4 transitionColor_ = { 0.0f,0.0f,0.0f,1.0f };
	//トランジションのフラグ
	bool isTransition_ = false;
	bool isTransitionEnd_ = false;
	//トランジションのタイマー
	float transitionTimer_ = 0;

#pragma endregion
};