#include "GameScene.h"
#include "GameManager.h"

void GameScene::Initialize() {
	sceneNum = GAME_SCENE;
	// キー入力
	input_ = Input::GetInstance();

	// 敵の出現情報
	LoadEnemyPopData();

	// カメラ
	viewProjection_.Initialize();
	// 座標
	worldTransform_.Initialize();

#pragma region モデルの読み込み
	// 自機
	playerModel_ = Model::CreateModelFromObj(CUBE);
	// 敵
	enemyModel_ = Model::CreateModelFromObj(CUBE);
	// 天球
	skydomeModel_ = Model::CreateModelFromObj(SPHERE);

#pragma endregion

	// カメラレールの生成
	railCamera_ = new RailCamera();
	// カメラレールの初期化
	railCamera_->Initialize(worldTransform_, worldTransform_.rotation_);

	// 自キャラの生成
	player_ = new Player();

	// 初期位置
	Vector3 playerPosition(0, 0, 10);
	// 自キャラの初期化
	player_->Initialize(UVCHEKER, playerPosition);
	player_->SetGameScene(this);
	// 自キャラとレールカメラの親子関係を結ぶ
	player_->SetParent(&railCamera_->GetWorldTransform());

	// 天球
	skydome_ = new Skydome();
	skydome_->Initialize({ 0, 0, 0 });

	// 衝突マネージャーの生成
	collisionManager_ = new CollisionManager();

	// 倒した敵の数
	killCount_ = 0;

	UI_[0] = Sprite::Create(UI_PAD_RB);
	UI_[0]->SetPos(Vector2{ 50,50 });
	UI_[0]->SetSize(Vector2{ 50,30 });

	UI_[1] = Sprite::Create(UI_SHOT);
	UI_[1]->SetPos(Vector2{ 115,50 });

	endCount_ = 1000;
	isEnd_ = false;
}

void GameScene::Update() {
	// カメラ
	viewProjection_.UpdateMatrix();
	// 敵の出現するタイミングと座標
	UpdateEnemyPopCommands();

	// enemyの更新
	for (Enemy* enemy : enemy_) {
		enemy->Update();
	}
	// 敵の削除
	enemy_.remove_if([](Enemy* enemy) {
		if (enemy->IsDead()) {
			delete enemy;
			return true;
		}
		return false;
		});

	// 自キャラの更新
	player_->Update(viewProjection_);
	// 終了した弾を削除
	playerBullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
		});
	// 弾の更新
	for (PlayerBullet* bullet : playerBullets_) {
		bullet->Update();
	}

	// 天球
	skydome_->Update();


	// 当たり判定を必要とするObjectをまとめてセットする
	collisionManager_->SetGameObject(player_, enemy_, enemyBullets_, playerBullets_);
	// 衝突マネージャー(当たり判定)
	collisionManager_->CheckAllCollisions(this, player_);

	// デバッグカメラの更新
	railCamera_->Update();
	viewProjection_.matView = railCamera_->GetViewProjection().matView;
	viewProjection_.matProjection = railCamera_->GetViewProjection().matProjection;

	// ビュープロジェクション行列の転送
	viewProjection_.TransferMatrix();

	// 倒した敵のカウント
	for (Enemy* enemy : enemy_) {
		if (enemy->GetDeadSignal()) {
			killCount_++;
		}
	}

	if (killCount_ == 10) {
		sceneNum = GAMECLEAR_SCENE;
	}

	if (isEnd_) {
		endCount_--;
		if (endCount_ <= 0) {
			sceneNum = GAMEOVER_SCENE;
		}
	}
}

void GameScene::Draw() {

	// 天球
	skydome_->Draw(viewProjection_);
	// 自弾
	for (PlayerBullet* bullet : playerBullets_) {
		bullet->Draw(viewProjection_);
	}
	// 自機
	player_->Draw(viewProjection_);
	// 敵弾
	for (EnemyBullet* bullet : enemyBullets_) {
	}
	// 敵
	for (Enemy* enemy : enemy_) {
		enemy->Draw(viewProjection_);
	}

	// 2Dレティクル
	player_->DrawUI();

	// UI
	for (int i = 0; i < 2; i++) {
		UI_[i]->Draw();
	}
}

void GameScene::Finalize() {
	delete collisionManager_;
	enemyPopCommands_.seekg(0, std::ios::beg);
	enemyPopCommands_.clear();
	delete player_;
	enemy_.clear();
	playerBullets_.clear();
	delete skydome_;
	viewProjection_.constBuff_.ReleaseAndGetAddressOf();
}

void GameScene::SpawnEnemy(Vector3 pos) {
	Enemy* enemy = new Enemy();
	// 自機の位置をもらう
	enemy->SetPlayer(player_);
	// 初期化
	enemy->SetGameScene(this);
	enemy->Initialize(pos);

	// リストに登録
	enemy_.push_back(enemy);
}

void GameScene::LoadEnemyPopData() {
	// ファイルを開く
	std::ifstream file;
	file.open("engine/resources/csv/enemyPop.csv");
	assert(file.is_open());

	enemyPopCommands_ << file.rdbuf();

	// ファイルを閉じる
	file.close();
}

void GameScene::UpdateEnemyPopCommands() {
	// 待機処理
	if (isWait_) {
		waitTime_--;
		if (waitTime_ <= 0) {
			// 待機完了
			isWait_ = false;
		}
		return;
	}

	// 1桁分の文字列を入れる変数
	std::string line;

	while (getline(enemyPopCommands_, line)) {
		std::istringstream line_stream(line);

		std::string word;
		// ,区切りで行の先頭文字列を取得
		getline(line_stream, word, ',');

		// "//"から始まる行はコメント
		if (word.find("//") == 0) {
			// コメント行を飛ばす
			continue;
		}

		// POPコマンド
		if (word.find("POP") == 0) {
			// x座標
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());

			// y座標
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());

			// z座標
			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());

			// 敵を発生させる
			SpawnEnemy(Vector3(x, y, z));
		}
		
		// WAITコマンド
		else if (word.find("WAIT") == 0) {
			getline(line_stream, word, ',');

			// 待ち時間
			int32_t waitTime = atoi(word.c_str());

			// 待ち時間
			isWait_ = true;
			waitTime_ = waitTime;

			// コマンドループを抜ける
			break;
		}
		else if (word.find("END") == 0) {
			getline(line_stream, word, ',');
			isEnd_ = true;

			break;
		}
	}
}

void GameScene::AddPlayerBullet(PlayerBullet* playerBullet) {
	// リストに登録する
	playerBullets_.push_back(playerBullet);
}

void GameScene::AddEnemyBullet(EnemyBullet* enemyBullet) {
	// リストに登録する
	enemyBullets_.push_back(enemyBullet);
}