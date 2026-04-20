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
	/*if (ins.IsTrgDown(KEY_INPUT_RETURN))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::QUEST);
	}*/

	//フェーズ選択の処理
	ProcessPhaseSelection();

	if (ins.IsTrgDown(KEY_INPUT_RETURN) || ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT))
	{
		switch (phase_) {
		case QUEST_PHASE::PHASE_QUEST:
			// シーンマネージャーにクエストシーンへの変更を命令
			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::QUEST);
			break;
		case QUEST_PHASE::PHASE_CLASSWORK:
			// 授業シーン、あるいは授業内処理へ
			break;
		case QUEST_PHASE::PHASE_JOB_CHANGE:
			// 資格試験シーンへ
			break;
		}
	}
}

//描画処理
void GameScene::Draw(void)
{
	DrawString(0, 0, "Scene : Game", 0xFFFFFF);

	int color = GetColor(255, 255, 255);
	int selectColor = GetColor(255, 255, 0); // 選択中は黄色にする

	DrawFormatString(100, 100, (phase_ == QUEST_PHASE::PHASE_QUEST ? selectColor : color), "クエストに出発");
	DrawFormatString(100, 140, (phase_ == QUEST_PHASE::PHASE_CLASSWORK ? selectColor : color), "授業を受ける");
	DrawFormatString(100, 180, (phase_ == QUEST_PHASE::PHASE_JOB_CHANGE ? selectColor : color), "資格試験");
}

//解放処理
void GameScene::Release(void)
{
}

void GameScene::ProcessPhaseSelection(void)
{
	//フェーズ選択の処理
	if (ins.IsTrgDown(KEY_INPUT_UP) ||
		ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DG_UP))
	{
		//statusType_ = (statusType_ + 3) % 4;
		int index = static_cast<int>(phase_);
		index = (index - 1 + static_cast<int>(QUEST_PHASE::MAX)) % static_cast<int>(QUEST_PHASE::MAX);
		phase_ = static_cast<QUEST_PHASE>(index);
	}
	if (ins.IsTrgDown(KEY_INPUT_DOWN) ||
		ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DG_DOWN))
	{
		int index = static_cast<int>(phase_);
		index = (index + 1) % static_cast<int>(QUEST_PHASE::MAX);
		phase_ = static_cast<QUEST_PHASE>(index);
	}

}
