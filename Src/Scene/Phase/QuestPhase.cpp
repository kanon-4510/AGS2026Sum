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
	, battleStep_(BATTLE_STEP::DIFFICULTY_SELECTION)
	, currentWave_(1) //Waveの初期化
{
	activeEnemy_ = SpawnEnemyByTurn(gameScene_.GetTurn());//クエスト開始時に1戦目の敵を生成する
	statusEffect_ = STATUS_EFFECT::NONE;	//状態異常の初期化
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

	if(battleStep_ != BATTLE_STEP::DIFFICULTY_SELECTION
		&& battleStep_ != BATTLE_STEP::RESULT)
	{
		DrawFormatString(0, 80, 0xFFFF00, "【 BATTLE %d / %d 】", currentWave_, MAX_WAVES);//連戦（Wave）の表示
		DrawFormatString(0, 100, 0xFFFFFF, "プレイヤーのHP %d / %d", playerStatus_->hp_, playerStatus_->GetMaxHp());

		//敵のHPを activeEnemy_ から直接もらう
		if (activeEnemy_ != nullptr && !activeEnemy_->IsDead())
		{
			DrawFormatString(0, 120, 0xFFFFFF, "%sのHP %d", activeEnemy_->GetName().c_str(), activeEnemy_->GetCurrentHp());
			//画像を表示する場合はここに activeEnemy_->Draw(); を追加
		}
	}

	if (battleStep_ == BATTLE_STEP::DIFFICULTY_SELECTION)
	{
		Utility::DrawCommandMenu(0, 40, { "優しい", "普通", "難しい" }, static_cast<int>(difficulty_));
	}
	else if (battleStep_ == BATTLE_STEP::COMMAND_SELECTION) //難易度選択中はコマンドやHPを表示しない
	{
		DrawCommandSelection();

		if (battleMessage_ != "")
		{
			DrawFormatString(0, 150, 0xFF0000, battleMessage_.c_str());
			DrawString(0, 170, "Enterキーで次へ", 0xFF0000);
		}
	}
	else if (battleStep_ == BATTLE_STEP::COMMAND_SUB_SELECTION)
	{
		DrawFormatString(0, 40, 0xFFFFFFF, "%d", subMenuCursor_);

		//サブコマンドの描画（例：魔法の種類やアイテムの選択肢など）
		Utility::DrawCommandMenu(0, 200, subActionMessages_, subMenuCursor_);
		DrawFormatString(0, 80, 0xFFFF00, "【 BATTLE %d / %d 】", currentWave_, MAX_WAVES);//連戦（Wave）の表示
	}
	
	if (activeEnemy_ != nullptr && !activeEnemy_->IsDead())
	{
		DrawFormatString(0, 150, 0xFF0000, battleMessage_.c_str());
		DrawString(0, 170, "Enterキーで次へ", 0xFF0000);
	}

	if (battleStep_ == BATTLE_STEP::RESULT)
	{
		DrawString(0, 150, "遠征クリア！ 経験値を獲得した！", 0xFFFFFF);
		DrawString(0, 170, "レベルが上がった！", 0xFFFFFF);
		DrawString(0, 190, "基礎ステータスが上がった！", 0xFFFFFF);
		DrawString(0, 210, "Enterキーで次へ", 0xFFFFFF);
	}
}

bool QuestPhase::IsFinished() const
{
	return isFinished_;
}

void QuestPhase::ProcessDifficulty(void)
{
	//選択肢の数（今回は仮に3つ：「優しい」「普通」「難しい」）
	int difficultyIndex = static_cast<int>(difficulty_); //enum class を int に変換して操作する
	int maxDifficulty = static_cast<int>(DIFFICULTY::MAX);
	//カーソル移動
	Utility::ProcessCommandMenuSelection(difficultyIndex, maxDifficulty);

	//更新された int の値を、安全に元の enum class 型に戻して代入する
	difficulty_ = static_cast<DIFFICULTY>(difficultyIndex);
	
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
		//DETERMINEに進むかCOMMAND_SELECTIONに戻るか
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
	case QuestPhase::BATTLE_STEP::STATUS_EFFECT:
		//状態異常の処理を行う関数
		ProcessStatusEffect();
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
	actionOrder_.push_back({ "プレイヤー", playerStatus_->speed_, true, 0, (int)command_, 0});
	if (command_ == COMMAND::ATTACK)
	{
		if (subMenuCursor_ == 0) {
			actionOrder_.back().skillName = "単体攻撃";
		}
		else {
			actionOrder_.back().skillName = "全体攻撃";
		}
	}
	// 魔法攻撃（1）の場合
	else if (command_ == COMMAND::MAGIC)
	{
		// サブメニューの選択肢に応じて魔法の性質を分ける！
		if (subMenuCursor_ == static_cast<int>(MAGIC_TYPE::MAGIC_ATTACK)) {
			actionOrder_.back().magicType = MAGIC_TYPE::MAGIC_ATTACK; //攻撃魔法
		}
		else if (subMenuCursor_ == static_cast<int>(MAGIC_TYPE::HEAL)) {
			actionOrder_.back().magicType = MAGIC_TYPE::HEAL;         //回復魔法
		}
		else if (subMenuCursor_ == static_cast<int>(MAGIC_TYPE::BUFF)) {
			actionOrder_.back().magicType = MAGIC_TYPE::BUFF;         //強化魔法
		}
		else if (subMenuCursor_ == static_cast<int>(MAGIC_TYPE::DEBUFF)) {
			actionOrder_.back().magicType = MAGIC_TYPE::DEBUFF;         //弱化魔法
		}
		// 将来のバフ・デバフ魔法もここに条件を足すだけ！
	}

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
		battleStep_ = BATTLE_STEP::STATUS_EFFECT;
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
				switch (unit.magicType)
				{
				case MAGIC_TYPE::MAGIC_ATTACK:
					battleMessage_ = unit.name + " の魔法攻撃！";
					activeEnemy_->TakeDamage(playerStatus_->MagicAttack());
					break;
				case MAGIC_TYPE::HEAL:
					battleMessage_ = unit.name + " の回復魔法！";
					playerStatus_->Heal();
					break;
				case MAGIC_TYPE::BUFF:
					battleMessage_ = unit.name + " の強化魔法！";
					playerStatus_->Heal();
					break;
				case MAGIC_TYPE::DEBUFF:
					battleMessage_ = unit.name + " の弱化魔法！";
					statusEffect_ = STATUS_EFFECT::POISON;
					break;
				default:
					break;
				}
			}
		}
		else
		{
			//敵の行動分岐
			battleMessage_ = unit.name + " の " + unit.skillName + "！";

			//技の名前によって特別な効果を発動させる
			if (unit.skillName == "かいふく" || unit.skillName == "めいそう")
			{
				int healAmount = 50; //回復量（必要に応じて調整してください）
				activeEnemy_->Heal(healAmount);
				battleMessage_ = unit.name + " のHPが " + std::to_string(healAmount) + " 回復した！";
			}
			else if (unit.skillName == "まもる" || unit.skillName == "かまえる" || unit.skillName == "すいへき")
			{
				//将来的に防御力アップやダメージ軽減を実装する用の枠です
				battleMessage_ = unit.name + " は身構えている！";
			}
			else if (unit.skillName == "へびにらみ")
			{
				//将来的にプレイヤーを状態異常（マヒなど）にする用の枠です
				battleMessage_ = "プレイヤーは 石化している！";
			}
			else
			{
				//--- それ以外は通常の攻撃技として処理 ---
				//unit.command (0:通常行動, 1:中技, 2:大技) で威力を変える
				int basePower = activeEnemy_->GetPower();
				int damage = 0;

				if (unit.command == 0) 
				{
					damage = basePower;
				}
				else if (unit.command == 1)
				{
					damage = basePower;
				}
				else if (unit.command == 2) 
				{
					damage = basePower;
				}

				//回避判定
				//計算式：基本回避率 運のステータス×2　　　※運の数値に合わせて調整
				int evasionChance = playerStatus_->astrology_ / 5;

				//バランス崩壊を防ぐための安全装置（最大回避率を90%でストップさせる）
				if (evasionChance > 90) evasionChance = 90;
				int roll = GetRand(99);

				if (roll < evasionChance)
				{
					//回避成功！ダメージ処理はスキップしてメッセージだけ上書き
					battleMessage_ = "攻撃を回避！";
				}
				else
				{
					//回避失敗 通常通りダメージを受ける
					playerStatus_->Damage(damage);
				}
			}
		}

		//倒した時の上書き
		if (activeEnemy_->IsDead ())
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

				statusEffect_ = STATUS_EFFECT::NONE; //状態異常リセット
				return;
			}
			else
			{
				//全Waveクリア！
				battleStep_ = BATTLE_STEP::RESULT;
				statusEffect_ = STATUS_EFFECT::NONE; //状態異常リセット
				return;
			}
		}

		//敵がまだ生きていれば、次のキャラの行動へ
		battleMessage_ = "";
		currentActionIdx_++;
	}
}

void QuestPhase::ProcessStatusEffect(void)
{
	if (statusEffect_ == STATUS_EFFECT::POISON)
	{
		battleMessage_ = actionOrder_[currentActionIdx_].name + " は毒ダメージを受けた！";

		//敵も毒状態になっている場合は、ターンの最後にダメージを受ける（例：1ダメージ）
		activeEnemy_->TakeDamage(1);
	}

	//ターン終了時にフラグを更新
	wasMagicUsedLastTurn_ = magicUsedThisTurn_;
	currentActionIdx_ = 0;
	battleMessage_ = "";

	battleStep_ = BATTLE_STEP::COMMAND_SELECTION;
}

void QuestPhase::DisplayResult(void)
{
	//経験値獲得は ProcessActionLoop内で敵を倒すたびに行うように変更したため、ここは次へ進む処理のみ
	if (ins_.IsTrgDown(KEY_INPUT_RETURN))
	{
		PhaseBase::phaseResult_ = PhaseBase::PHASE_RESULT::NEXT_TURN;
		isFinished_ = true; //フェーズ終了
		playerStatus_->FullHeal(); //HPを全回復
	}
}

void QuestPhase::ProcessPlayerAction()
{
	int commandIndex = static_cast<int>(command_); //enum class を int に変換して操作する
	int maxItems = static_cast<int>(COMMAND::MAX);
	
	//カーソル移動
	Utility::ProcessCommandMenuSelection(commandIndex, maxItems);
	//更新された int の値を、安全に元の enum class 型に戻して代入する
	command_ = static_cast<COMMAND>(commandIndex);

	//決定処理
	if (ins_.IsTrgDown(KEY_INPUT_RETURN))
	{
		if (command_ == COMMAND::MAGIC && wasMagicUsedLastTurn_)return; //魔法の連続使用制限

		if (command_ == COMMAND::MAGIC) magicUsedThisTurn_ = true;
		else magicUsedThisTurn_ = false;

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
		case QuestPhase::COMMAND::MAX:
			break;
		default:
			break;
		}

		//次のステップへ
		battleStep_ = BATTLE_STEP::COMMAND_SUB_SELECTION;
		//battleStep_ = BATTLE_STEP::DETERMINE;
	}
}

void QuestPhase::ProcessPlayerSubAction(void)
{
	// 選択肢の数を、埋め込まれた配列のサイズから自動取得！
	int maxSubItems = static_cast<int>(subActionMessages_.size());
	if (maxSubItems == 0) return; // 念のため

	//カーソル移動処理
	Utility::ProcessCommandMenuSelection(subMenuCursor_, maxSubItems);

	//--- 決定処理 ---
	if (ins_.IsTrgDown(KEY_INPUT_RETURN))
	{
		//【重要】ここで「何番のサブメニューを選んだか」を記憶しておく！
		//例：chosenSubMenuIdx_ = subMenuCursor_; 
		//この数値を、後の DetermineActionOrder や ActionUnit に引き渡します。

		//行動決定へ進む
		battleStep_ = BATTLE_STEP::DETERMINE;
	}

	if (ins_.IsTrgDown(KEY_INPUT_TAB))
	{
		battleStep_ = BATTLE_STEP::COMMAND_SELECTION;
	}
}

void QuestPhase::DrawCommandSelection(void)
{
	Utility::DrawCommandMenu(0,200,{"攻撃","魔法","アイテム"},static_cast<int>(command_));
}