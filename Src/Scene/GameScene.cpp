#include <DxLib.h>
#include "../Application.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/InputManager.h"
#include "../Utility/AsoUtility.h"
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
	playerImg_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::PLAYER).handleId_;
	stageImg_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::GAME_SCENE).handleId_;
}

//更新処理
void GameScene::Update(void)
{
	//３ターン目までチュートリアルの処理
	if (turn_ <= 3)
	{
		ProcessTutorial();
	}
	

	//まず現在のフェーズのUpdateを回す
	if (currentPhase_) {
		currentPhase_->Update();

		//フェーズが終わった瞬間の処理
		if (currentPhase_->phaseResult_ == PhaseBase::PHASE_RESULT::NEXT_TURN
			&& currentPhase_->IsFinished()) {
			currentPhase_.reset(); // クエスト終了！
			turn_++; //ターンを進める
			isInputBlocked_ = true; // ←ここで「次のフレームは入力を受け付けない」フラグを立てる
			return;
		}
		else if (currentPhase_->phaseResult_ == PhaseBase::PHASE_RESULT::CANCEL
			&& currentPhase_->IsFinished())
		{
			currentPhase_.reset(); // クエスト終了！
			isInputBlocked_ = true;
			return; //コマンド選択をやり直す
		}
	}

	//入力ブロック中なら解除して飛ばす
	if (isInputBlocked_) 
	{
		isInputBlocked_ = false;
		return;
	}

	//フェーズが実行中でない場合（メニュー選択）
	//上のif文の中でreturnしていれば、最終ターン終了時はここは実行されない
	if (!currentPhase_) 
	{
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
		playerStatus_->Draw();
	}
	else 
	{
		DrawGraph(0, 0, stageImg_, TRUE);
		//メニュー画面の描画処理
		DrawFormatString(0, 0, 0xFFFFFF, "Scene : Game 現在のターン %d", turn_);

		int color = GetColor(255, 255, 255);
		int selectColor = GetColor(255, 255, 0); //選択中は黄色にする

		Utility::DrawCommandMenu(200, 200,{"クエスト", "授業", "資格試験"}, (phase_));

		DrawGraph(1100, 520, playerImg_, TRUE);

		//仮でプレイヤー情報を表示
		playerStatus_->Draw();
	}
	//DrawTutorial();
}

//解放処理
void GameScene::Release(void)
{
}

void GameScene::ProcessTutorial(void)
{
	
}

void GameScene::DrawTutorial(void)
{
	//チュートリアルの処理
	if (turn_ == 1)
	{
		DrawFormatString(0, 1000, 0xFFFFFF, "チュートリアル：クエストフェーズの説明");
	}
	else if (turn_ == 2)
	{
		DrawFormatString(0, 1000, 0xFFFFFF, "チュートリアル：授業フェーズの説明");
	}
	else if (turn_ == 3)
	{
		DrawFormatString(0, 1000, 0xFFFFFF, "チュートリアル：資格試験フェーズの説明");
	}
}

void GameScene::ProcessPhaseSelection(void)
{
	//フェーズ選択の処理
	if(turn_ >= MAX_TURN)
	{
		phase_ = QUEST_PHASE::PHASE_FINAL;
		return;
	}

	//安全に int 型の変数に写し取る
	int currentIndex = static_cast<int>(phase_);
	int maxItems = static_cast<int>(QUEST_PHASE::PHASE_FINAL);

	//int型の変数を渡して、メニュー選択処理を行う（currentIndexの中身が更新される）
	Utility::ProcessCommandMenuSelection(currentIndex, maxItems);

	//更新された int の値を、安全に元の enum class 型に戻して代入する
	phase_ = static_cast<QUEST_PHASE>(currentIndex);
}

void GameScene::ProcessPhaseDecision()
{
	if (ins_.IsTrgDown(KEY_INPUT_RETURN) || ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT))
	{
		switch (phase_) 
		{
		case QUEST_PHASE::PHASE_QUEST:
			currentPhase_ = std::make_unique<QuestPhase>(playerStatus_,*this);
			break;
		case QUEST_PHASE::PHASE_CLASSWORK:
			currentPhase_ = std::make_unique<ClassWorkPhase>(playerStatus_);
			break;
		case QUEST_PHASE::PHASE_JOB_CHANGE:
			currentPhase_ = std::make_unique<JobChangePhase>(playerStatus_);
			break;
		case QUEST_PHASE::PHASE_FINAL:
			currentPhase_ = std::make_unique<FinalPhase>(playerStatus_);
			break;
		}
	}
}