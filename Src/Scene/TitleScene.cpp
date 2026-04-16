#include <DxLib.h>
#include "../Application.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "TitleScene.h"

////デフォルトコンストラクタ
//SceneTitle::SceneTitle(void)
//{
//	titleImage_ = 0;
//}

////デストラクタ
//SceneTitle::~SceneTitle(void)
//{
//
//}

//初期化処理
void TitleScene::Init(void)
{
	titleImage_ = -1;

}

//更新処理
void TitleScene::Update(void)
{
	auto& ins = InputManager::GetInstance();

	if (ins.IsTrgDown(KEY_INPUT_RETURN))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
	}
}

//描画処理
void TitleScene::Draw(void)
{
	DrawString(0, 0, "Scene : Title", 0xFFFFFF);

	DrawBox(50
		, 50
		, Application::SCREEN_SIZE_X - 50
		, Application::SCREEN_SIZE_Y - 50
		, 0x00FFFF
		, true);
}

//解放処理
void TitleScene::Release(void)
{
}
