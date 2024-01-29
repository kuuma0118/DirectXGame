#include "OverScene/OverScene.h"
#include "GameManager.h"
#include "GameScene.h"
#include "TitleScene/TitleScene.h"
#include "Engine/Base/TextureManager.h"
#include <cassert>

GameOverScene::GameOverScene() {};

GameOverScene::~GameOverScene() {
};

void GameOverScene::Initialize(GameManager* gameManager) {

	//TextureManagerのインスタンスを取得
	textureManager_ = TextureManager::GetInstance();
	//Inputのインスタンスを取得
	input_ = Input::GetInstance();
	//オーディオクラスのインスタンスを取得
	audio_ = Audio::GetInstance();
	//ポストプロセスのインスタンスを取得
	postProcess_ = PostProcess::GetInstance();

	overSoundHandle_ = audio_->SoundLoadWave("Resources/Sounds/over.wav");

	audio_->SoundPlayWave(overSoundHandle_, true);

	// カメラ
	worldTransform_.Initialize();
	viewProjection_.Initialize();
	viewProjection_.translation_ = { 0.0f,5.0f,-50.0f };
	worldTransform_.translation_.y = 5.0f;

	//タイトル
	overTextureHandle_ = TextureManager::Load("Resources/Pictures/over.png");
	overSprite_.reset(Sprite::Create(overTextureHandle_, { 0.0f,0.0f }));
	//スプライトの生成
	transitionSprite_.reset(Sprite::Create(transitionTextureHandle_, { 0.0f,0.0f }));
	transitionSprite_->SetColor(transitionColor_);
	transitionSprite_->SetSize(Vector2{ 640.0f,360.0f });

	// 当たり判定のインスタンスを生成
	collisionManager_ = new CollisionManager();
};

void GameOverScene::Update(GameManager* gameManager) {

	if (input_->IsPushKeyEnter(DIK_T))
	{
		if (isTransitionEnd_) {
			isTransition_ = true;
			if (soundCount_ == 0)
			{
				soundCount_ = 1;
				/*audio_->SoundPlayWave(SelectsoundHandle_, false);*/
			}
		}
	}

	if (isTransitionEnd_ == false) {
		transitionTimer_ += 1.0f / kTransitionTime;
		transitionColor_.w = Lerp(transitionColor_.w, 0.0f, transitionTimer_);
		transitionSprite_->SetColor(transitionColor_);

		if (transitionColor_.w <= 0.0f) {
			isTransitionEnd_ = true;
			transitionTimer_ = 0.0f;
		}
	}

	if (isTransition_) {
		transitionTimer_ += 1.0f / kTransitionTime;
		transitionColor_.w = Lerp(transitionColor_.w, 1.0f, transitionTimer_);
		transitionSprite_->SetColor(transitionColor_);

		if (transitionColor_.w >= 1.0f) {
			audio_->StopAudio(overSoundHandle_);
			gameManager->ChangeScene(new GameTitleScene);

		}
	}
};

void GameOverScene::Draw(GameManager* gameManager) {

#pragma region 背景スプライトの描画

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	Sprite::PostDraw();

#pragma endregion

	//深度バッファのクリア
	FCS::GetInstance()->ClearDepthBuffer();

#pragma region モデルの描画

	Model::PreDraw();

	Model::PostDraw();

#pragma endregion

#pragma region スプライトの描画

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	overSprite_->Draw();

	transitionSprite_->Draw();

	Sprite::PostDraw();

#pragma endregion
};