#include "ClearScene/ClearScene.h"
#include "GameManager.h"
#include "GameScene.h"
#include "TitleScene/TitleScene.h"
#include "Engine/Base/TextureManager.h"
#include <cassert>

GameClearScene::GameClearScene() {};

GameClearScene::~GameClearScene() {

};

void GameClearScene::Initialize(GameManager* gameManager) {

	//TextureManagerのインスタンスを取得
	textureManager_ = TextureManager::GetInstance();
	//Inputのインスタンスを取得
	input_ = Input::GetInstance();
	//オーディオクラスのインスタンスを取得
	audio_ = Audio::GetInstance();
	//ポストプロセスのインスタンスを取得
	postProcess_ = PostProcess::GetInstance();

	clearSoundHandle_ = audio_->SoundLoadWave("Resources/Sounds/clear.wav");

	audio_->SoundPlayWave(clearSoundHandle_, true);

	// カメラ
	worldTransform_.Initialize();
	viewProjection_.Initialize();
	viewProjection_.translation_ = { 0.0f,5.0f,-50.0f };
	worldTransform_.translation_.y = 5.0f;

	//タイトル
	clearTextureHandle_ = TextureManager::Load("Resources/Pictures/clear.png");
	clearSprite_.reset(Sprite::Create(clearTextureHandle_, { 0.0f,0.0f }));
	//スプライトの生成
	transitionSprite_.reset(Sprite::Create(transitionTextureHandle_, { 0.0f,0.0f }));
	transitionSprite_->SetColor(transitionColor_);
	transitionSprite_->SetSize(Vector2{ 640.0f,360.0f });

	// 当たり判定のインスタンスを生成
	collisionManager_ = new CollisionManager();
};

void GameClearScene::Update(GameManager* gameManager) {

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
			audio_->StopAudio(clearSoundHandle_);
			gameManager->ChangeScene(new GameTitleScene);

		}
	}
};

void GameClearScene::Draw(GameManager* gameManager) {

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

	clearSprite_->Draw();

	transitionSprite_->Draw();

	Sprite::PostDraw();

#pragma endregion
};