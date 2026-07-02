#include <DxLib.h>
#include "../Application.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Common/Color.h"
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

	ProcessTitleDecision();
	ProcessTitleSelection();
}

//描画処理
void TitleScene::Draw(void)
{
	DrawString(0, 0, "Scene : Title", 0xFFFFFF);

	if(titleSelection_ == START_GAME)
	{
		DrawFormatString(80, 100, Color::WHITE, "→");
	}
	else
	{
		DrawFormatString(80, 140, Color::WHITE, "→");
	}

	DrawFormatString(100, 100, Color::WHITE, "ゲーム");
	if (SceneManager::GetInstance().IsTutorialEnabled()) {
		DrawString(100, 140, "チュートリアル: 【 ON 】", Color::WHITE);
	}
	else {
		DrawString(100, 140, "チュートリアル: 【 OFF 】", Color::WHITE);
	}
}

//解放処理
void TitleScene::Release(void)
{
}

void TitleScene::ProcessTitleSelection(void)
{
	if (ins_.IsTrgDown(KEY_INPUT_UP) ||
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DG_UP))
	{
		int index = static_cast<int>(titleSelection_);
		index = (index + (static_cast<int>(TITLE_SELECTION::MAX) - 1)) % static_cast<int>(TITLE_SELECTION::MAX);
		titleSelection_ = static_cast<TITLE_SELECTION>(index);
	}
	else if (ins_.IsTrgDown(KEY_INPUT_DOWN) ||
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DG_DOWN))
	{
		int index = static_cast<int>(titleSelection_);
		index = (index + 1) % static_cast<int>(TITLE_SELECTION::MAX);
		titleSelection_ = static_cast<TITLE_SELECTION>(index);
	}
}

void TitleScene::ProcessTitleDecision(void)
{
	if(ins_.IsTrgDown(KEY_INPUT_RETURN) ||
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT))
	{
		switch (titleSelection_)
		{
		case TitleScene::START_GAME:
			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
			break;
		case TitleScene::TUTORIAL:
			SceneManager::GetInstance().ToggleTutorial();
			break;
		case TitleScene::MAX:
			break;
		default:
			break;
		}
	}
}
