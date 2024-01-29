#pragma once
#include "3D/Model/Model.h"
#include "3D/Matrix/WorldTransform.h"
#include "2D/ImGuiManager.h"
#include "Components/Input.h"
#include "Utility/CollisionManager/Collider.h"
#include "PlayerBullet.h"
#include "UI.h"
#include "3D/Model/ParticleModel.h"
#include "Components/Particle/ParticleSystem.h"

class Player
{
public:

	~Player();

	void Initialize(PlayerBullet* bullet_);

	void Update();

	void Draw(const ViewProjection viewProjection);

	void ApplyGlobalVariables();

	void ModelMotion();

	void DrawSprite();

	void StartAnimation();

	void DrawParticle(const ViewProjection& viewProjection);

private:
	Input* input_ = nullptr;

	XINPUT_STATE joyState_;

	std::unique_ptr<Model> playerModel_ = nullptr;

	WorldTransform playerWorldTransform_;

	uint32_t textureHandle_ = 0u;

	PlayerBullet* bullet_;

	struct PlayerSt {
		Vector3 translation_; //Local座標
		Vector3 rotation_;//Local回転
		Vector3 scale_;//Localスケール
		Vector4 color_;//色
	};

	//自機の移動スピード
	float playerMoveSpeed_ = 0.1f;

	//モデルとモーション
	enum {
		Stay,
		Move,
	};

	int  motionMode_;
	Vector3 prePlayerTranslation_;

	PlayerSt player_;

	//基礎サイズ
	Vector3 NormalScale_;

	float UISpace = 16.0f;
	Vector2 UISpriteSize = { 131.0f * 2.0f,36.0f * 2.0f };

	UI RockOnUI_;

	//パーティクル
	std::unique_ptr<ParticleModel> particleModel_ = nullptr;
	std::unique_ptr<ParticleSystem> particleSystem_ = nullptr;
};



