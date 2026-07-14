#include <DxLib.h>
#include "../Application.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/InputManager.h"
#include "../Utility/AsoUtility.h"
#include "Phase/QuestPhase.h"
#include "Phase/ClassWorkPhase.h"
#include "Phase/JobChangePhase.h"
#include "Phase/EventPhase.h"
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
	if (SceneManager::GetInstance().IsTutorialEnabled())
	{
		if (turn_ <= 3)
		{
			ProcessTutorial();
		}
	}

	if (turn_ == 16)
	{
		phase_ = QUEST_PHASE::PHASE_QUEST;
	}
	
	//まず現在のフェーズのUpdateを回す
	if (currentPhase_) 
	{
		currentPhase_->Update();

		//フェーズが終わった瞬間の処理
		if (currentPhase_->IsFinished())
		{
			//フェーズの終了結果を一時保存してからリセットする
			auto result = currentPhase_->phaseResult_;
			currentPhase_.reset();

			//クエストなどをクリアして、ターンを進める場合
			if (result == PhaseBase::PHASE_RESULT::NEXT_TURN)
			{
				turn_++; //ターンを進める

				//4ターン目になった瞬間の処理
				if (turn_ == 4 && playerStatus_->currentRoute_ == PLAYER_ROUTE::NONE)
				{
					//メニュー選択に戻さずそのままイベントを強制スタート
					currentPhase_ = std::make_unique<EventPhase>(playerStatus_);
				}
			}
			//CANCELの場合などは turn_ は増えず、そのまま拠点メニューに戻る
			isInputBlocked_ = true; //次のフレームは入力を受け付けないフラグを立てる
			return;
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
		//ここが呼ばれていないと、どれだけ切り替わっても画面は変変わらない
		currentPhase_->Draw();
	}
	else 
	{
		DrawGraph(0, 0, stageImg_, TRUE);
		//メニュー画面の描画処理
		DrawFormatString(0, 0, 0xFFFFFF, "Scene : Game 現在のターン %d", turn_);

		//現在のルートを文字列に変換して表示する
		std::string routeName = "未選択";
		switch (playerStatus_->currentRoute_)
		{
		case PLAYER_ROUTE::BREAKTHROUGH: routeName = "打破"; break;
		case PLAYER_ROUTE::SALVATION:    routeName = "救世"; break;
		case PLAYER_ROUTE::TRUTH:        routeName = "真理"; break;
		case PLAYER_ROUTE::SELFLESS:     routeName = "無欲"; break;
		}
		// ターンの少し下に黄色っぽく表示
		DrawFormatString(0, 30, GetColor(255, 255,0), "現在のルート : %s", routeName.c_str());

		if (SceneManager::GetInstance().IsTutorialEnabled())
		{
			DrawTutorial();
		}
		
		int color = GetColor(255, 255, 255);
		int selectColor = GetColor(255, 255, 0); //選択中は黄色にする

		Utility::DrawCommandMenu(200, 200,{"クエスト", "授業", "資格試験"}, (phase_));

		DrawGraph(700, 200, playerImg_, TRUE);

		//仮でプレイヤー情報を表示
		playerStatus_->Draw();
	}
	
}

//解放処理
void GameScene::Release(void)
{
}

void GameScene::ProcessTutorial(void)
{
	//チュートリアルの処理
	if (turn_ == 1)
	{
		phase_ = QUEST_PHASE::PHASE_QUEST;
	}
	else if (turn_ == 2)
	{
		phase_ = QUEST_PHASE::PHASE_CLASSWORK;
	}
	else if (turn_ == 3)
	{
		phase_ = QUEST_PHASE::PHASE_JOB_CHANGE;
	}
}

void GameScene::DrawTutorial(void)
{
	//チュートリアルの処理
	if (turn_ == 1)
	{
		DrawFormatString(0, 500, 0xFFFFFF, "チュートリアル：クエストフェーズの説明\nここでは敵と戦って経験値を獲得します");
	}
	else if (turn_ == 2)
	{
		DrawFormatString(0, 500, 0xFFFFFF, "チュートリアル：授業フェーズの説明\nここではスキルを学びます");
	}
	else if (turn_ == 3)
	{
		DrawFormatString(0, 500, 0xFFFFFF, "チュートリアル：資格試験フェーズの説明\nここでは職業を選択できます");
	}
}

void GameScene::ProcessPhaseSelection(void)
{
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
	if (ins_.IsTrgDown(KEY_INPUT_RETURN) || 
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
	{
		switch (phase_) 
		{
		case QUEST_PHASE::PHASE_QUEST:
			currentPhase_ = std::make_unique<QuestPhase>(playerStatus_,*this);
			break;
		case QUEST_PHASE::PHASE_CLASSWORK:
			currentPhase_ = std::make_unique<ClassWorkPhase>(playerStatus_,*this);
			break;
		case QUEST_PHASE::PHASE_JOB_CHANGE:
			currentPhase_ = std::make_unique<JobChangePhase>(playerStatus_, *this);
			break;
		}
	}
}