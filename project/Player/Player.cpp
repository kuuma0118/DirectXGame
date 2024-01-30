#include "Player.h"
#include "CollisionConfig.h"
#include "WorldTransform.h"
#include "ImGuiManager.h"
#include "MathStructs.h"
#include "GameScene.h"
#include "Sprite.h"
#include <cassert>

Player::Player() {}
Player::~Player() {
	delete sprite2DReticle_;
}

// Initializeの関数定義
void Player::Initialize(uint32_t textureHandle, const Vector3& pos) {
	// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();

	// objモデルを割り当て
	model_ = Model::CreateModelFromObj(CUBE);
	// NULLポインタチェック
	assert(model_);
	
	/// テクスチャ
	// 自機
	playerTexture_ = textureHandle;
	// レティクル
	sprite2DReticle_ = Sprite::Create(RETICLE);

	// 衝突属性を設定
	SetCollisionAttribute(kCollisionAttributePlayer);
	// 衝突対象を自分の属性以外に設定
	SetCollisionMask(~kCollisionAttributePlayer);

	// ワールド変換の初期化
	model_->worldTransform.Initialize();
	// 3Dレティクルのワールドトランスフォーム初期化
	worldTransform3DReticle_.Initialize();

	model_->worldTransform.translation_ = pos;

	// スプライトの現在座標を取得
	spritePosition_ = sprite2DReticle_->GetPos();
}

// Updateの関数定義
void Player::Update(const ViewProjection& viewProjection) {
	// キャラクターの移動ベクトル
	Vector3 move = { 0, 0, 0 };

	// キャラクターの移動の速さ
	const float kCharacterSpeed = 0.2f;

#pragma region Move

	XINPUT_STATE joyState;
	// ゲームパッド状態取得
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		// デッドゾーンの設定
		SHORT leftThumbX = Input::GetInstance()->ApplyDeadzone(joyState.Gamepad.sThumbLX);
		SHORT leftThumbY = Input::GetInstance()->ApplyDeadzone(joyState.Gamepad.sThumbLY);
		move.x += (float)leftThumbX / SHRT_MAX * kCharacterSpeed;
		move.y += (float)leftThumbY / SHRT_MAX * kCharacterSpeed;
	}

	// 移動限界座標
	const Vector2 kMoveLimit = { 40 - 10, 30 - 15 };

	// 範囲を超えない処理
	model_->worldTransform.translation_.x = max(model_->worldTransform.translation_.x, -kMoveLimit.x);
	model_->worldTransform.translation_.x = min(model_->worldTransform.translation_.x, kMoveLimit.x);
	model_->worldTransform.translation_.y = max(model_->worldTransform.translation_.y, -kMoveLimit.y);
	model_->worldTransform.translation_.y = min(model_->worldTransform.translation_.y, kMoveLimit.y);

	// 座標移動
	model_->worldTransform.translation_.x += move.x;
	model_->worldTransform.translation_.y += move.y;
	model_->worldTransform.translation_.z += move.z;

#pragma endregion

#pragma region Rotate

	// 旋回処理
	Rotate();

	// アフィン変換行列をワールド行列に代入
	model_->worldTransform.matWorld_ = MakeAffineMatrix(
		model_->worldTransform.scale_, model_->worldTransform.rotation_, model_->worldTransform.translation_);

#pragma endregion

	model_->worldTransform.UpdateMatrix();
	// 弾の処理
	Attack();
	// 3Dレティクルの配置
	Deploy3DReticle();

	// 2Dレティクルの配置
	Deploy2DReticle(viewProjection);

	Vector2 joyRange{};
	// ジョイスティック状態取得
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		// デッドゾーンの設定
		SHORT rightThumbX = Input::GetInstance()->ApplyDeadzone(joyState.Gamepad.sThumbRX);
		SHORT rightThumbY = Input::GetInstance()->ApplyDeadzone(joyState.Gamepad.sThumbRY);
		joyRange.x += (float)rightThumbX / SHRT_MAX * 10.0f;
		joyRange.y += (float)rightThumbY / SHRT_MAX * 10.0f;
		spritePosition_.x += joyRange.x;
		spritePosition_.y -= joyRange.y;
		// スプライトへの座標変更を反映
		sprite2DReticle_->SetPos(spritePosition_);
	}

	// ビューポート行列
	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, (float)WinApp::kClientWidth_, (float)WinApp::kClientHeight_, 0, 1);
	// ビュープロジェクションビューポート合成行列
	Matrix4x4 matVPV = Multiply(viewProjection.matView, Multiply(viewProjection.matProjection, matViewport));
	// 合成行列の逆行列を計算する
	Matrix4x4 matInverseVPV = Inverse(matVPV);

	// スクリーン座標
	Vector3 posNear = Vector3((float)sprite2DReticle_->GetPos().x, (float)sprite2DReticle_->GetPos().y - 16, 0);
	Vector3 posFar = Vector3((float)sprite2DReticle_->GetPos().x, (float)sprite2DReticle_->GetPos().y - 16, 1);

	// スクリーン座標系からワールド座標系へ
	posNear = Transforms(posNear, matInverseVPV);
	posFar = Transforms(posFar, matInverseVPV);
	// マウスレイの方向
	Vector3 mouseDirection = Subtract(posFar, posNear);
	mouseDirection = Normalize(mouseDirection);
	// カメラから照準オブジェクトの距離
	const float kDistanceTestObject = 1000.0f;
	// 3Dレティクルを2Dカーソルに配置
	worldTransform3DReticle_.translation_.x = posNear.x - mouseDirection.x * kDistanceTestObject;
	worldTransform3DReticle_.translation_.y = posNear.y - mouseDirection.y * kDistanceTestObject;
	worldTransform3DReticle_.translation_.z = posNear.z - mouseDirection.z * kDistanceTestObject;

	worldTransform3DReticle_.UpdateMatrix();
}

// Drawの関数定義
void Player::Draw(ViewProjection& viewProjection) {
	// player
	model_->Draw(viewProjection, playerTexture_);
	// 3Dレティクルを描画
	model_->Draw(viewProjection, UVCHEKER);
}

void Player::DrawUI() {
	sprite2DReticle_->Draw();
}

// playerの回転
void Player::Rotate() {
	// 回転速さ[ラジアン/frame]
	const float kRotSpeed = 0.02f;

	// 押した方向で移動ベクトルを変更
	if (input_->PressKey(DIK_A)) {
		model_->worldTransform.rotation_.y -= kRotSpeed;
	}
	else if (input_->PressKey(DIK_D)) {
		model_->worldTransform.rotation_.y += kRotSpeed;
	}
}

// 攻撃
void Player::Attack() {
	XINPUT_STATE joyState;
	// ゲームパッド未接続なら何もせず抜ける
	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return;
	}

	// Rトリガーを押していたら
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER || input_->TriggerKey(DIK_SPACE)) {
		// 弾の速度
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, kBulletSpeed);
		// 速度ベクトルを自機の向きに合わせて回転させる
		Vector3 worldPos = GetWorldPosition();
		velocity = TransformNormal(velocity, model_->worldTransform.matWorld_);
		// 自機から照準オブジェクトへのベクトル
		Vector3 worldReticlePos = {
			worldTransform3DReticle_.matWorld_.m[3][0], worldTransform3DReticle_.matWorld_.m[3][1],
			worldTransform3DReticle_.matWorld_.m[3][2] };
		velocity = Subtract(worldReticlePos, worldPos);
		velocity = Normalize(velocity);
		velocity.x *= kBulletSpeed;
		velocity.y *= kBulletSpeed;
		velocity.z *= kBulletSpeed;

		// 弾を生成し、初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(worldPos, velocity);

		// 弾を登録
		gameScene_->AddPlayerBullet(newBullet);
	}
}

void Player::Deploy3DReticle() {
	// 自機から3Dレティクルへの距離
	const float kDistancePlayerTo3DReticle = 50.0f;
	// 自機から3Dレティクルへのオフセット(Z+向き)
	Vector3 offset{ 0, 0, 1.0f };
	// 自機のワールド行列の回転を反映する
	offset = TransformNormal(offset, model_->worldTransform.matWorld_);
	offset = Normalize(offset);
	// ベクトルの長さを整える
	offset.x *= kDistancePlayerTo3DReticle;
	offset.y *= kDistancePlayerTo3DReticle;
	offset.z *= kDistancePlayerTo3DReticle;

	// 3Dレティクルの座標を設定
	worldTransform3DReticle_.translation_.x = GetWorldPosition().x + offset.x;
	worldTransform3DReticle_.translation_.y = GetWorldPosition().y + offset.y;
	worldTransform3DReticle_.translation_.z = GetWorldPosition().z + offset.z;

	worldTransform3DReticle_.UpdateMatrix();
}

void Player::Deploy2DReticle(const ViewProjection& viewProjection) {
	// 3Dレティクルのワールド座標を取得
	Vector3 positionReticle = GetWorld3DReticlePosition();
	// ビューポート行列
	matViewport_ =
		MakeViewportMatrix(0, 0, (float)WinApp::kClientWidth_, (float)WinApp::kClientHeight_, 0, 1);
	// ビュー行列とプロジェクション行列、ビューポート行列を合成する
	Matrix4x4 matViewProjectionViewport{};
	matViewProjectionViewport =
		Multiply(viewProjection.matView, Multiply(viewProjection.matProjection, matViewport_));
	// ワールド→スクリーン座標変換
	positionReticle = Transforms(positionReticle, matViewProjectionViewport);
	// スプライトのレティクルに座標設定
	sprite2DReticle_->SetPos(Vector2(positionReticle.x, positionReticle.y));
}

void Player::OnCollision() {}

Vector3 Player::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos{};
	// ワールド行列の平行移動成分を取得
	worldPos.x = model_->worldTransform.matWorld_.m[3][0];
	worldPos.y = model_->worldTransform.matWorld_.m[3][1];
	worldPos.z = model_->worldTransform.matWorld_.m[3][2];

	return worldPos;
}

Vector3 Player::GetWorld3DReticlePosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得
	worldPos.x = worldTransform3DReticle_.matWorld_.m[3][0];
	worldPos.y = worldTransform3DReticle_.matWorld_.m[3][1];
	worldPos.z = worldTransform3DReticle_.matWorld_.m[3][2];

	return worldPos;
}

void Player::SetParent(const WorldTransform* parent) {
	// 親子関係を結ぶ
	model_->worldTransform.parent_ = parent;
}
