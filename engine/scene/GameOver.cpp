#include "GameOver.h"
#include "GameManager.h"

void GameOver::Initialize() {
	input_ = Input::GetInstance();

	UI_[0] = Sprite::Create(UI_PAD_A);
	UI_[0]->SetPos(Vector2{ (float)WinApp::kClientWidth_ / 2, (float)WinApp::kClientHeight_ / 2 + 100 });
	UI_[0]->SetSize(Vector2{ 32,32 });

	UI_[1] = Sprite::Create(UI_GAMEOVER);
	UI_[1]->SetPos(Vector2{ (float)WinApp::kClientWidth_ / 2, (float)WinApp::kClientHeight_ / 2 - 150 });

	backGround_ = Sprite::Create(BACKGROUND);
	backGround_->SetPos(Vector2{ (float)WinApp::kClientWidth_ / 2, (float)WinApp::kClientHeight_ / 2 });
}

void GameOver::Update() {
	XINPUT_STATE joyState;
	// ゲームパッド未接続なら何もせず抜ける
	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return;
	}
	if (!input_->GamePadTrigger(XINPUT_GAMEPAD_A)) {
		sceneNum = TITLE_SCENE;
	}
}

void GameOver::Draw() {
	backGround_->Draw();
	for (int i = 0; i < 2; i++) {
		UI_[i]->Draw();
	}
}

void GameOver::Finalize() {
	delete backGround_;
	for (int i = 0; i < 2; i++) {
		delete UI_[i];
	}
}