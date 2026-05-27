#include <DxLib.h>
#include <algorithm>
#include "../../Application.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/InputManager.h"
#include "../../Utility/AsoUtility.h"
#include "QuestPhase.h"

QuestPhase::QuestPhase(PlayerStatus* playerStatus) 
	: playerStatus_(playerStatus)
	, command_(COMMAND::ATTACK)
	, battleStep_(BATTLE_STEP::DIFFICULTY_SELECTION)
{
}

void QuestPhase::Update(void)
{
	//ターン管理関数
	ManageTurn();
}

void QuestPhase::Draw(void)
{
	DrawString(0, 0, "Scene : Quest", 0xFFFFFF);

	if(battleStep_ != BATTLE_STEP::RESULT 
		&& battleStep_ != BATTLE_STEP::DIFFICULTY_SELECTION)
	{
		DrawFormatString(0, 100, 0xFFFFFF, "プレイヤーのHP %d / %d", playerStatus_->hp_, playerStatus_->maxHp_);
		DrawFormatString(0, 120, 0xFFFFFF, "敵のHP %d　/ %d", enemyHp_, enemyMaxHp_);
	}

	if (battleStep_ == BATTLE_STEP::DIFFICULTY_SELECTION)
	{
		Utility::DrawCommandMenu(0, 40, { "優しい", "普通", "難しい" }, static_cast<int>(difficulty_));
	}
	else if(battleStep_ == BATTLE_STEP::COMMAND_SELECTION)
	{
		DrawCommandSelection();
	}
	else if (battleStep_ == BATTLE_STEP::RESULT)
	{
		DrawFormatString(0, 150, 0xFFFFFF, "経験値を%d獲得した！", expGain_);
		DrawFormatString(0, 170, 0xFFFFFF, "LVが上がった！");
		DrawFormatString(0, 190, 0xFFFFFF, "基礎ステータスが上がった！");
		
		DrawString(0, 210, "Enterキーで次へ", 0xFFFFFF);
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
		//難易度に応じて敵のステータスを変える
		switch (difficulty_)
		{
		case DIFFICULTY::EASY:
			enemyMaxHp_ = static_cast<int>(MAX_HP * ENEMY_INCREASE);
			enemyHp_ = enemyMaxHp_;
			enemyPow_ = static_cast<int>(ENEMY_POW * ENEMY_INCREASE);
			enemySpeed_ = static_cast<int>(ENEMY_SPEED * ENEMY_INCREASE);
			expGain_ = static_cast<int>(EXP_GAIN * ENEMY_INCREASE);
			break;
		case DIFFICULTY::NORMAL:
			enemyMaxHp_ = MAX_HP;
			enemyHp_ = enemyMaxHp_;
			enemyPow_ = ENEMY_POW;
			enemySpeed_ = ENEMY_SPEED;
			expGain_ = EXP_GAIN;
			break;
		case DIFFICULTY::HARD:
			enemyMaxHp_ = static_cast<int>(MAX_HP * HARD_INCREASE);
			enemyHp_ = enemyMaxHp_;
			enemyPow_ = static_cast<int>(ENEMY_POW * HARD_INCREASE);
			enemySpeed_ = static_cast<int>(ENEMY_SPEED * HARD_INCREASE);
			expGain_ = static_cast<int>(EXP_GAIN * HARD_INCREASE);
			break;
		default:
			break;
		}
		
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
		//プレイヤーの行動の処理などをここに書く
		ProcessPlayerAction();
		break;
	case QuestPhase::BATTLE_STEP::DETERMINE:
		DetermineActionOrder();
		break;
	case QuestPhase::BATTLE_STEP::ACTION_LOOP:
		ProcessActionLoop();
		break;
	case QuestPhase::BATTLE_STEP::RESULT:
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
	//名前、速度、プレイヤーかどうか、ターゲット、行動の種類、ダメージ）
	actionOrder_.push_back({ "プレイヤー", playerStatus_->speed_, true, 0, (int)command_, 0 });

	//敵追加 (とりあえず1体)
	if (enemyHp_ > 0) 
	{
		actionOrder_.push_back({ "スライム", enemySpeed_, false, 0, 0, 0 });
	}

	//ソート
	std::sort(actionOrder_.begin(), actionOrder_.end(), [](const ActionUnit& a, const ActionUnit& b)
		{
			return a.speed > b.speed;
		}
	);

	//currentActionIdx_ の初期化もここで行うのが良いです
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

	if (battleMessage_ == "")
	{
		if (unit.isPlayer && command_ == COMMAND::ATTACK)
		{
			battleMessage_ = unit.name + " の攻撃！";
			enemyHp_ -= playerStatus_->power_;
		}
		else if (unit.isPlayer && command_ == COMMAND::MAGIC)
		{
			battleMessage_ = unit.name + " の魔法攻撃！";
			enemyHp_ -= playerStatus_->magic_;
		}
		else if (unit.isPlayer && command_ == COMMAND::ITEM)
		{
			battleMessage_ = unit.name + " がアイテム使用！";
			playerStatus_->hp_ += HP_RECOVERY_AMOUNT;
		}
		else
		{
			battleMessage_ = unit.name + " の攻撃！";
			playerStatus_->Damage(enemyPow_);
		}
		if (enemyHp_ <= 0)
		{
			battleMessage_ = unit.name + " が敵を倒した！";
		}
	}

	if (ins_.IsTrgDown(KEY_INPUT_RETURN))
	{
		battleMessage_ = "";
		currentActionIdx_++;

		// 判定：敵を倒したか？
		if (enemyHp_ <= 0)
		{
			//戦闘終了時にも忘れずに魔法フラグを更新する
			wasMagicUsedLastTurn_ = magicUsedThisTurn_;
			PhaseBase::phaseResult_ = PhaseBase::PHASE_RESULT::NEXT_TURN; //次のターンへ
			battleStep_ = BATTLE_STEP::RESULT;
			return;
		}
	}
}

void QuestPhase::DisplayResult(void)
{
	//ここで勝敗の結果を表示する処理を書く
	//Enterが押されたらメッセージをリセットして次へ
	if (ins_.IsTrgDown(KEY_INPUT_RETURN))
	{
		playerStatus_->GetExp(expGain_);
		isFinished_ = true; //フェーズ終了
	}
}

void QuestPhase::ProcessPlayerAction()
{
	//選択肢の数（今回は仮に3つ：「攻撃」「魔法」「アイテム」）
	int maxItems = static_cast<int>(COMMAND::MAX);
	
	//カーソル移動
	Utility::ProcessCommandMenuSelection(reinterpret_cast<int&>(command_), maxItems);

	//決定処理
	if (ins_.IsTrgDown(KEY_INPUT_RETURN))
	{
		//魔法を選択している、かつ前回魔法を使っていたら決定させない
		if (command_ == COMMAND::MAGIC && wasMagicUsedLastTurn_)
		{
			//ここで「今は使えない！」というSEを鳴らしたり、
			//メッセージを一時的に出すと親切です
			return;
		}

		//今回魔法を使うかどうかを記録しておく
		if (command_ == COMMAND::MAGIC) {
			magicUsedThisTurn_ = true;
		}
		else {
			magicUsedThisTurn_ = false;
		}

		//次のステップへ
		battleStep_ = BATTLE_STEP::DETERMINE;
	}

	if(ins_.IsTrgDown(KEY_INPUT_TAB))
	{
		PhaseBase::phaseResult_ = PhaseBase::PHASE_RESULT::CANCEL; //コマンド選択に戻る
		isFinished_ = true; //フェーズ終了
	}
}

void QuestPhase::DrawCommandSelection(void)
{
	Utility::DrawCommandMenu(0, 200, { "攻撃", "魔法", "アイテム" }, static_cast<int>(command_));
}
