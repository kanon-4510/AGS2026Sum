#include <DxLib.h>
#include <algorithm>
#include "../../Application.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/InputManager.h"
#include "../../Utility/AsoUtility.h"
#include "../GameScene.h"
#include "../../Object/Enemy.h"
#include "QuestPhase.h"

//外部にある敵生成関数（Enemy.cppなどに実装している想定）
extern Enemy* SpawnEnemyByTurn(int currentTurn);
extern Enemy* SpawnRushEnemy(int stage); //ラッシュ専用の敵生成関数

//コンストラクタ
QuestPhase::QuestPhase(PlayerStatus* playerStatus, GameScene& gameScene, bool isHellQuest)
	: gameScene_(gameScene)
	, playerStatus_(playerStatus)
	, isHellQuest_(isHellQuest)
	, bgImageHandle_(-1)
{
	//クエスト開始時は一旦通常の敵を生成しておく
	activeEnemy_ = SpawnEnemyByTurn(gameScene_.GetTurn());
	statusEffect_ = STATUS_EFFECT::NONE;	//状態異常の初期化
	battleStep_ = BATTLE_STEP::DIFFICULTY_SELECTION;
	locationMenu_ = { "平原","魔法の森","岩山の道場","魔大陸","壊れた聖堂","古代遺跡","星の丘" };	//ここで難易度メニューを動的に作成
	selectableLocations_ = { QUEST_LOCATION::PLAINS,QUEST_LOCATION::FOREST,QUEST_LOCATION::SHRINE,QUEST_LOCATION::CONTINENT,QUEST_LOCATION::CATHEDRAL,QUEST_LOCATION::RUINS,QUEST_LOCATION::HILL };

	//13ターン目以降 かつ まだ一度も挑んでいないなら
	if (gameScene_.GetTurn() >= 13 && !playerStatus_->hasChallengedHellQuest_)
	{
		locationMenu_.push_back("エクストラ");
		selectableLocations_.push_back(QUEST_LOCATION::EXTRA);
	}
}

//デストラクタ
QuestPhase::~QuestPhase(void)
{
	if (bgImageHandle_ != -1)DeleteGraph(bgImageHandle_);

	if (activeEnemy_ != nullptr)
	{
		delete activeEnemy_;
		activeEnemy_ = nullptr;
	}
}

//更新処理
void QuestPhase::Update(void)
{
	ProcessTutorial();

	//ターン管理関数
	ManageTurn();

	if (activeEnemy_ != nullptr)
	{
		activeEnemy_->Update(); //敵の更新処理
	}
}

//描画処理
void QuestPhase::Draw(void)
{
	if (bgImageHandle_ != -1)DrawGraph(0, 0, bgImageHandle_, TRUE);//背景を描画する

	DrawString(0, 0, "Scene : Quest", 0xFFFFFF);
	DrawTutorial();

	if (battleStep_ != BATTLE_STEP::DIFFICULTY_SELECTION && battleStep_ != BATTLE_STEP::RESULT)
	{
		activeEnemy_->Draw(); //敵の描画

		int maxWaves = isHellQuest_ ? 5 : MAX_WAVES;
		DrawFormatString(0, 100, 0xFFFF00, "【 BATTLE %d / %d 】", currentWave_, maxWaves);//連戦（Wave）の表示
		DrawFormatString(PLAYER_HP_MSG_X, PLAYER_HP_MSG_Y, 0xFFFFFF, "%sのHP %d / %d", playerStatus_->GetName().c_str(), playerStatus_->hp_, playerStatus_->GetMaxHp());
		playerStatus_->DrawQuestImages();	//プレイヤーの画像を描画

		//敵のHPを activeEnemy_ から直接もらう
		if (activeEnemy_ != nullptr && !activeEnemy_->IsDead())
		{
			DrawFormatString(ENEMY_HP_MSG_X, ENEMY_HP_MSG_Y, 0xFFFFFF, "%sのHP %d", activeEnemy_->GetName().c_str(), activeEnemy_->GetCurrentHp());
			//画像を表示する場合はここに activeEnemy_->Draw(); を追加
		}
	}

	if (battleStep_ == BATTLE_STEP::DIFFICULTY_SELECTION)//難易度選択中はコマンドやHPを表示しない
	{
		DrawString(DIFFICULTY_MSG_X, DIFFICULTY_MSG_Y - 30, "どこに行く？", 0xFFFFFF);
		Utility::DrawCommandMenu(DIFFICULTY_MSG_X, DIFFICULTY_MSG_Y, locationMenu_, difficultyCursor_);
	}
	else if (battleStep_ == BATTLE_STEP::COMMAND_SELECTION)
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
		//サブコマンドの描画（例：魔法の種類やアイテムの選択肢など）
		Utility::DrawCommandMenu(COMMAND_MSG_X, COMMAND_MSG_Y, subActionMessages_, subMenuCursor_);
	}
	else if (battleStep_ == BATTLE_STEP::MAGIC_SELECTION)
	{
		//魔法の種類の描画
		Utility::DrawCommandMenu(COMMAND_MSG_X, COMMAND_MSG_Y, magicTypeMessages_, magicMenuCursor_);
	}
	else if (battleStep_ == BATTLE_STEP::RESULT)
	{
		DrawString(0, 150, "遠征クリア！ 経験値を獲得した！", 0xFFFFFF);
		DrawString(0, 170, "レベルが上がった！", 0xFFFFFF);
		DrawString(0, 190, locationRewardMsg_.c_str(), 0xFFFF00);
		DrawString(0, 210, "基礎ステータスが上がった！", 0xFFFFFF);
		DrawString(0, 230, "Enterキーで次へ", 0xFFFFFF);
	}

	if (activeEnemy_ != nullptr && !activeEnemy_->IsDead())
	{
		DrawFormatString(BATTLE_MSG_X, BATTLE_MSG_Y, 0xFF0000, battleMessage_.c_str());
		DrawString(NEXT_MSG_X, NEXT_MSG_Y, "Enterキーで次へ", 0xFF0000);
	}
}

bool QuestPhase::IsFinished() const
{
	return isFinished_;
}

void QuestPhase::ProcessDifficulty(void)
{
	//選択肢の数を、配列のサイズから自動取得
	int maxDifficulty = static_cast<int>(locationMenu_.size());
	Utility::ProcessCommandMenuSelection(difficultyCursor_, maxDifficulty);

	//決定処理
	if (ins_.IsTrgDown(KEY_INPUT_RETURN) ||
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
	{
		//選んだメニューの「文字列」で分岐させる
		if (locationMenu_[difficultyCursor_] == "エクストラ")
		{
			isHellQuest_ = true;
			playerStatus_->hasChallengedHellQuest_ = true;	//二度と選べないようにフラグを回収
			location_ = QUEST_LOCATION::EXTRA;			//場所を魔大陸に設定

			//通常敵のメモリを解放し5連戦用の1体目とすげ替える
			delete activeEnemy_;
			activeEnemy_ = SpawnRushEnemy(0);
		}
		else
		{
			isHellQuest_ = false;
			//通常の難易度としてenumに保存する
			location_ = selectableLocations_[difficultyCursor_];
		}
		switch (location_)
		{
		case QUEST_LOCATION::PLAINS: bgImageHandle_ = LoadGraph("Data/Image/Stage/Stage_1.png"); break;
		case QUEST_LOCATION::FOREST: bgImageHandle_ = LoadGraph("Data/Image/Stage/Stage_2.png"); break;
		case QUEST_LOCATION::SHRINE: bgImageHandle_ = LoadGraph("Data/Image/Stage/Stage_3.png"); break;
		case QUEST_LOCATION::CONTINENT: bgImageHandle_ = LoadGraph("Data/Image/Stage/Stage_4.png"); break;
		case QUEST_LOCATION::CATHEDRAL: bgImageHandle_ = LoadGraph("Data/Image/Stage/Stage_5.png"); break;
		case QUEST_LOCATION::RUINS: bgImageHandle_ = LoadGraph("Data/Image/Stage/Stage_6.png"); break;
		case QUEST_LOCATION::HILL: bgImageHandle_ = LoadGraph("Data/Image/Stage/Stage_7.png"); break;
		case QUEST_LOCATION::EXTRA: bgImageHandle_ = LoadGraph("Data/Image/Stage/Stage_8.png"); break;
		}

		battleStep_ = BATTLE_STEP::COMMAND_SELECTION;
	}
	else if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_TAB) ||
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT)) {
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
	case QuestPhase::BATTLE_STEP::MAGIC_SELECTION:
		//DETERMINEに進むかCOMMAND_SUB_SELECTIONに戻るか
		MagicSelection();
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
	actionOrder_.push_back({ playerStatus_->GetName().c_str(), playerStatus_->speed_, true, 0, (int)command_, 0 });
	if (command_ == COMMAND::ATTACK)
	{
		actionOrder_.back().skillName = "単体攻撃";
	}
	// 魔法攻撃（1）の場合
	if (command_ == COMMAND::MAGIC)
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
	}

	//敵の追加（DecideActionでランダムに行動を決定）
	if (activeEnemy_ != nullptr && !activeEnemy_->IsDead())
	{
		EnemyActionInfo eAction = activeEnemy_->DecideAction();
		actionOrder_.push_back({ eAction.name, eAction.speed, false, 0, eAction.actionType, 0 });

		actionOrder_.back().skillName = eAction.skillName;
	}

	//ソート（スピード順）
	std::sort(actionOrder_.begin(), actionOrder_.end(), [](const ActionUnit& a, const ActionUnit& b) {return a.speed > b.speed; });

	currentActionIdx_ = 0;
	battleStep_ = BATTLE_STEP::ACTION_LOOP;
}

void QuestPhase::ProcessActionLoop(void)
{
	if (currentActionIdx_ >= actionOrder_.size())
	{
		//ターン終了時にフラグを更新
		battleStep_ = BATTLE_STEP::STATUS_EFFECT;
		battleTurn_++;
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
			//---状態異常の行動前チェック---
			bool skipAction = false;
			bool isMiss = false;

			//凍結(行動不可)
			if (statusEffect_ == STATUS_EFFECT::FREEZE)
			{
				if (GetRand(99) < 60) //60%で凍ったまま行動不可
				{
					battleMessage_ = "体が凍りついて動けない";
					skipAction = true;
				}
				else //確率を乗り越えたら解除してそのまま行動
				{
					battleMessage_ = "氷が溶けてうごけるようになった!";
					statusEffect_ = STATUS_EFFECT::NONE;
				}
			}
			//閃光(命中低下)
			if (!skipAction && statusEffect_ == STATUS_EFFECT::FLASH)
			{
				if (command_ == COMMAND::ATTACK || (command_ == COMMAND::MAGIC && (unit.magicType == MAGIC_TYPE::MAGIC_ATTACK || unit.magicType == MAGIC_TYPE::DEBUFF)))
				{
					if (GetRand(99) < 50) isMiss = true; //50%で外れる
				}
			}

			//実際の行動処理
			if (skipAction)
			{
				//メッセージは設定済みなので何もしない
			}
			else if (isMiss)
			{
				battleMessage_ += "目が眩んで攻撃が外れた";
			}
			else
			{
				if (command_ == COMMAND::ATTACK)
				{
					battleMessage_ += unit.name + "の攻撃！";
					//会心判定
					//計算式：武術のステータス÷5
					int criticalChance = playerStatus_->martialArts_ / 5;
					int roll = GetRand(99);
					if (roll < criticalChance)
					{
						battleMessage_ += "クリティカルヒット！";
						activeEnemy_->ChangeAnim(ANIM_DAMAGE);
						activeEnemy_->Damage(playerStatus_->Attack() * 3);
					}
					else
					{
						activeEnemy_->ChangeAnim(ANIM_DAMAGE);
						activeEnemy_->Damage(playerStatus_->Attack());
					}
				}
				else if (command_ == COMMAND::MAGIC)
				{
					switch (unit.magicType)
					{
					case MAGIC_TYPE::MAGIC_ATTACK:
						battleMessage_ += unit.name + "の攻撃魔法！";
						activeEnemy_->ChangeAnim(ANIM_DAMAGE);
						activeEnemy_->Damage(playerStatus_->MagicAttack());
						break;
					case MAGIC_TYPE::HEAL:
						battleMessage_ += unit.name + "の回復魔法！";
						playerStatus_->Heal();
						break;
					case MAGIC_TYPE::BUFF:
						battleMessage_ += unit.name + "の状態回復魔法！";
						statusEffect_ = STATUS_EFFECT::NONE;
						statusTurns_ = 4;
						break;
					case MAGIC_TYPE::DEBUFF:
						if (chosenMagicIdx_ == 0)
						{
							battleMessage_ += unit.name + "のポイズン！";
							enemyStatusEffect_ = STATUS_EFFECT::POISON;
						}
						else if (chosenMagicIdx_ == 1)
						{
							battleMessage_ += unit.name + "のフリーズ！";
							enemyStatusEffect_ = STATUS_EFFECT::FREEZE;
						}
						else if (chosenMagicIdx_ == 2)
						{
							battleMessage_ += unit.name + "のフラッシュ！";
							enemyStatusEffect_ = STATUS_EFFECT::FLASH;
						}
						else if (chosenMagicIdx_ == 3)
						{
							battleMessage_ += unit.name + "のカース！";
							enemyStatusEffect_ = STATUS_EFFECT::CURSE;
						}
						break;
					default:
						break;
					}
				}
			}
		}
		else
		{
			//行動前チェック
			bool skipAction = false;
			bool isMiss = false;
			activeEnemy_->ResetGuard();

			//凍結(行動不可)
			if (enemyStatusEffect_ == STATUS_EFFECT::FREEZE)
			{
				if (GetRand(99) < 60) //60%で凍ったまま行動不可
				{
					battleMessage_ += unit.name + "は凍りついて動けない";
					skipAction = true;
				}
				else //確率を乗り越えたら解除してそのまま行動
				{
					battleMessage_ += unit.name + "は氷が溶けてうごけるようになった";
					enemyStatusEffect_ = STATUS_EFFECT::NONE;
				}
			}
			//閃光(命中低下)
			if (!skipAction && enemyStatusEffect_ == STATUS_EFFECT::FLASH)
			{
				if (GetRand(99) < 50) isMiss = true; //50%で外れる
			}
			//実際の行動処理
			if (skipAction)
			{
				//メッセージは設定済みなので何もしない
			}
			else if (isMiss)
			{
				battleMessage_ += unit.name + "は目が眩んで攻撃が外れた";
			}
			else
			{
				EnemyActionInfo eAction = activeEnemy_->DecideAction();
				unit.skillName = eAction.skillName;

				//敵の行動分岐
				battleMessage_ += unit.name + "の" + unit.skillName + "！";

				//技の名前によって特別な効果を発動させる
				if (unit.skillName == "大地の恵み" || unit.skillName == "電力チャージ"
					|| unit.skillName == "自己再生" || unit.skillName == "回復")
				{
					//Power分回復
					int healAmount = activeEnemy_->GetPower3();
					activeEnemy_->Heal(healAmount);
					battleMessage_ += unit.name + "の体力が" + std::to_string(healAmount) + "回復した";
				}
				else if (unit.skillName == "まもる" || unit.skillName == "守る"
					|| unit.skillName == "守りの構え" || unit.skillName == "受流しの構え")
				{
					//Power分ダメージ軽減
					activeEnemy_->SetGuard(activeEnemy_->GetPower3());
					battleMessage_ += unit.name + "は身構えている";
				}
				else if (unit.skillName == "蛇にらみ" || unit.skillName == "石化の魔眼"
					|| unit.skillName == "金縛り" || unit.skillName == "発狂")
				{
					//プレイヤーを凍結状態にする
					int damage = activeEnemy_->GetPower3();
					playerStatus_->Damage(damage);
					if (statusEffect_ == STATUS_EFFECT::NONE)
					{
						statusEffect_ = STATUS_EFFECT::FREEZE;
						battleMessage_ += playerStatus_->GetName() + "は凍りついた";
					}
					else battleMessage_ += "しかしうまく決まらなかった";
				}
				else if (unit.skillName == "放熱" || unit.skillName == "ふきつなかぜ"
					|| unit.skillName == "破魔空間" || unit.skillName == "火炎放射" || unit.skillName == "沈黙の呪い")
				{
					//プレイヤーを沈黙状態にする
					int damage = activeEnemy_->GetPower3();
					playerStatus_->Damage(damage);
					if (statusEffect_ == STATUS_EFFECT::NONE)
					{
						statusEffect_ = STATUS_EFFECT::SILENCE;
						battleMessage_ += playerStatus_->GetName() + "は沈黙になった";
					}
					else battleMessage_ += "しかしうまく決まらなかった";
				}
				else if (unit.skillName == "どくのや" || unit.skillName == "毒の粉"
					|| unit.skillName == "毒牙" || unit.skillName == "かみつく" || unit.skillName == "毒パンチ")
				{
					//プレイヤーを毒状態にする
					int damage = activeEnemy_->GetPower2();
					playerStatus_->Damage(damage);
					if (statusEffect_ == STATUS_EFFECT::NONE)
					{
						statusEffect_ = STATUS_EFFECT::POISON;
						battleMessage_ += playerStatus_->GetName() + "は毒状態になった";
					}
					else battleMessage_ += "しかしうまく決まらなかった";
				}
				else if (unit.skillName == "呪い" || unit.skillName == "呪われた包丁"
					|| unit.skillName == "血槍" || unit.skillName == "鬼火" || unit.skillName == "切断")
				{
					//プレイヤーを呪い状態にする
					int damage = activeEnemy_->GetPower2();
					playerStatus_->Damage(damage);
					if (statusEffect_ == STATUS_EFFECT::NONE)
					{
						statusEffect_ = STATUS_EFFECT::CURSE;
						battleMessage_ += playerStatus_->GetName() + "は呪われた";
					}
					else battleMessage_ += "しかしうまく決まらなかった";
				}
				else if (unit.skillName == "ばくはつ" || unit.skillName == "電撃斬"
					|| unit.skillName == "雷連斬" || unit.skillName == "エレキビーム" || unit.skillName == "斬撃")
				{
					//プレイヤーを閃光状態にする
					int damage = activeEnemy_->GetPower2();
					playerStatus_->Damage(damage);
					if (statusEffect_ == STATUS_EFFECT::NONE)
					{
						statusEffect_ = STATUS_EFFECT::FLASH;
						battleMessage_ += playerStatus_->GetName() + "は目がくらんだ";
					}
					else battleMessage_ += "しかしうまく決まらなかった";
				}
				else
				{
					//---それ以外は通常の攻撃技として処理---
					//unit.command (0:通常, 1:中技, 2:大技) で威力を変える
					int damage = 0;

					if (unit.command == ANIM_ACT_1)
					{
						damage = activeEnemy_->GetPower1();

						activeEnemy_->ChangeAnim(unit.command);
					}
					else if (unit.command == ANIM_ACT_2)
					{
						damage = activeEnemy_->GetPower2();
						activeEnemy_->ChangeAnim(unit.command);
					}
					else if (unit.command == ANIM_ACT_3)
					{
						damage = activeEnemy_->GetPower3();
						activeEnemy_->ChangeAnim(unit.command);
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
						battleMessage_ = "攻撃を回避した！";
					}
					else if (!activeEnemy_->IsDead())
					{
						//回避失敗 通常通りダメージを受ける
						playerStatus_->Damage(damage);
					}
				}
			}
		}

		//倒した時の上書き
		if (activeEnemy_->IsDead())
		{
			activeEnemy_->ChangeAnim(ANIM_DEAD);
			battleMessage_ = activeEnemy_->GetName() + "を倒した！";
		}
	}

	//②Enterキー待ちと、連戦(Wave)の処理
	if (ins_.IsTrgDown(KEY_INPUT_RETURN) ||
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
	{
		battleMessage_ = "";

		//もし敵を倒していたら
		if (activeEnemy_->IsDead())
		{
			CheckEnemyDeath();
			return; //敵を倒した場合は次のターン
		}

		//敵がまだ生きていれば、次のキャラの行動へ
		currentActionIdx_++;
	}
}

void QuestPhase::ProcessStatusEffect(void)
{
	//まだメッセージが設定されていなければ、状態異常の処理を行う
	if (battleMessage_ == "")
	{
		bool hasEffectMessage = false; //メッセージを出すべき効果があったか

		//定数ダメージ(poison)
		if (enemyStatusEffect_ == STATUS_EFFECT::POISON)
		{
			//ターンの最後にダメージを受ける（例：1ダメージ）
			battleMessage_ = activeEnemy_->GetName() + "は毒のダメージを受けた";
			activeEnemy_->ChangeAnim(ANIM_DAMAGE);
			activeEnemy_->Damage(1);
			hasEffectMessage = true;
		}
		else if (enemyStatusEffect_ == STATUS_EFFECT::CURSE)
		{
			enemyCurs_--;
			if (enemyCurs_ <= 0)
			{
				battleMessage_ = activeEnemy_->GetName()+"にかかった呪いが発動した……";
				//activeEnemy_->currentHp_=1; //殺す
			}
			else
			{
				battleMessage_ = "呪いまで あと " + std::to_string(enemyCurs_) + "ターン";
			}
			hasEffectMessage = true;
		}
		//定数ダメージ(poison)
		if (statusEffect_ == STATUS_EFFECT::POISON)
		{
			//ターンの最後にダメージを受ける（例：1ダメージ）
			if (enemyStatusEffect_ == STATUS_EFFECT::POISON)battleMessage_ = "お互いに毒のダメージを受けた";
			else battleMessage_ = playerStatus_->GetName() + "は毒のダメージを受けた";
			playerStatus_->Damage(1);
			hasEffectMessage = true;
		}
		//ターン経過で即死(curse)
		else if (statusEffect_ == STATUS_EFFECT::CURSE)
		{
			statusTurns_--; //残りターンを減らす
			if (statusTurns_ <= 0)
			{
				battleMessage_ = "呪いが発動した……";
				playerStatus_->hp_ = 1; //HPを1にする
			}
			else
			{
				battleMessage_ = "呪いまで あと " + std::to_string(statusTurns_) + "ターン";
			}
			hasEffectMessage = true;
		}
		//魔法使用不可ターン経過で治癒(silence)
		else if (statusEffect_ == STATUS_EFFECT::SILENCE)
		{
			statusTurns_--; //残りターンを減らす
			if (statusTurns_ <= 0)
			{
				battleMessage_ = "沈黙が解けた ";
				statusTurns_ = 4;
				statusEffect_ = STATUS_EFFECT::NONE; //状態異常を解除
				hasEffectMessage = true;
			}
		}

		//何も状態異常がない、またはメッセージが発生しなかった場合は、すぐに次のターンの準備へ
		if (!hasEffectMessage)
		{
			wasMagicUsedLastTurn_ = magicUsedThisTurn_;
			currentActionIdx_ = 0;
			battleStep_ = BATTLE_STEP::COMMAND_SELECTION;
			return;
		}
	}

	//ここに来るのは、battleMessage_ に何かしらの文章が入っている場合
	//Enterキー待ちをしてから次のターン（またはゲームオーバー）へ進む
	if (ins_.IsTrgDown(KEY_INPUT_RETURN) ||
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
	{
		//もし敵を倒していたら
		if (activeEnemy_->IsDead())
		{
			CheckEnemyDeath();
			return; //敵を倒した場合は次のターン
		}

		battleMessage_ = ""; //メッセージをリセット
		wasMagicUsedLastTurn_ = magicUsedThisTurn_;
		currentActionIdx_ = 0;
		battleStep_ = BATTLE_STEP::COMMAND_SELECTION;
	}
}

void QuestPhase::CheckEnemyDeath(void)
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
		else activeEnemy_ = SpawnEnemyByTurn(gameScene_.GetTurn()); //次の敵を生成

		actionOrder_.clear(); //行動リストを綺麗に掃除
		currentActionIdx_ = 0;//インデックスも0に戻す
		battleMessage_ = "";  //メッセージをリセットして次のターンへ！
		enemyStatusEffect_ = STATUS_EFFECT::NONE;
		return;
	}
	else
	{
		//全Waveクリア 経験値と場所ボーナスを付与
		int rand = 15 + (GetRand(20) - 10);	//乱数の取得
		int statusBonus = 0;//実際にボーナス計算された後の値を入れる変数

		switch (location_)
		{
		case QUEST_LOCATION::PLAINS:
			playerStatus_->GetExp(rand);
			locationRewardMsg_ = "追加で経験値を" + std::to_string(rand) + "獲得した";
			break;
		case QUEST_LOCATION::FOREST:
			statusBonus = playerStatus_->AddSkillPoint(PlayerStatus::SkillType::Pharmacy, rand);
			locationRewardMsg_ = "追加で薬学を" + std::to_string(statusBonus) + "獲得した";
			break;
		case QUEST_LOCATION::SHRINE:
			statusBonus = playerStatus_->AddSkillPoint(PlayerStatus::SkillType::MartialArts, rand);
			locationRewardMsg_ = "追加で武術を" + std::to_string(statusBonus) + "獲得した";
			break;
		case QUEST_LOCATION::CONTINENT:
			statusBonus = playerStatus_->AddSkillPoint(PlayerStatus::SkillType::MagicKnowledge, rand);
			locationRewardMsg_ = "追加で魔法知識を" + std::to_string(statusBonus) + "獲得した";
			break;
		case QUEST_LOCATION::CATHEDRAL:
			statusBonus = playerStatus_->AddSkillPoint(PlayerStatus::SkillType::Faith, rand);
			locationRewardMsg_ = "追加で信仰を" + std::to_string(statusBonus) + "獲得した";
			break;
		case QUEST_LOCATION::RUINS:
			statusBonus = playerStatus_->AddSkillPoint(PlayerStatus::SkillType::Archaeology, rand);
			locationRewardMsg_ = "追加で考古学を" + std::to_string(statusBonus) + "獲得した";
			break;
		case QUEST_LOCATION::HILL:
			statusBonus = playerStatus_->AddSkillPoint(PlayerStatus::SkillType::Astrology, rand);
			locationRewardMsg_ = "追加で占星術を" + std::to_string(statusBonus) + "獲得した";
			break;
		}

		if (isHellQuest_)playerStatus_->GetExp(150);//激ムズクエストは莫大な経験値を付与

		wasMagicUsedLastTurn_ = magicUsedThisTurn_;
		battleStep_ = BATTLE_STEP::RESULT;
		statusEffect_ = STATUS_EFFECT::NONE; //状態異常リセット
		enemyStatusEffect_ = STATUS_EFFECT::NONE;
		return;
	}
}

void QuestPhase::DisplayResult(void)
{
	//経験値獲得は ProcessActionLoop内で敵を倒すたびに行うように変更したため、ここは次へ進む処理のみ
	if (ins_.IsTrgDown(KEY_INPUT_RETURN) ||
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
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
	//更新された intの値を、安全に元の enum class 型に戻して代入する
	command_ = static_cast<COMMAND>(commandIndex);

	//決定処理
	if (ins_.IsTrgDown(KEY_INPUT_RETURN) ||
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
	{
		//沈黙状態の時は、魔法を選ぼうとしても決定できない
		if (command_ == COMMAND::MAGIC && statusEffect_ == STATUS_EFFECT::SILENCE)
		{
			//エラー音を鳴らす
			battleMessage_ = "魔法を唱えられない";
			return; //決定処理を中断
		}
		if (command_ == COMMAND::MAGIC && wasMagicUsedLastTurn_)return; //魔法の連続使用制限

		if (command_ == COMMAND::MAGIC) magicUsedThisTurn_ = true;
		else magicUsedThisTurn_ = false;

		subActionMessages_.clear(); //一度リセット
		subMenuCursor_ = 0;			//カーソルを先頭に

		switch (command_)
		{
		case QuestPhase::COMMAND::ATTACK:
			battleStep_ = BATTLE_STEP::DETERMINE;
			return;
		case QuestPhase::COMMAND::MAGIC:
			subActionMessages_ = { "攻撃","回復","強化","状態異常付与" };
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
	//選択肢の数を、埋め込まれた配列のサイズから自動取
	int maxSubItems = static_cast<int>(subActionMessages_.size());
	if (maxSubItems == 0) return; //念のため
	
	//カーソル移動処理
	Utility::ProcessCommandMenuSelection(subMenuCursor_, maxSubItems);

	//--- 決定処理 ---
	if (ins_.IsTrgDown(KEY_INPUT_RETURN) ||
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
	{
		//【重要】ここで「何番のサブメニューを選んだか」を記憶しておく
		//例：chosenSubMenuIdx_ = subMenuCursor_; 
		//この数値を、後の DetermineActionOrder や ActionUnit に引き渡します

		magicMenuCursor_ = 0;

		//一度リストを空にする
		magicTypeMessages_.clear();

		//行動決定へ進む
		switch (subMenuCursor_)
		{
		case 0:
			//攻撃
			magicTypeMessages_.push_back("魔法1");

			//レベルに応じて追加
			if (playerStatus_->magicKnowledge_ >= 70)
			{
				magicTypeMessages_.push_back("魔法2");
			}
			if (playerStatus_->magicKnowledge_ >= 140)
			{
				magicTypeMessages_.push_back("魔法3");
			}
			break;
		case 1:
			//回復
			magicTypeMessages_.push_back("ヒール1");
			if (playerStatus_->magicKnowledge_ >= 70)
			{
				magicTypeMessages_.push_back("ヒール2");
			}
			if (playerStatus_->magicKnowledge_ >= 140)
			{
				magicTypeMessages_.push_back("ヒール3");
			}
			break;
		case 2:
			//強化
			magicTypeMessages_ = { "状態異常回復" };
			break;
		case 3:
			//状態異常付与
			magicTypeMessages_.push_back("毒");

			if (playerStatus_->magicKnowledge_ >= 10)
			{
				magicTypeMessages_.push_back("凍結");
			}
			if (playerStatus_->magicKnowledge_ >= 10)
			{
				magicTypeMessages_.push_back("閃光");
			}
			if (playerStatus_->magicKnowledge_ >= 10)
			{
				magicTypeMessages_.push_back("呪い");
			}
			break;
		default:
			break;
		}

		battleStep_ = BATTLE_STEP::MAGIC_SELECTION;
	}
	if (ins_.IsTrgDown(KEY_INPUT_TAB) ||
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT))
	{
		battleStep_ = BATTLE_STEP::COMMAND_SELECTION;
	}
}

void QuestPhase::MagicSelection()
{
	//選択肢の数を、埋め込まれた配列のサイズから自動取
	int maxMagItems = static_cast<int>(magicTypeMessages_.size());
	if (maxMagItems == 0) return;

	//カーソル移動処理
	Utility::ProcessCommandMenuSelection(magicMenuCursor_, maxMagItems);

	//決定処理
	if (ins_.IsTrgDown(KEY_INPUT_RETURN) ||
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
	{
		//ここで最終的に何番目の魔法・状態異常を選んだかを記憶する
		chosenMagicIdx_ = magicMenuCursor_;

		battleStep_ = BATTLE_STEP::DETERMINE;
	}
	if (ins_.IsTrgDown(KEY_INPUT_TAB) ||
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT))
	{
		battleStep_ = BATTLE_STEP::COMMAND_SUB_SELECTION;
	}
}

void QuestPhase::DrawCommandSelection(void)
{
	Utility::DrawCommandMenu(COMMAND_MSG_X, COMMAND_MSG_Y, { "攻撃","魔法" }, static_cast<int>(command_));
}

void QuestPhase::ProcessTutorial(void)
{
	if (!SceneManager::GetInstance().IsTutorialEnabled()) return;

	//チュートリアルの内容をここに書く
	if (gameScene_.GetTurn() == 1)
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

			//もし攻撃のサブメニュー（単体・全体）まで開いているなら、
			//最初の選択肢「単体攻撃（0）」にカーソルを強制ロック！
			if (battleStep_ == BATTLE_STEP::COMMAND_SUB_SELECTION)
			{
				subMenuCursor_ = 0;
				// このあとの通常のカーソル移動キーの入力を無視するフラグ（returnなど）に繋げます
			}
		}
		else if (battleStep_ == BATTLE_STEP::COMMAND_SELECTION
			&& battleTurn_ == 2)
		{
			// メインコマンドは「攻撃（0）」に強制固定
			command_ = COMMAND::MAGIC;
		}
	}
}
void QuestPhase::DrawTutorial(void)
{
	if (!SceneManager::GetInstance().IsTutorialEnabled()) return;

	tutorialMessage_ = "";

	if (battleStep_ == BATTLE_STEP::DIFFICULTY_SELECTION)
	{
		tutorialMessage_ = "チュートリアル\nここではステージを選択できます\n好きなステージを選んで下さい\nEnterキーを押してみましょう";
	}
	if (battleStep_ != BATTLE_STEP::DIFFICULTY_SELECTION
		&& battleTurn_ == 1)
	{
		tutorialMessage_ = "チュートリアル\n攻撃を選びましょう！\n攻撃を選んでEnterキーを押してみましょう";
	}
	else if (battleStep_ != BATTLE_STEP::DIFFICULTY_SELECTION
		&& battleTurn_ == 2)
	{
		tutorialMessage_ = "チュートリアル\n次は魔法で攻撃をしましょう\n魔法を選んでEnterキーを押してみましょう";
	}
	else if (battleStep_ != BATTLE_STEP::DIFFICULTY_SELECTION
		&& battleTurn_ == 3)
	{
		tutorialMessage_ = "チュートリアル\n魔法を使うと１ターンの休憩が必要です\n攻撃を選んでEnterキーを押してみましょう";
	}
	else if (battleStep_ != BATTLE_STEP::DIFFICULTY_SELECTION
		&& battleTurn_ == 4)
	{
		tutorialMessage_ = "チュートリアル\nあとは好きなように戦いましょう";
	}
	//チュートリアルの内容をここに書く
	//例：最初のターンだけ特別な説明を表示するなど
	if (gameScene_.GetTurn() == 1)
	{
		DrawFormatString(0, 500, 0xFFFFFF, tutorialMessage_.c_str());
	}
}