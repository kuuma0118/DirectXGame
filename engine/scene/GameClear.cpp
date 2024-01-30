#include "GameClear.h"
#include "GameManager.h"

void GameClear::Initialize() {
	input_ = Input::GetInstance();

	UISprite_[0] = Sprite::Create(UI_PAD_A);
	UISprite_[0]->SetPos(Vector2{ (float)WinApp::kClientWidth_ / 2, (float)WinApp::kClientHeight_ / 2 + 100 });
	UISprite_[0]->SetSize(Vector2{ 32,32 });

	UISprite_[1] = Sprite::Create(UI_CLEAR);
	UISprite_[1]->SetPos(Vector2{ (float)WinApp::kClientWidth_ / 2, (float)WinApp::kClientHeight_ / 2 - 150 });

	backGround_ = Sprite::Create(BACKGROUND);
	backGround_->SetPos(Vector2{ (float)WinApp::kClientWidth_ / 2, (float)WinApp::kClientHeight_ / 2 });
}

void GameClear::Update() {
	XINPUT_STATE joyState;
	// ゲームパッド未接続なら何もせず抜ける
	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return;
	}
	if (!input_->GamePadTrigger(XINPUT_GAMEPAD_A)) {
		sceneNum = TITLE_SCENE;
	}
}

void GameClear::Draw() {
	backGround_->Draw();
	for (int i = 0; i < 2; i++) {
		UISprite_[i]->Draw();
	}
}

void GameClear::Finalize() {
	delete backGround_;
	for (int i = 0; i < 2; i++) {
		delete UISprite_[i];
	}
}