#include <DxLib.h>
#include "../Application.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "Phase/QuestPhase.h"
#include "Phase/ClassWorkPhase.h"
#include "Phase/JobChangePhase.h"
#include "Phase/FinalPhase.h"
#include "GameScene.h"

//デフォルトコンストラクタ
GameScene::GameScene(void)
{
	turn_ = 0;
	phase_ = QUEST_PHASE::PHASE_QUEST;
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
	turn_ = 1;
	phase_ = QUEST_PHASE::PHASE_QUEST;
}

//更新処理
void GameScene::Update(void)
{
	//現在のフェーズの更新処理
	if (currentPhase_) {

		currentPhase_->Update();

		//フェーズが終了したかどうかを確認
		if (currentPhase_->IsFinished()) {

			//ターン数を増やす
			turn_++;

			//フェーズを終了してメニューに戻る
			currentPhase_.reset();

			if (turn_ > MAX_TURN) {
				//ゲームオーバーなどの処理をここに書く
				//今回はとりあえずターン数が最大を超えたらゲーム終了
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::CLEAR);
				return;
			}
		}
	}

	//2.フェーズが実行中でない場合（メニュー選択）
	//上のif文の中でreturnしていれば、最終ターン終了時はここは実行されない
	if (!currentPhase_) {
		ProcessPhaseSelection();
		ProcessPhaseDecision();
	}
}

//描画処理
void GameScene::Draw(void)
{
	if (currentPhase_) {
		//ここが呼ばれていないと、どれだけ切り替わっても画面は変わりません
		currentPhase_->Draw();
	}
	else if(turn_ >= MAX_TURN){
		DrawFormatString(0, 0, 0xFFFFFF, "Scene : Game 現在のターン %d", turn_);

		int color = GetColor(255, 255, 255);
		int selectColor = GetColor(255, 255, 0); //選択中は黄色にする

		DrawFormatString(100, 100, (phase_ == QUEST_PHASE::PHASE_FINAL ? selectColor : color), "ボスバトル");

	}
	else {
		//メニュー画面の描画処理
		DrawFormatString(0, 0, 0xFFFFFF, "Scene : Game 現在のターン %d", turn_);

		int color = GetColor(255, 255, 255);
		int selectColor = GetColor(255, 255, 0); //選択中は黄色にする

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
	if(turn_ >= MAX_TURN)
	{
		phase_ = QUEST_PHASE::PHASE_FINAL;
		return;
	}
	if (ins_.IsTrgDown(KEY_INPUT_UP) ||
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DG_UP))
	{
		//statusType_ = (statusType_ + 3) % 4;
		int index = static_cast<int>(phase_);
		index = (index - 1 + static_cast<int>(QUEST_PHASE::PHASE_FINAL)) % static_cast<int>(QUEST_PHASE::PHASE_FINAL);
		phase_ = static_cast<QUEST_PHASE>(index);
	}
	else if (ins_.IsTrgDown(KEY_INPUT_DOWN) ||
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DG_DOWN))
	{
		int index = static_cast<int>(phase_);
		index = (index + 1) % static_cast<int>(QUEST_PHASE::PHASE_FINAL);
		phase_ = static_cast<QUEST_PHASE>(index);
	}
}

void GameScene::ProcessPhaseDecision()
{
	if (ins_.IsTrgUp(KEY_INPUT_RETURN) || ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT))
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
		case QUEST_PHASE::PHASE_FINAL:
			currentPhase_ = std::make_unique<FinalPhase>();
			break;
		}
	}
}