#include <DxLib.h>
#include "../Application.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "OverScene.h"

////デフォルトコンストラクタ
//OverScene::OverScene(void)
//{
//
//}
//
////デストラクタ
//OverScene::~OverScene(void)
//{
//
//}

//初期化処理
void OverScene::Init(void)
{
}

//更新処理
void OverScene::Update(void)
{
	auto& ins = InputManager::GetInstance();

	if (ins.IsTrgDown(KEY_INPUT_SPACE))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
	}
}

//描画処理
void OverScene::Draw(void)
{
	DrawString(0, 0, "Scene : GameOver", 0xFFFFFF);

	DrawBox(50
		, 50
		, Application::SCREEN_SIZE_X - 50
		, Application::SCREEN_SIZE_Y - 50
		, 0xFF00FF
		, true);
}

//解放処理
void OverScene::Release(void)
{
}
