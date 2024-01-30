#pragma once
#include "IScene.h"
#include "Input.h"
#include "Sprite.h"

class GameManager;
class GameClear : public IScene
{
public:
	// 初期化
	void Initialize()override;

	// 更新処理
	void Update()override;

	// 描画
	void Draw()override;

	void Finalize()override;
private:
	Input* input_;

	// UI
	Sprite* UISprite_[2];
	// 背景
	Sprite* backGround_;
};

