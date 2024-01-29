#include "Player.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "Utility/GlobalVariables.h"

Player::~Player()
{
	delete RockOnUI_.sprite_;
}

void Player::Initialize(PlayerBullet* bullet)
{

	playerModel_.reset(Model::CreateFromOBJ("Resources", "cube.obj"));

	bullet_ = bullet;

	textureHandle_ = TextureManager::Load("Resources/uvChecker.png");

	input_ = Input::GetInstance();

	//Player
	playerWorldTransform_.translation_.x = 5.0f;
	playerWorldTransform_.translation_.y = -5.0f;
	playerWorldTransform_.scale_ = { 1.0f,1.0f,1.0f };

	playerWorldTransform_.UpdateMatrix();

	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	//グループを追加
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "playerMoveSpeed", playerMoveSpeed_);


	//Motion
	player_ = {
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
		{1.0f,1.0f,1.0f},
		{1.0f,1.0f,1.0f,1.0f},
	};

	NormalScale_ = { 1.0f,1.0f,1.0f };

	ModelMotion();

	RockOnUI_ = {
			true,
			TextureManager::Load("Resources/Images/UI.png"),
			{ float(WinApp::GetInstance()->kClientWidth) - UISpace - UISpriteSize.x - 160,float(WinApp::GetInstance()->kClientHeight) - UISpace - UISpriteSize.y},
			0.0f,
			{1.0f,1.0f},
			nullptr,
	};

	RockOnUI_.sprite_ = Sprite::Create(RockOnUI_.textureHandle_, RockOnUI_.position_);

	//パーティクルの初期化
	particleModel_.reset(ParticleModel::CreateFromOBJ("Resources/Particle", "Particle.obj"));
	particleSystem_ = std::make_unique<ParticleSystem>();
	particleSystem_->Initialize();

	//エミッターの作成
	ParticleEmitter* particleEmitter = EmitterBuilder()
		.SetArea({ -0.5f,-0.5f,0.0f }, { 0.5f,-0.5f,0.0f })
		.SetAzimuth(0.0f, 0.0f)
		.SetColor({ 1.0f,1.0f,1.0f,0.5f }, { 1.0f,1.0f,1.0f,0.5f })
		.SetCount(10)
		.SetDeleteTime(60)
		.SetElevation(0.0f, 0.0f)
		.SetEmitterName("PlayerMove")
		.SetFrequency(0.1f)
		.SetLifeTime(0.4f, 0.4f)
		.SetParticleType(ParticleEmitter::ParticleType::kScale)
		.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
		.SetScale({ 0.2f,0.2f,0.2f }, { 0.2f,0.2f,0.2f })
		.SetTranslation(playerWorldTransform_.translation_)
		.SetVelocity({ 0.1f,0.1f,0.1f }, { 0.1f,0.1f,0.1f })
		.Build();
	particleSystem_->AddParticleEmitter(particleEmitter);
}

void Player::Update()
{
	//エミッターが消えていたら再生成
	if (particleSystem_->GetParticleEmitter("PlayerMove") == nullptr) {
		//エミッターの作成
		ParticleEmitter* particleEmitter = EmitterBuilder()
			.SetArea({ -0.5f,-0.5f,0.0f }, { 0.5f,-0.5f,0.0f })
			.SetAzimuth(180.0f, 180.0f)
			.SetColor({ 1.0f,1.0f,1.0f,0.5f }, { 1.0f,1.0f,1.0f,0.5f })
			.SetCount(10)
			.SetDeleteTime(60)
			.SetElevation(0.0f, 0.0f)
			.SetEmitterName("PlayerMove")
			.SetFrequency(0.1f)
			.SetLifeTime(0.4f, 0.4f)
			.SetParticleType(ParticleEmitter::ParticleType::kScale)
			.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetScale({ 0.2f,0.2f,0.2f }, { 0.2f,0.2f,0.2f })
			.SetTranslation(playerWorldTransform_.translation_)
			.SetVelocity({ 0.1f,0.1f,0.1f }, { 0.1f,0.1f,0.1f })
			.Build();
		particleSystem_->AddParticleEmitter(particleEmitter);
	}

	//パーティクルを出さないようにする
	particleSystem_->GetParticleEmitter("PlayerMove")->SetPopCount(0);


	//プレイヤーの移動
	if (Input::GetInstance()->GetJoystickState(joyState_))
	{
		const float deadZone = 0.5f;

		bool isMoving = false;

		Vector3 move = { (float)joyState_.Gamepad.sThumbLX / SHRT_MAX, 0.0f,0.0f };

		if (Length(move) > deadZone)
		{
			isMoving = true;
		}

		if (isMoving)
		{
			move = Multiply(playerMoveSpeed_, Normalize(move));

			playerWorldTransform_.translation_ = Add(playerWorldTransform_.translation_, move);

			//移動中はパーティクルを出す
			if (move.x > 0.0f) {
				particleSystem_->GetParticleEmitter("PlayerMove")->SetPopArea({ -1.0f,-0.5f,0.0f }, { -1.0f,-0.5f,0.0f });
				particleSystem_->GetParticleEmitter("PlayerMove")->SetPopAzimuth(210.0f, 210.0f);
				particleSystem_->GetParticleEmitter("PlayerMove")->SetPopCount(10);
				particleSystem_->GetParticleEmitter("PlayerMove")->SetTranslation(playerWorldTransform_.translation_);
			}
			else if (move.x <= 0.0f) {
				particleSystem_->GetParticleEmitter("PlayerMove")->SetPopArea({ 1.0f,-0.5f,0.0f }, { 1.0f,-0.5f,0.0f });
				particleSystem_->GetParticleEmitter("PlayerMove")->SetPopAzimuth(0.0f, 0.0f);
				particleSystem_->GetParticleEmitter("PlayerMove")->SetPopCount(10);
				particleSystem_->GetParticleEmitter("PlayerMove")->SetTranslation(playerWorldTransform_.translation_);
			}

		}
		ModelMotion();
	}


	playerWorldTransform_.UpdateMatrix();
	prePlayerTranslation_ = playerWorldTransform_.translation_;
	Player::ApplyGlobalVariables();

	//パーティクルの更新
	particleSystem_->Update();
}

void Player::Draw(const ViewProjection viewProjection)
{
	playerModel_->Draw(playerWorldTransform_, viewProjection);
}

void Player::ApplyGlobalVariables()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	playerMoveSpeed_ = globalVariables->GetFloatValue(groupName, "playerMoveSpeed");
}

void Player::DrawSprite()
{
	RockOnUI_.sprite_->Draw();
}

void Player::StartAnimation() {
	//更新
	playerWorldTransform_.UpdateMatrix();
	ModelMotion();
	prePlayerTranslation_ = playerWorldTransform_.translation_;
	Player::ApplyGlobalVariables();
}

void Player::DrawParticle(const ViewProjection& viewProjection) {
	//パーティクルモデルの描画
	particleModel_->Draw(particleSystem_.get(), viewProjection);
}
