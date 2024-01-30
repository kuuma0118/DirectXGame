#include "TitleScene.h"
#include "ImGuiManager.h"

void TitleScene::Initialize() {
	sceneNum = TITLE_SCENE;
	// 入力
	input_ = Input::GetInstance();

	UISprite_[0] = Sprite::Create(UI_PAD_A);
	UISprite_[0]->SetPos(Vector2{ (float)WinApp::kClientWidth_ / 2, (float)WinApp::kClientHeight_ / 2 + 100 });
	UISprite_[0]->SetSize(Vector2{ 32,32 });

	UISprite_[1] = Sprite::Create(UI_TITLE);
	UISprite_[1]->SetPos(Vector2{ (float)WinApp::kClientWidth_ / 2, (float)WinApp::kClientHeight_ / 2 - 150 });

	backGround_ = Sprite::Create(BACKGROUND);
	backGround_->SetPos(Vector2{ (float)WinApp::kClientWidth_ / 2, (float)WinApp::kClientHeight_ / 2 });
}

void TitleScene::Update() {
	XINPUT_STATE joyState;
	// ゲームパッド未接続なら何もせず抜ける
	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return;
	}
	if (!input_->GamePadTrigger(XINPUT_GAMEPAD_A)) {
		sceneNum = GAME_SCENE;
	}
}

void TitleScene::Draw() {
	backGround_->Draw();
	for (int i = 0; i < 2; i++) {
		UISprite_[i]->Draw();
	}
}

void TitleScene::Finalize() {
}