#include <DxLib.h>
#include <algorithm>
#include "../../Application.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/InputManager.h"
#include "../../Utility/AsoUtility.h"
#include "../../Scene/GameScene.h"
#include "../../Object/Enemy.h"
#include "QuestPhase.h"

//外部にある敵生成関数（Enemy.cppなどに実装している想定）
extern Enemy* SpawnEnemyByTurn(int currentTurn);

//コンストラクタ
QuestPhase::QuestPhase(PlayerStatus* playerStatus, GameScene& gameScene)
	: gameScene_(gameScene)
	, playerStatus_(playerStatus)
	, command_(COMMAND::ATTACK)
	, battleStep_(BATTLE_STEP::COMMAND_SELECTION)
	, currentWave_(1) //Waveの初期化
{
	activeEnemy_ = SpawnEnemyByTurn(gameScene_.GetTurn());//クエスト開始時に1戦目の敵を生成する
}

//デストラクタ
QuestPhase::~QuestPhase(void)
{
	if (activeEnemy_ != nullptr)
	{
		delete activeEnemy_;
		activeEnemy_ = nullptr;
	}
}

//更新処理
void QuestPhase::Update(void)
{
	//ターン管理関数
	ManageTurn();
}

//描画処理
void QuestPhase::Draw(void)
{
	DrawString(0, 0, "Scene : Quest", 0xFFFFFF);

	if (battleStep_ == BATTLE_STEP::RESULT)
	{
		DrawString(0, 150, "遠征クリア！ 経験値を獲得した！", 0xFFFFFF);
		DrawString(0, 170, "LVが1上がった！", 0xFFFFFF);
		DrawString(0, 190, "基礎ステータスが上がった！", 0xFFFFFF);
		DrawString(0, 210, "Enterキーで次へ", 0xFFFFFF);
	}
	else
	{
		DrawFormatString(0, 20, 0xFFFFFF, "command %d", static_cast<int>(command_));
		DrawFormatString(0, 80, 0xFFFF00, "【 BATTLE %d / %d 】", currentWave_, MAX_WAVES);//連戦（Wave）の表示
		DrawFormatString(0, 100, 0xFFFFFF, "プレイヤーのHP %d / %d", playerStatus_->hp_, playerStatus_->maxHp_);

		//敵のHPを activeEnemy_ から直接もらう
		if (activeEnemy_ != nullptr && !activeEnemy_->IsDead())
		{
			DrawFormatString(0, 120, 0xFFFFFF, "%sのHP %d", activeEnemy_->GetName().c_str(), activeEnemy_->GetCurrentHp());
			//画像を表示する場合はここに activeEnemy_->Draw(); を追加
		}
		Utility::DrawCommandMenu(0, 200, { "攻撃", "魔法", "アイテム" }, static_cast<int>(command_));

		if (battleMessage_ != "")
		{
			DrawFormatString(0, 150, 0xFF0000, battleMessage_.c_str());
			DrawString(0, 170, "Enterキーで次へ", 0xFF0000);
		}
	}

	if (battleStep_ == BATTLE_STEP::DIFFICULTY_SELECTION)
	{
		Utility::DrawCommandMenu(0, 40, { "優しい", "普通", "難しい" }, static_cast<int>(difficulty_));
	}
	else if(battleStep_ == BATTLE_STEP::COMMAND_SELECTION)
	{
		DrawCommandSelection();
	}
	if (battleMessage_ != "")
	{
		DrawFormatString(0, 150, 0xFF0000, battleMessage_.c_str());
		DrawString(0, 170, "Enterキーで次へ", 0xFF0000);
	}
}

bool QuestPhase::IsFinished() const
{
	return isFinished_;
}

void QuestPhase::ProcessDifficulty(void)
{
	//選択肢の数（今回は仮に3つ：「優しい」「普通」「難しい」）
	int maxDifficulty = static_cast<int>(DIFFICULTY::MAX);
	//カーソル移動
	Utility::ProcessCommandMenuSelection(reinterpret_cast<int&>(difficulty_), maxDifficulty);

	//決定処理
	if (ins_.IsTrgDown(KEY_INPUT_RETURN))
	{
		battleStep_ = BATTLE_STEP::COMMAND_SELECTION; //次のステップへ
	}
}

void QuestPhase::ManageTurn(void)
{
	switch (battleStep_)
	{
	case QuestPhase::BATTLE_STEP::DIFFICULTY_SELECTION:
		//難易度選択処理などをここに書く
		ProcessDifficulty();
		break;
	case QuestPhase::BATTLE_STEP::COMMAND_SELECTION:
		ProcessPlayerAction();
		break;
	case QuestPhase::BATTLE_STEP::COMMAND_SUB_SELECTION:
		//プレイヤーの行動の処理などをここに書く
		ProcessPlayerSubAction();
		break;
	case QuestPhase::BATTLE_STEP::DETERMINE:
		//行動の順番を決定する関数
		DetermineActionOrder();
		break;
	case QuestPhase::BATTLE_STEP::ACTION_LOOP:
		//行動の順番に従って処理を行う関数
		ProcessActionLoop();
		break;
	case QuestPhase::BATTLE_STEP::RESULT:
		//結果表示の処理などをここに書く
		DisplayResult();
		break;
	default:
		break;
	}
}

void QuestPhase::DetermineActionOrder(void)
{
	actionOrder_.clear();

	//プレイヤー追加 (idは0、ターゲットは今のところ敵の0番とする)
	actionOrder_.push_back({ "プレイヤー", playerStatus_->speed_, true, 0, (int)command_, 0 });

	//敵の追加（DecideActionでランダムに行動を決定）
	if (activeEnemy_ != nullptr && !activeEnemy_->IsDead())
	{
		EnemyActionInfo eAction = activeEnemy_->DecideAction();
		actionOrder_.push_back({ eAction.name, eAction.speed, false, 0, eAction.actionType, 0 });
	}

	//ソート（スピード順）
	std::sort(actionOrder_.begin(), actionOrder_.end(), [](const ActionUnit& a, const ActionUnit& b){return a.speed > b.speed;});

	currentActionIdx_ = 0;
	battleStep_ = BATTLE_STEP::ACTION_LOOP;
}

void QuestPhase::ProcessActionLoop(void)
{
	if (currentActionIdx_ >= actionOrder_.size())
	{
		//ターン終了時にフラグを更新
		wasMagicUsedLastTurn_ = magicUsedThisTurn_;

		battleStep_ = BATTLE_STEP::COMMAND_SELECTION;
		currentActionIdx_ = 0;
		battleMessage_ = "";
		return;
	}

	auto& unit = actionOrder_[currentActionIdx_];

	//①ダメージとメッセージの処理
	if (battleMessage_ == "")
	{
		if (unit.isPlayer)
		{
			if (command_ == COMMAND::ATTACK) 
			{
				battleMessage_ = unit.name + " の攻撃！";
				activeEnemy_->TakeDamage(playerStatus_->Attack());
			}
			else if (command_ == COMMAND::MAGIC) 
			{
				battleMessage_ = unit.name + " の魔法攻撃！";
				activeEnemy_->TakeDamage(playerStatus_->MagicAttack());
			}
			else if (command_ == COMMAND::ITEM) 
			{
				battleMessage_ = unit.name + " がアイテム使用！";
				playerStatus_->hp_ += 200;
			}
		}
		else
		{
			//敵のターン
			battleMessage_ = unit.name + " の攻撃！";
			playerStatus_->Damage(1); //とりあえず固定ダメージ10。後で敵の攻撃力に変更。
		}

		//倒した時の上書き
		if (activeEnemy_->IsDead())
		{
			battleMessage_ = activeEnemy_->GetName() + " を倒した！";
		}
	}

	//②Enterキー待ちと、連戦(Wave)の処理
	if (ins_.IsTrgDown(KEY_INPUT_RETURN))
	{
		//もし敵を倒していたら
		if (activeEnemy_->IsDead())
		{
			//倒した敵から経験値を獲得
			playerStatus_->GetExp(activeEnemy_->GetExp());

			//今の敵を消去
			delete activeEnemy_;
			activeEnemy_ = nullptr;

			//連戦チェック
			if (currentWave_ < MAX_WAVES)
			{
				currentWave_++; //次のWaveへ
				activeEnemy_ = SpawnEnemyByTurn(gameScene_.GetTurn()); // 次の敵を生成

				wasMagicUsedLastTurn_ = magicUsedThisTurn_;
				battleStep_ = BATTLE_STEP::COMMAND_SELECTION;
				currentActionIdx_ = 0;
				battleMessage_ = "";
				return;
			}
			else
			{
				//全Waveクリア！
				wasMagicUsedLastTurn_ = magicUsedThisTurn_;
				battleStep_ = BATTLE_STEP::RESULT;
				battleMessage_ = "";
				return;
			}
		}

		//敵がまだ生きていれば、次のキャラの行動へ
		battleMessage_ = "";
		currentActionIdx_++;
	}
}

void QuestPhase::DisplayResult(void)
{
	//経験値獲得は ProcessActionLoop内で敵を倒すたびに行うように変更したため、ここは次へ進む処理のみ
	if (ins_.IsTrgDown(KEY_INPUT_RETURN))
	{
		PhaseBase::phaseResult_ = PhaseBase::PHASE_RESULT::NEXT_TURN;
		isFinished_ = true; //フェーズ終了
	}
}

void QuestPhase::ProcessPlayerAction()
{
	int maxItems = static_cast<int>(COMMAND::MAX);
	
	//カーソル移動
	Utility::ProcessCommandMenuSelection(reinterpret_cast<int&>(command_), maxItems);

	//決定処理
	if (ins_.IsTrgDown(KEY_INPUT_RETURN))
	{
		if (command_ == COMMAND::MAGIC && wasMagicUsedLastTurn_)return; //魔法の連続使用制限

		if (command_ == COMMAND::MAGIC) magicUsedThisTurn_ = true;
		else magicUsedThisTurn_ = false;

		//subActionMessages_.clear(); //一度リセット
		//subMenuCursor_ = 0;    //カーソルを先頭に

		//switch (command_)
		//{
		//case QuestPhase::COMMAND::ATTACK:
		//	subActionMessages_ = { "単体攻撃", "全体攻撃" }; 
		//	break;
		//case QuestPhase::COMMAND::MAGIC:
		//	subActionMessages_ = { "単体魔法","回復", "強化", "状態異常付与" };
		//	break;
		//case QuestPhase::COMMAND::ITEM:
		//	subActionMessages_ = { "回復", "状態異常回復" };
		//	break;
		//case QuestPhase::COMMAND::MAX:
		//	break;
		//default:
		//	break;
		//}

		//次のステップへ
		battleStep_ = BATTLE_STEP::COMMAND_SUB_SELECTION;
	}
}

void QuestPhase::ProcessPlayerSubAction(void)
{
	//// 選択肢の数を、埋め込まれた配列のサイズから自動取得！
	//int maxSubItems = static_cast<int>(subActionMessages_.size());
	//if (maxSubItems == 0) return; // 念のため

	//
	//Utility::ProcessCommandMenuSelection(subMenuCursor_, maxSubItems);

	// --- 決定処理 ---
	if (ins_.IsTrgDown(KEY_INPUT_RETURN))
	{
		// 【重要】ここで「何番のサブメニューを選んだか」を記憶しておく！
		// 例：chosenSubMenuIdx_ = subMenuCursor_; 
		// この数値を、後の DetermineActionOrder や ActionUnit に引き渡します。

		// 行動決定へ進む
		battleStep_ = BATTLE_STEP::DETERMINE;
	}

	if (ins_.IsTrgDown(KEY_INPUT_TAB))
	{
		battleStep_ = BATTLE_STEP::COMMAND_SELECTION;
	}
}

void QuestPhase::DrawCommandSelection(void)
{
	Utility::DrawCommandMenu(0, 200, { "攻撃", "魔法", "アイテム" }, static_cast<int>(command_));
}
