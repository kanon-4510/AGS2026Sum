#include <DxLib.h>
#include "../Application.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "Phase/QuestPhase.h"
#include "Phase/ClassWorkPhase.h"
#include "Phase/JobChangePhase.h"
#include "GameScene.h"

//デフォルトコンストラクタ
GameScene::GameScene(void)
{
		playerStatus_ = new PlayerStatus();
}

//デストラクタ
GameScene::~GameScene(void)
{
	delete playerStatus_;
	playerStatus_ = nullptr;
}

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

	if (ins_.IsTrgDown(KEY_INPUT_RETURN) || ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT))
	{
		switch (phase_) {
		case QUEST_PHASE::PHASE_QUEST:
			currentPhase_ = std::make_unique<QuestPhase>();
			break;
		case QUEST_PHASE::PHASE_CLASSWORK:
			currentPhase_ = std::make_unique<ClassWorkPhase>();
			break;
		case QUEST_PHASE::PHASE_JOB_CHANGE:
			currentPhase_ = std::make_unique<JobChangePhase>();
			break;
		}
	}

	if (currentPhase_) {
		currentPhase_->Update();
		// フェーズが終了したかどうかを確認
		if (currentPhase_->IsFinished()) {
			currentPhase_.reset(); // フェーズを終了してメニューに戻る
		}
	}
}

//描画処理
void GameScene::Draw(void)
{
	if (currentPhase_) {
		// ここが呼ばれていないと、どれだけ切り替わっても画面は変わりません
		currentPhase_->Draw();
	}
	else {
		// メニュー画面の描画処理
		DrawString(0, 0, "Scene : Game", 0xFFFFFF);

		int color = GetColor(255, 255, 255);
		int selectColor = GetColor(255, 255, 0); // 選択中は黄色にする

		DrawFormatString(100, 100, (phase_ == QUEST_PHASE::PHASE_QUEST ? selectColor : color), "クエストに出発");
		DrawFormatString(100, 140, (phase_ == QUEST_PHASE::PHASE_CLASSWORK ? selectColor : color), "授業を受ける");
		DrawFormatString(100, 180, (phase_ == QUEST_PHASE::PHASE_JOB_CHANGE ? selectColor : color), "資格試験");

		playerStatus_->Draw();

	}
}

//解放処理
void GameScene::Release(void)
{
}

void GameScene::ProcessPhaseSelection(void)
{
	//フェーズ選択の処理
	if (ins_.IsTrgDown(KEY_INPUT_UP) ||
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DG_UP))
	{
		//statusType_ = (statusType_ + 3) % 4;
		int index = static_cast<int>(phase_);
		index = (index - 1 + static_cast<int>(QUEST_PHASE::MAX)) % static_cast<int>(QUEST_PHASE::MAX);
		phase_ = static_cast<QUEST_PHASE>(index);
	}
	if (ins_.IsTrgDown(KEY_INPUT_DOWN) ||
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DG_DOWN))
	{
		int index = static_cast<int>(phase_);
		index = (index + 1) % static_cast<int>(QUEST_PHASE::MAX);
		phase_ = static_cast<QUEST_PHASE>(index);
	}

}
