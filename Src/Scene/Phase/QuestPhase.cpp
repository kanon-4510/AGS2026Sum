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
extern Enemy* SpawnRushEnemy(int stage); //ラッシュ専用の敵生成関数

//コンストラクタ
QuestPhase::QuestPhase(PlayerStatus* playerStatus, GameScene& gameScene, bool isHellQuest)
	: gameScene_(gameScene)
	, playerStatus_(playerStatus)
	, command_(COMMAND::ATTACK)
	, battleStep_(BATTLE_STEP::DIFFICULTY_SELECTION)
	, currentWave_(1) //Waveの初期化
	, isHellQuest_(false)
{
	//クエスト開始時は一旦通常の敵を生成しておく
	activeEnemy_ = SpawnEnemyByTurn(gameScene_.GetTurn());

	//ここで難易度メニューを動的に作成
	difficultyMenu_ = { "優しい", "普通", "難しい" };

	//13ターン目以降 かつ まだ一度も挑んでいないなら
	if (gameScene_.GetTurn() >= 13 && !playerStatus_->hasChallengedHellQuest_)
	{
		difficultyMenu_.push_back("エクストラ");
	}
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
	//ProcessTutorial();

	//ターン管理関数
	ManageTurn();
}

//描画処理
void QuestPhase::Draw(void)
{
	DrawString(0, 0, "Scene : Quest", 0xFFFFFF);

	//DrawTutorial();

	if(battleStep_ != BATTLE_STEP::DIFFICULTY_SELECTION
		&& battleStep_ != BATTLE_STEP::RESULT)
	{
		activeEnemy_->Draw(); //敵の描画

		int maxWaves = isHellQuest_ ? 5 : MAX_WAVES;
		DrawFormatString(0, 80, 0xFFFF00, "現在のターン %d", battleTurn_);//連戦（Wave）の表示
		DrawFormatString(0, 100, 0xFFFF00, "【 BATTLE %d / %d 】", currentWave_, maxWaves);//連戦（Wave）の表示
		DrawFormatString(PLAYER_HP_MSG_X, PLAYER_HP_MSG_Y, 0xFFFFFF, "プレイヤーのHP %d / %d", playerStatus_->hp_, playerStatus_->GetMaxHp());

		//敵のHPを activeEnemy_ から直接もらう
		if (activeEnemy_ != nullptr && !activeEnemy_->IsDead())
		{
			DrawFormatString(ENEMY_HP_MSG_X, ENEMY_HP_MSG_Y, 0xFFFFFF, "%sのHP %d", activeEnemy_->GetName().c_str(), activeEnemy_->GetCurrentHp());
			//画像を表示する場合はここに activeEnemy_->Draw(); を追加
		}
	}

	if (battleStep_ == BATTLE_STEP::DIFFICULTY_SELECTION)
	{
		Utility::DrawCommandMenu(DIFFICULTY_MSG_X, DIFFICULTY_MSG_Y,difficultyMenu_, difficultyCursor_);
	}
	else if (battleStep_ == BATTLE_STEP::COMMAND_SELECTION) //難易度選択中はコマンドやHPを表示しない
	{
		DrawCommandSelection();

		//行動の結果メッセージがあれば表示
		if (battleMessage_ != "")
		{
			DrawFormatString(BATTLE_MSG_X, BATTLE_MSG_Y, 0xFF0000, battleMessage_.c_str());
			DrawString(NEXT_MSG_X, NEXT_MSG_Y, "Enterキーで次へ", 0xFF0000);
		}
	}
	else if (battleStep_ == BATTLE_STEP::COMMAND_SUB_SELECTION)
	{
		//DrawFormatString(0, 40, 0xFFFFFFF, "%d", subMenuCursor_);

		//サブコマンドの描画（例：魔法の種類やアイテムの選択肢など）
		Utility::DrawCommandMenu(COMMAND_MSG_X, COMMAND_MSG_Y, subActionMessages_, subMenuCursor_);
	}
	
	if (activeEnemy_ != nullptr && !activeEnemy_->IsDead())
	{
		DrawFormatString(BATTLE_MSG_X, BATTLE_MSG_Y, 0xFF0000, battleMessage_.c_str());
		DrawString(NEXT_MSG_X, NEXT_MSG_Y, "Enterキーで次へ", 0xFF0000);
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
	//選択肢の数を、配列のサイズから自動取得
	int maxDifficulty = static_cast<int>(difficultyMenu_.size());
	Utility::ProcessCommandMenuSelection(difficultyCursor_, maxDifficulty);

	//決定処理
	if (ins_.IsTrgDown(KEY_INPUT_RETURN))
	{
		//選んだメニューの「文字列」で分岐させる
		if (difficultyMenu_[difficultyCursor_] == "エクストラ")
		{
			isHellQuest_ = true;
			playerStatus_->hasChallengedHellQuest_ = true;//二度と選べないようにフラグを回収

			//通常敵のメモリを解放し、5連戦用の1体目とすげ替える！
			delete activeEnemy_;
			activeEnemy_ = SpawnRushEnemy(0);
		}
		else
		{
			isHellQuest_ = false;
			//通常の難易度として enum に保存する（既存の処理を維持）
			difficulty_ = static_cast<DIFFICULTY>(difficultyCursor_);
		}

		battleStep_ = BATTLE_STEP::COMMAND_SELECTION;
	}
	else if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_TAB)) {
		//キャンセルキーが押されたらフェーズを終了する
		PhaseBase::phaseResult_ = PhaseBase::PHASE_RESULT::CANCEL; //コマンド選択に戻る
		isFinished_ = true;
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
		if (subMenuCursor_ == 0) 
		{
			actionOrder_.back().skillName = "単体攻撃";
		}
		else 
		{
			actionOrder_.back().skillName = "全体攻撃";
		}
	}
	// 魔法攻撃（1）の場合
	else if (command_ == COMMAND::MAGIC)
	{
		// サブメニューの選択肢に応じて魔法の性質を分ける！
		if (subMenuCursor_ == static_cast<int>(MAGIC_TYPE::MAGIC_ATTACK))
		{
			actionOrder_.back().magicType = MAGIC_TYPE::MAGIC_ATTACK; //攻撃魔法
		}
		else if (subMenuCursor_ == static_cast<int>(MAGIC_TYPE::HEAL)) 
		{
			actionOrder_.back().magicType = MAGIC_TYPE::HEAL;         //回復魔法
		}
		else if (subMenuCursor_ == static_cast<int>(MAGIC_TYPE::BUFF)) 
		{
			actionOrder_.back().magicType = MAGIC_TYPE::BUFF;         //強化魔法
		}
		else if (subMenuCursor_ == static_cast<int>(MAGIC_TYPE::DEBUFF)) 
		{
			actionOrder_.back().magicType = MAGIC_TYPE::DEBUFF;       //弱化魔法
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

	//敵が死んでいる場合はスキップ（プレイヤーの行動は常に処理する）
	if (!unit.isPlayer && (activeEnemy_ == nullptr || activeEnemy_->IsDead()))
	{
		currentActionIdx_++;
		return;
	}

	//①ダメージとメッセージの処理
	if (battleMessage_ == "")
	{
		if (unit.isPlayer)
		{
			if (command_ == COMMAND::ATTACK) 
			{
				battleMessage_ = unit.name + " の攻撃！";
				//会心判定
				//計算式：武術のステータス÷5
				int criticalChance = playerStatus_->martialArts_ / 5;
				int roll = GetRand(99);
				if (roll < criticalChance)
				{
					battleMessage_ = "クリティカルヒット！";
					activeEnemy_->Damage(playerStatus_->Attack()*2);
				}
				else
				{
					activeEnemy_->Damage(playerStatus_->Attack());
				}
			}
			else if (command_ == COMMAND::MAGIC) 
			{
				switch (unit.magicType)
				{
				case MAGIC_TYPE::MAGIC_ATTACK:
					battleMessage_ = unit.name + " の魔法攻撃！";
					activeEnemy_->Damage(playerStatus_->MagicAttack());
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
				//計算式：占星術のステータス÷5　　※運の数値に合わせて調整
				int evasionChance = playerStatus_->astrology_ / 5;

				//バランス崩壊を防ぐための安全装置（最大回避率を90%でストップさせる）
				if (evasionChance > 90) evasionChance = 90;
				int roll = GetRand(99);

				if (roll < evasionChance)
				{
					//回避成功！ダメージ処理はスキップしてメッセージだけ上書き
					battleMessage_ = "攻撃を回避！";
				}
				else if(!activeEnemy_->IsDead())
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

		battleMessage_ = "";

		//もし敵を倒していたら
		if (activeEnemy_->IsDead())
		{
			//倒した敵から経験値を獲得(ゲキムズ以外)
			if (!isHellQuest_) playerStatus_->GetExp(activeEnemy_->GetExp());

			//今の敵を消去
			delete activeEnemy_;
			activeEnemy_ = nullptr;

			int maxWaves = isHellQuest_ ? 5 : MAX_WAVES;//最大Wave数を動的に切り替え

			//連戦チェック
			if (currentWave_ < maxWaves)
			{
				currentWave_++; //次のWaveへ
				//次の敵の呼び出し分け
				if (isHellQuest_)activeEnemy_ = SpawnRushEnemy(currentWave_ - 1); //0スタートの配列に合わせる
				else activeEnemy_ = SpawnEnemyByTurn(gameScene_.GetTurn()); // 次の敵を生成
				
				actionOrder_.clear(); // 行動リストを綺麗に掃除
				currentActionIdx_ = 0; // インデックスも0に戻す
				statusEffect_ = STATUS_EFFECT::NONE; //状態異常リセット
				battleMessage_ = ""; // メッセージをリセットして次のターンへ！
				return;
			}
			else
			{
				//全Waveクリア！

				//激ムズクエストは莫大な経験値を付与
				if (isHellQuest_)playerStatus_->GetExp(100);
				wasMagicUsedLastTurn_ = magicUsedThisTurn_;
				battleStep_ = BATTLE_STEP::RESULT;
				statusEffect_ = STATUS_EFFECT::NONE; //状態異常リセット
				return;
			}
		}

		//敵がまだ生きていれば、次のキャラの行動へ
		currentActionIdx_++;
	}
}

void QuestPhase::ProcessStatusEffect(void)
{
	if (statusEffect_ == STATUS_EFFECT::POISON)
	{
		battleMessage_ = actionOrder_[currentActionIdx_].name + " は毒ダメージを受けた！";

		//敵も毒状態になっている場合は、ターンの最後にダメージを受ける（例：1ダメージ）
		activeEnemy_->Damage(1);
	}

	//ターン終了時にフラグを更新
	wasMagicUsedLastTurn_ = magicUsedThisTurn_;
	currentActionIdx_ = 0;

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
	//選択肢の数を、埋め込まれた配列のサイズから自動取得！
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
	Utility::DrawCommandMenu(COMMAND_MSG_X, COMMAND_MSG_Y,{"攻撃","魔法"},static_cast<int>(command_));
}

void QuestPhase::ProcessTutorial(void)
{
	//チュートリアルの内容をここに書く
	if(gameScene_.GetTurn() == 1)
	{
		// 最初のクエスト（turn == 0）以外は通常の自由戦闘なので何もしない
		if (gameScene_.GetTurn() != 1) return;

		// 【バトル内の、最初のコマンド入力フェーズのとき】
		// ※仮にコマンド選択中のステップ名を BATTLE_STEP::COMMAND_SELECTION とします
		if (battleStep_ == BATTLE_STEP::COMMAND_SELECTION
			&& battleTurn_ == 1)
		{
			// メインコマンドは「攻撃（0）」に強制固定
			command_ = COMMAND::ATTACK;

			// もし攻撃のサブメニュー（単体・全体）まで開いているなら、
			// 最初の選択肢「単体攻撃（0）」にカーソルを強制ロック！
			if (battleStep_ == BATTLE_STEP::COMMAND_SUB_SELECTION)
			{
				subMenuCursor_ = 0;
				// このあとの通常のカーソル移動キーの入力を無視するフラグ（returnなど）に繋げます
			}
		}
		
	}
}
void QuestPhase::DrawTutorial(void)
{
	tutorialMessage_ = "";

	if (battleStep_ == BATTLE_STEP::DIFFICULTY_SELECTION)
	{
		tutorialMessage_ = "チュートリアル\nここでは難易度を選択できます\n最初は優しいを選んで下さい\nEnterキーを押してみましょう！";
	}
	if (battleStep_ != BATTLE_STEP::DIFFICULTY_SELECTION
		&&battleTurn_ == 1)
	{
		tutorialMessage_ = "チュートリアル\n最初のターンは攻撃コマンドしか選べません！\n攻撃を選んでEnterキーを押してみましょう！";
	}
	else if(battleStep_ != BATTLE_STEP::DIFFICULTY_SELECTION
		&& battleTurn_ == 2)
	{
		tutorialMessage_ = "チュートリアル\n次は魔法で攻撃をしましょう\n魔法を選んでEnterキーを押してみましょう！";
	}
	else if(battleStep_ != BATTLE_STEP::DIFFICULTY_SELECTION
		&& battleTurn_ == 3)
	{
		tutorialMessage_ = "チュートリアル\n魔法を使うと１ターンの休憩が必要です\n攻撃を選んでEnterキーを押してみましょう！";
	}
	else if (battleStep_ != BATTLE_STEP::DIFFICULTY_SELECTION
		&& battleTurn_ == 4)
	{
		tutorialMessage_ = "チュートリアル\nあとは好きなように戦いましょう";
	}


	//チュートリアルの内容をここに書く
	//例：最初のターンだけ特別な説明を表示するなど
	if(gameScene_.GetTurn() == 1)
	{
		DrawFormatString(0, 900, 0xFFFFFF, tutorialMessage_.c_str());
	}
}
