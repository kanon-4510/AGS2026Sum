#include <chrono>
#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "../Common/Fader.h"
#include "../Scene/TitleScene.h"
#include "../Scene/GameScene.h"
#include "../Scene/OverScene.h"
#include "../Scene/ClearScene.h"
#include "ResourceManager.h"
#include "SceneManager.h"

SceneManager* SceneManager::instance_ = nullptr;

void SceneManager::CreateInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new SceneManager();
	}
	instance_->Init();
}

SceneManager& SceneManager::GetInstance(void)
{
	return *instance_;
}

void SceneManager::Init(void)
{
	sceneId_ = SCENE_ID::TITLE;
	waitSceneId_ = SCENE_ID::NONE;

	fader_ = std::make_unique<Fader>();
	fader_->Init();

	isSceneChanging_ = false;

	// デルタタイム
	preTime_ = std::chrono::system_clock::now();

	// 初期シーンの設定
	DoChangeScene(SCENE_ID::TITLE);

}

void SceneManager::Update(void)
{
	if (scene_ == nullptr)
	{
		return;
	}

	// デルタタイム
	auto nowTime = std::chrono::system_clock::now();
	deltaTime_ = static_cast<float>(
		std::chrono::duration_cast<std::chrono::nanoseconds>(nowTime - preTime_).count() / 1000000000.0);
	preTime_ = nowTime;

	fader_->Update();
	if (isSceneChanging_)
	{
		Fade();
	}
	else
	{
		scene_->Update();
	}
}

void SceneManager::Draw(void)
{

	// 描画先グラフィック領域の指定
	// (３Ｄ描画で使用するカメラの設定などがリセットされる)
	SetDrawScreen(DX_SCREEN_BACK);

	// 画面を初期化
	ClearDrawScreen();

	// Effekseerにより再生中のエフェクトを更新する。
	UpdateEffekseer3D();

	// 描画
	scene_->Draw();

	// Effekseerにより再生中のエフェクトを描画する。
	DrawEffekseer3D();

	// 暗転・明転
	fader_->Draw();

}

void SceneManager::Destroy(void)
{
	delete instance_;
}

void SceneManager::ChangeScene(SCENE_ID nextId)
{

	// フェード処理が終わってからシーンを変える場合もあるため、
	// 遷移先シーンをメンバ変数に保持
	waitSceneId_ = nextId;

	// フェードアウト(暗転)を開始する
	fader_->SetFade(Fader::STATE::FADE_OUT);
	isSceneChanging_ = true;

}

SceneManager::SCENE_ID SceneManager::GetSceneID(void)
{
	return sceneId_;
}

float SceneManager::GetDeltaTime(void) const
{
	//return 1.0f / 60.0f;
	return deltaTime_;
}

SceneManager::SceneManager(void)
{
	sceneId_ = SCENE_ID::NONE;
	waitSceneId_ = SCENE_ID::NONE;

	scene_ = nullptr;
	fader_ = nullptr;

	isSceneChanging_ = false;

	// デルタタイム
	deltaTime_ = 1.0f / 60.0f;

}

void SceneManager::ResetDeltaTime(void)
{
	deltaTime_ = 0.016f;
	preTime_ = std::chrono::system_clock::now();
}

void SceneManager::DoChangeScene(SCENE_ID sceneId)
{
	// リソースの解放
	ResourceManager::GetInstance().Release();

	// 現在のシーンの終了処理を呼ぶ
	if (scene_ != nullptr)
	{
		scene_->Release();
		scene_.reset();
	}

	// シーンを変更する
	sceneId_ = sceneId;

	switch (sceneId_)
	{
	case SCENE_ID::TITLE:
		scene_ = std::make_unique<TitleScene>();
		break;
	case SCENE_ID::GAME:
		scene_ = std::make_unique<GameScene>();
		break;
	case SCENE_ID::OVER:
		scene_ = std::make_unique<OverScene>();
		break;
	case SCENE_ID::CLEAR:
		scene_ = std::make_unique<ClearScene>();
		break;
	}

	scene_->Init();

	ResetDeltaTime();

	waitSceneId_ = SCENE_ID::NONE;
}

void SceneManager::Fade(void)
{

	Fader::STATE fState = fader_->GetState();
	switch (fState)
	{
	case Fader::STATE::FADE_IN:
		if (fader_->IsEnd())
		{
			fader_->SetFade(Fader::STATE::NONE);
			isSceneChanging_ = false;
		}
		break;

	case Fader::STATE::FADE_OUT:
		if (fader_->IsEnd())
		{
			if (!isLoading_)
			{
				// ローディング表示期間に入る
				isLoading_ = true;
				loadingTimer_ = 0;
			}
			else
			{
				loadingTimer_++;

				if (loadingTimer_ > 60) // 約1秒間ローディングを見せる
				{
					DoChangeScene(waitSceneId_);
					fader_->SetFade(Fader::STATE::FADE_IN);
					isLoading_ = false;
				}
			}
		}
		break;
	}
}


