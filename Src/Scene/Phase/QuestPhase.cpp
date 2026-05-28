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
	else if (battleStep_ == BATTLE_STEP::COMMAND_SUB_SELECTION)
	{
		DrawString(0, 160, "TABで戻る", 0xFF0000);
		Utility::DrawCommandMenu(0, 200, subActionMessages_, static_cast<int>(subMenuCursor_));
	}
	else if (battleStep_ == BATTLE_STEP::RESULT)
	{
		int yOffset = 150;

		for (size_t i = 0; i <= messageDisplayIdx_ && i < resultMessages_.size(); ++i)
		{
			DrawString(0, yOffset, resultMessages_[i].c_str(), 0xFFFFFF);
			yOffset += 20;
		}

		// 「Enterキーで次へ」は常に一番下に出す
		DrawString(0, yOffset + 20, "Enterキーで次へ", 0xFFFFFF);
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
			enemyHp_ -= playerStatus_->Attack();
		}
		else if (unit.isPlayer && command_ == COMMAND::MAGIC)
		{
			battleMessage_ = unit.name + " の魔法攻撃！";
			enemyHp_ -= playerStatus_->MagicAttack();
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
			
			DrawResultMessage();
			
			PhaseBase::phaseResult_ = PhaseBase::PHASE_RESULT::NEXT_TURN; //次のターンへ
			battleStep_ = BATTLE_STEP::RESULT;
			return;
		}
	}
}

void QuestPhase::DrawResultMessage()
{
	//次のリザルト画面のために、配列とインデックスをリセットする
	resultMessages_.clear();
	messageDisplayIdx_ = 0; //0に初期化

	//配列（resultMessages_）に順番に文字列を埋め込んでいく（push_back）
	resultMessages_.push_back(std::to_string(expGain_) + " の経験値を獲得した！");

	//プレイヤーに経験値を与えて、レベルアップしたかチェック
	int oldLevel = playerStatus_->level_;
	playerStatus_->GetExp(expGain_); //内部の経験値・レベル加算処理
	int newLevel = playerStatus_->level_;

	//レベルが上がっていた場合のみ、追加のメッセージを配列に埋め込む
	if (newLevel > oldLevel)
	{
		resultMessages_.push_back("LVが " + std::to_string(newLevel) + " に上がった！");
		resultMessages_.push_back("基礎ステータスが上がった！");
	}
}

void QuestPhase::DisplayResult(void)
{	
	// Enterが押されたときの処理
	if (ins_.IsTrgDown(KEY_INPUT_RETURN))
	{
		//まだ表示していないメッセージの行がある場合
		if (messageDisplayIdx_ + 1 < resultMessages_.size())
		{
			messageDisplayIdx_++; // 次の行を表示に加える
		}
		else
		{
			//すべての行が出きった状態でさらにEnterが押されたら、終了処理へ
			playerStatus_->GetExp(expGain_); //(※経験値の獲得タイミングはここで調整してください)
			isFinished_ = true;        //フェーズ終了して拠点へ
		}
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
			//【重要】魔法を選択しているのに決定させない場合、次のステップへ進まないようにすること！
			return;
		}
		//今回魔法を使うかどうかを記録しておく
		if (command_ == COMMAND::MAGIC) {
			magicUsedThisTurn_ = true;
		}
		else {
			magicUsedThisTurn_ = false;
		}

		subActionMessages_.clear(); //一度リセット
		subMenuCursor_ = 0;    //カーソルを先頭に

		switch (command_)
		{
		case QuestPhase::COMMAND::ATTACK:
			subActionMessages_ = { "単体攻撃", "全体攻撃" }; 
			break;
		case QuestPhase::COMMAND::MAGIC:
			subActionMessages_ = { "単体魔法","回復", "強化", "状態異常付与" };
			break;
		case QuestPhase::COMMAND::ITEM:
			subActionMessages_ = { "回復", "状態異常回復" };
			break;
		case QuestPhase::COMMAND::MAX:
			break;
		default:
			break;
		}

		//次のステップへ
		battleStep_ = BATTLE_STEP::COMMAND_SUB_SELECTION;
	}
}

void QuestPhase::ProcessPlayerSubAction(void)
{
	// 選択肢の数を、埋め込まれた配列のサイズから自動取得！
	int maxSubItems = static_cast<int>(subActionMessages_.size());
	if (maxSubItems == 0) return; // 念のため

	
	Utility::ProcessCommandMenuSelection(subMenuCursor_, maxSubItems);

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
