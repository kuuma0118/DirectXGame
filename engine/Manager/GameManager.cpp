#include "GameManager.h"
#include "ConvertString.h"
#include "GlobalVariables.h"

GameManager::GameManager() {
	// 各シーンの配列
	sceneArr_[TITLE_SCENE] = new TitleScene();
	sceneArr_[GAME_SCENE] = new GameScene();
	sceneArr_[GAMECLEAR_SCENE] = new GameClear();
	sceneArr_[GAMEOVER_SCENE] = new GameOver();
}

void GameManager::Initialize() {
	// タイトル名を入力
	const char kWindowTitle[] = "LE2B_23__Remake";
	// タイトルバーの変換
	auto&& titleString = ConvertString(kWindowTitle);

	/// エンジン機能の生成
	// windowの初期化
	winApp_ = WinApp::GetInstance();
	winApp_->Initialize(titleString.c_str(), 1280, 720);
	// DirectXの初期化
	directXCommon_ = DirectXCommon::GetInstance();
	directXCommon_->DirectXCommon::GetInstance()->Initialize(winApp_->GetHwnd());
	// Textureの初期化
	textureManager_ = TextureManager::GetInstance();
	textureManager_->TextureManager::GetInstance()->Initialize();
	// objモデルのマネージャの読み込み
	modelManager_ = ModelManager::GetInstance();
	modelManager_->Initialize();
	// PSOの初期化
	pipelineManager_ = PipelineManager::GetInstance();
	pipelineManager_->Initialize();
	// ImGuiの初期化
	imGuiManager_ = new ImGuiManager();
	imGuiManager_->Initialize(winApp_->GetHwnd());
	// ブローバル変数の読み込み
	//GlobalVariables::GetInstance()->LoadFiles();
	// ポストエフェクト
	//postEffect_ = new PostEffect();
	//postEffect_->Initialize();

	/// Components
	// 入力(キーボードとゲームパッド)
	input_ = Input::GetInstance();
	input_->Initialize();
	// ライトの設定
	directionalLight_ = DirectionalLight::GetInstance();
	directionalLight_->Initialize();
	// 点光源
	pointLight_ = PointLight::GetInstance();
	pointLight_->Initialize();
	// スポットライト
	spotLight_ = SpotLight::GetInstance();
	spotLight_->Initialize();
	//// Audioの初期化
	//audio_ = Audio::GetInstance();
	//HRESULT result;
	//// Xaudio2エンジンのインスタンスを生成
	//result = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	//// マスターボイスを生成
	//result = xAudio2_->CreateMasteringVoice(&masterVoice_);
	//// 音声読み込み
	//soundData1_ = audio_->SoundLoadWave("resources/fanfare.wav");
	//// 音声再生
	//audio_->SoundPlayWave(xAudio2_.Get(), soundData1_);

	//初期シーンの設定
	sceneNum_ = TITLE_SCENE;
	// シーンごとの初期化
	sceneArr_[sceneNum_]->Initialize();
}

void GameManager::Run() {
	// 初期化
	Initialize();

	MSG msg{};
	// ウィンドウの×ボタンが押されるまでループ
	while (msg.message != WM_QUIT) {
		// Windowにメッセージが来ていたら最優先で処理させる
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			// 描画前の処理
			BeginFrame();

			// シーンチェック
			preSceneNum_ = sceneNum_;
			sceneNum_ = sceneArr_[sceneNum_]->GetSceneNum();

			//シーン変更チェック
			if (sceneNum_ != preSceneNum_) {
				sceneArr_[preSceneNum_]->Finalize();
				sceneArr_[sceneNum_]->Initialize();				
			}

			///
			/// 更新処理
			/// 
			sceneArr_[sceneNum_]->Update();

			// ImGuiのパラメータを入れている
			//ImGuiAdjustParameter();

			// FPSカウンターの表示
		/*	ImGui::Begin("Control panel");
			ImGui::Text("Frame rate: %6.2f fps", ImGui::GetIO().Framerate);
			ImGui::End();*/

			///
			/// 描画処理
			/// 
			//postEffect_->PreDrawScene();		
			sceneArr_[sceneNum_]->Draw();
			//postEffect_->Draw();
			//postEffect_->PostDrawScene();
			
			// 描画後の処理
			EndFrame();
		}
	}
	// 解放処理
	Finalize();
}

void GameManager::BeginFrame() {
	// キーの状態を取得
	input_->Update();
	// DirectXCommon
	directXCommon_->PreDraw();
	// PSO
	pipelineManager_->PreDraw();
	// ImGui
	imGuiManager_->PreDraw();
	// グローバル変数の更新
	//GlobalVariables::GetInstance()->Update();
}

void GameManager::EndFrame() {
	// ImGui
	imGuiManager_->PostDraw();
	// DirectXCommon
	directXCommon_->PostDraw();
	// PSO
	pipelineManager_->PostDraw();
}

void GameManager::Finalize() {
	sceneArr_[sceneNum_]->Finalize();
	for (int i = 0; i < 4; i++) {
		delete sceneArr_[i];
	}
	// ImGui
	imGuiManager_->Finalize();
	textureManager_->Finalize();
	directXCommon_->Finalize();
	CloseWindow(winApp_->GetHwnd());
	xAudio2_.Reset();
	audio_->SoundUnload(&soundData1_);
	// Textureのゲーム終了処理
	textureManager_->ComUninit();
}

void GameManager::ImGuiAdjustParameter() {
	ImGui::Begin("CommonSettings");
	if (ImGui::BeginTabBar("CommonTabBar"))
	{
		// ライトのImGui
		// 平行光源
		if (ImGui::BeginTabItem("Directional Light")) {
			directionalLight_->ImGuiAdjustParameter();
			ImGui::EndTabItem();
		}
		// 点光源
		if (ImGui::BeginTabItem("Point Light")) {
			pointLight_->ImGuiAdjustParameter();
			ImGui::EndTabItem();
		}
		// スポットライト
		if (ImGui::BeginTabItem("Spot Light")) {
			spotLight_->ImGuiAdjustParameter();
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::End();
}