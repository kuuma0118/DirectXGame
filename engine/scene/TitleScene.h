#pragma once
#include "IScene.h"
#include "Input.h"
#include "Sprite.h"

class GameManager;

class TitleScene : public IScene
{
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize()override;
private:
	// 入力
	Input* input_;

	// UI
	Sprite* UISprite_[2];
	// 背景
	Sprite* backGround_;
};