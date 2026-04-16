#include <DxLib.h>
#include "../Application.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "GameScene.h"

////デフォルトコンストラクタ
//GameScene::GameScene(void)
//{
//
//}
//
////デストラクタ
//GameScene::~GameScene(void)
//{
//
//}

//初期化処理
void GameScene::Init(void)
{
}

//更新処理
void GameScene::Update(void)
{
	auto& ins = InputManager::GetInstance();

	if (ins.IsTrgDown(KEY_INPUT_RETURN))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::OVER);
	}
	if (CheckHitKey(KEY_INPUT_SPACE) == 1)
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::CLEAR);
	}
}

//描画処理
void GameScene::Draw(void)
{
	DrawString(0, 0, "Scene : Game", 0xFFFFFF);

	DrawBox(50
		, 50
		, Application::SCREEN_SIZE_X - 50
		, Application::SCREEN_SIZE_Y - 50
		, 0xFFFF00
		, true);
}

//解放処理
void GameScene::Release(void)
{
}
