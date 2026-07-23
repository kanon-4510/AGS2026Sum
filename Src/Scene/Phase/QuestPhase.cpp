#include <DxLib.h>
#include <algorithm>
#include "../../Application.h"
#include "../../Common/Color.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/InputManager.h"
#include "../../Utility/AsoUtility.h"
#include "../GameScene.h"
#include "../../Object/Enemy.h"
#include "QuestPhase.h"

//外部にある敵生成関数（Enemy.cppなどに実装している想定）
extern Enemy* SpawnEnemyByTurn(int currentTurn);
extern Enemy* SpawnRushEnemy(int stage); //ラッシュ専用の敵生成関数

// ゲーム内の全魔法データベース
const std::vector<MagicData> MAGIC_DATABASE = 
{
	//【攻撃魔法】
	{1,"ファイア",    MAGIC_TYPE::ATTACK,1.5f,STATUS_EFFECT::NONE,0,0},
	{2,"フレイム",    MAGIC_TYPE::ATTACK,2.0f,STATUS_EFFECT::NONE,0,100},
	{3,"プロミネンス",MAGIC_TYPE::ATTACK,4.0f,STATUS_EFFECT::NONE,0,350},
	{4,"ビッグバン",  MAGIC_TYPE::ATTACK,10.0f,STATUS_EFFECT::NONE,0,999},

	//【回復魔法】高中低、状態異常回復＋回復
	{5, "ドレイン",  MAGIC_TYPE::HEAL,0.7f,STATUS_EFFECT::NONE,0,230,true,false},
	{6, "ヒール",    MAGIC_TYPE::HEAL,0.3f,STATUS_EFFECT::NONE,0,0},
	{7, "ハイヒール",MAGIC_TYPE::HEAL,0.5f,STATUS_EFFECT::NONE,0,100},
	{8, "メガヒール",MAGIC_TYPE::HEAL,1.0f,STATUS_EFFECT::NONE,0,360},
	{9, "キュア",    MAGIC_TYPE::HEAL,0.0f,STATUS_EFFECT::NONE,0,60,false,true},
	{10,"レスキュー",MAGIC_TYPE::HEAL,0.5f,STATUS_EFFECT::NONE,0,250,false,true},

	//【状態異常攻撃】高中低（確率は低30%, 中50%, 高70%,100%）
	{11,"アシッドニードル",MAGIC_TYPE::DEBUFF,1.0f,STATUS_EFFECT::POISON,30,50},
	{12,"ヴェノムスピア",  MAGIC_TYPE::DEBUFF,2.5f,STATUS_EFFECT::POISON,50,120},
	{13,"トキシックランス",MAGIC_TYPE::DEBUFF,4.0f,STATUS_EFFECT::POISON,70,320},
	{14,"ポイズン",		   MAGIC_TYPE::DEBUFF,0.0f,STATUS_EFFECT::POISON,100,220},
	{15,"コールドブレス",  MAGIC_TYPE::DEBUFF,1.0f,STATUS_EFFECT::FREEZE,30,50},
	{16,"スノーテンペスト",MAGIC_TYPE::DEBUFF,2.5f,STATUS_EFFECT::FREEZE,50,120},
	{17,"アイシクルノヴァ",MAGIC_TYPE::DEBUFF,4.0f,STATUS_EFFECT::FREEZE,70,320},
	{18,"フリーズ",		   MAGIC_TYPE::DEBUFF,0.0f,STATUS_EFFECT::FREEZE,100,220},
	{19,"フォトンシュート",MAGIC_TYPE::DEBUFF,1.0f,STATUS_EFFECT::FLASH,30,50},
	{20,"プリズムレーザー",MAGIC_TYPE::DEBUFF,2.5f,STATUS_EFFECT::FLASH,50,120},
	{21,"ミラージュレイン",MAGIC_TYPE::DEBUFF,4.0f,STATUS_EFFECT::FLASH,70,320},
	{22,"フラッシュ",	   MAGIC_TYPE::DEBUFF,0.0f,STATUS_EFFECT::FLASH,100,220},
	{23,"リーサルクロー",  MAGIC_TYPE::DEBUFF,3.0f,STATUS_EFFECT::CURSE,5,180},
	{24,"フェイタルソード",MAGIC_TYPE::DEBUFF,4.5f,STATUS_EFFECT::CURSE,10,400},
};

//コンストラクタ
QuestPhase::QuestPhase(PlayerStatus* playerStatus, GameScene& gameScene, bool isHellQuest)
	: gameScene_(gameScene)
	, playerStatus_(playerStatus)
	, isHellQuest_(isHellQuest)
	, bgImageHandle_(-1)
{
	bgImageBar_ = LoadGraph("Data/Image/Stage/BattleBar.png");
	bgImg_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::GAME_SCENE).handleId_;
	playerImg_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::PLAYER).handleId_;

	messageBoxImg_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::MESSAGE_BOX).handleId_;

	//クエスト開始時は一旦通常の敵を生成しておく
	activeEnemy_ = SpawnEnemyByTurn(gameScene_.GetTurn());
	statusEffect_ = STATUS_EFFECT::NONE;	//状態異常の初期化
	battleStep_ = BATTLE_STEP::DIFFICULTY_SELECTION;
	locationMenu_ = { "平原","魔法の森","岩山の道場","魔大陸","壊れた聖堂","古代遺跡","星の丘" };	//ここで難易度メニューを動的に作成
	selectableLocations_ = { QUEST_LOCATION::PLAINS,QUEST_LOCATION::FOREST,QUEST_LOCATION::SHRINE,QUEST_LOCATION::CONTINENT,QUEST_LOCATION::CATHEDRAL,QUEST_LOCATION::RUINS,QUEST_LOCATION::HILL };

	//13ターン目以降 かつ まだ一度も挑んでいないなら
	if (gameScene_.GetTurn() >= 17 && !playerStatus_->hasChallengedHellQuest_)
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

	playerStatus_->Update(); //プレイヤーの更新処理

	if (activeEnemy_ != nullptr)
	{
		activeEnemy_->Update(); //敵の更新処理
	}

	if (gameScene_.GetTurn() >= 21  && activeEnemy_ != nullptr && activeEnemy_->IsDead())
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::CLEAR);
	}
}

//描画処理
void QuestPhase::Draw(void)
{
	if (bgImageHandle_ != -1)DrawGraph(0, -100, bgImageHandle_, TRUE);//背景を描画する
#pragma region 戦闘時の画面下部バーの表示
	if (battleStep_ != BATTLE_STEP::DIFFICULTY_SELECTION)
	{
		SetFontSize(22);
		DrawGraph(0, 0, bgImageBar_, true);
		DrawFormatString( 230,590,0xffffff,"ルピナス");
		DrawFormatString( 245,615,0xffffff,"レベル %2d",playerStatus_->level_);
		DrawFormatString( 245,640,0xffffff,"　筋力 %d" ,playerStatus_->power_ + playerStatus_->GetJobBonus().power);
		DrawFormatString( 245,665,0xffffff,"　魔力 %d" ,playerStatus_->MagicAttack());
		DrawFormatString( 245,690,0xffffff,"素早さ %d" ,playerStatus_->GetSpeed());
		DrawFormatString(1065,510,0xffffff,"　　治癒力:%+3d",playerStatus_->pharmacy_/10);
		DrawFormatString(1065,545,0xffffff,"会心発生率:%3d%%",playerStatus_->martialArts_/5);
		DrawFormatString(1065,580,0xffffff,"魔術ランク:%3d",(playerStatus_->magicKnowledge_/50)+1);
		DrawFormatString(1065,615,0xffffff,"　　守備力:%3d",playerStatus_->faith_/15);
		DrawFormatString(1065,650,0xffffff,"獲得経験値:%+3d",playerStatus_->archaeology_/10);
		DrawFormatString(1065,685,0xffffff,"　　回避率:%3d%%",playerStatus_->astrology_/5);
		DrawFormatString(900,590,0xffffff,"%s",playerStatus_->job.c_str());
		DrawFormatString(900,640,0xffffff,"状態:");
		if(statusEffect_==STATUS_EFFECT::NONE)   DrawFormatString(955,640,0xffffff,"なし");
		if(statusEffect_==STATUS_EFFECT::POISON) DrawFormatString(955,640,0x00cc00,"どく");
		if(statusEffect_==STATUS_EFFECT::FLASH)  DrawFormatString(955,640,0xffff00,"せんこう");
		if(statusEffect_==STATUS_EFFECT::FREEZE) DrawFormatString(955,640,0x00cccc,"とうけつ");
		if(statusEffect_==STATUS_EFFECT::CURSE)  DrawFormatString(955,640,0xcc00cc,"のろい");
		if(statusEffect_==STATUS_EFFECT::SILENCE)DrawFormatString(955,640,0xdd0000,"ちんもく");
		SetFontSize(16);
	}
#pragma endregion
	DrawTutorial();

	if (battleStep_ != BATTLE_STEP::DIFFICULTY_SELECTION && battleStep_ != BATTLE_STEP::RESULT)
	{
		activeEnemy_->Draw(); //敵の描画

		int maxWaves = isHellQuest_ ? 5 : MAX_WAVES;
		SetFontSize(32);
		DrawFormatString(0,0, 0xFFFF00, "【WAVE %d / %d】", currentWave_, maxWaves);
		SetFontSize(16);
		playerStatus_->DrawQuestImages();	//プレイヤーの画像を描画
	}

	if (battleStep_ == BATTLE_STEP::DIFFICULTY_SELECTION)//難易度選択中はコマンドやHPを表示しない
	{
		DrawGraph(0, 0, bgImg_, true);
		DrawGraph(700, 200, playerImg_, true);
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
		//同時に画面に表示したい最大件数（枠のサイズ）
		//リストが空の場合は描画しない
		if (!magicTypeMessages_.empty())
		{
			//スクロールの開始位置（オフセット）を計算
			int scrollOffset = 0;
			if (magicMenuCursor_ >= MAX_DISPLAY)
			{
				//カーソルが画面の下端（6個目以降）に行ったら、表示範囲をズラす
				scrollOffset = magicMenuCursor_ - MAX_DISPLAY + 1;
			}

			//「今画面に見せるべき魔法」だけを詰め替える
			std::vector<std::string> visibleMessages;
			for (int i = scrollOffset; i < scrollOffset + MAX_DISPLAY && i < magicTypeMessages_.size(); ++i)
			{
				visibleMessages.push_back(magicTypeMessages_[i]);
			}

			//切り出したリストと、枠内での相対カーソル位置を渡して描画
			int relativeCursor = magicMenuCursor_ - scrollOffset;
			Utility::DrawCommandMenu(COMMAND_MSG_X, COMMAND_MSG_Y, visibleMessages, relativeCursor);

			//上下にまだ隠れた魔法があるよ！という▲▼ガイド表示
			if (scrollOffset > 0)
			{
				DrawFormatString(COMMAND_MSG_X + 50, COMMAND_MSG_Y - 20, 0xFFFFFF, "▲"); // 上にスクロールできる
			}
			if (scrollOffset + MAX_DISPLAY < magicTypeMessages_.size())
			{
				//1行の高さが仮に30ピクセルだとした場合の計算
				DrawFormatString(COMMAND_MSG_X + 50, COMMAND_MSG_Y + (visibleMessages.size() * 30) + 10, 0xFFFFFF, "▼"); //下にスクロールできる
			}

			if (magicMenuCursor_ >= 0 && magicMenuCursor_ < availableMagics_.size())
			{
				const auto& hoverMagic = availableMagics_[magicMenuCursor_];

				//説明を表示する座標（魔法リストの右側に表示する設定）
				int descX = COMMAND_MSG_X + 170;
				int descY = COMMAND_MSG_Y;

				if (playerStatus_->magicKnowledge_ >= hoverMagic.reqMagicKnowledge)
				{
					DrawString(descX, descY, "【魔法の効果】", 0xffffff); // タイトルを黄色に

					//魔法のタイプごとに説明を出し分ける
					switch (hoverMagic.id)
					{
					case 1:
						DrawString(descX, descY + 25, "火を放ち敵を攻撃する魔法。\n威力は小さい。", 0xFFFFFF);
						break;
					case 2:
						DrawString(descX, descY + 25, "炎で敵を攻撃する魔法。\n威力は中くらい。", 0xFFFFFF);
						break;
					case 3:
						DrawString(descX, descY + 25, "灼熱で敵を焼き尽くす魔法。\n威力は大きい。", 0xFFFFFF);
						break;
					case 4:
						DrawString(descX, descY + 25, "目の前の敵を影ごと消し去る魔法。\n威力は絶大。", 0xFFFFFF);
						break;
					case 5: 
						DrawString(descX, descY + 25, "敵の生命力を奪い、\n自身の体力を回復する魔法。\n効果はまあまあ。", 0xFFFFFF);
						break;
					case 6:
						DrawString(descX, descY + 25, "自分の体力を回復する魔法。\n効果はちょこっと。", 0xFFFFFF);
						break;
					case 7:
						DrawString(descX, descY + 25, "自分の体力を回復する魔法。\n効果はなかなか。", 0xFFFFFF);
						break;
					case 8:
						DrawString(descX, descY + 25, "自分の体力を回復する魔法。\n効果はけっこう。", 0xFFFFFF);
						break;
					case 9:
						DrawString(descX, descY + 25, "自分の状態異常を治療する魔法。\n回復効果はない。", 0xFFFFFF);
						break;
					case 10:
						DrawString(descX, descY + 25, "自分の体力を回復し、\nさらに状態も治す魔法。\n効果はぼちぼち。", 0xFFFFFF);
						break;
					case 11:
						DrawString(descX, descY + 25, "毒の針で突き刺す魔法。\n確率で毒状態にする。\n威力と確率は低い。", 0xFFFFFF);
						break;
					case 12:
						DrawString(descX, descY + 25, "毒の槍で貫く魔法。\n確率で毒状態にする。\n威力と確率は普通。", 0xFFFFFF);
						break;
					case 13:
						DrawString(descX, descY + 25, "毒の巨戟で穿つ魔法。\n確率で毒状態にする。\n威力と確率は高い。", 0xFFFFFF);
						break;
					case 14:
						DrawString(descX, descY + 25, "毒液を浴びせる魔法。\n必ず毒状態にする。\n威力はない。", 0xFFFFFF);
						break;
					case 15:
						DrawString(descX, descY + 25, "冷たい風を吹かせる魔法。\n確率で凍結状態にする。\n威力と確率は低い。", 0xFFFFFF);
						break;
					case 16:
						DrawString(descX, descY + 25, "凍える嵐を起こす魔法。\n確率で凍結状態にする。\n威力と確率は普通。", 0xFFFFFF);
						break;
					case 17:
						DrawString(descX, descY + 25, "氷の爆風を作り出す魔法。\n確率で凍結状態にする。\n威力と確率は高い。", 0xFFFFFF);
						break;
					case 18:
						DrawString(descX, descY + 25, "凍らせる魔法。\n必ず凍結状態にする。\n威力はない。", 0xFFFFFF);
						break;
					case 19:
						DrawString(descX, descY + 25, "光る球をぶつける魔法。\n確率で閃光状態にする。\n威力と確率は低い。", 0xFFFFFF);
						break;
					case 20:
						DrawString(descX, descY + 25, "輝く光線で攻撃する魔法。\n確率で閃光状態にする。\n威力と確率は普通。", 0xFFFFFF);
						break;
					case 21:
						DrawString(descX, descY + 25, "煌めく雨を降らせる魔法。\n確率で閃光状態にする。\n威力と確率は高い。", 0xFFFFFF);
						break;
					case 22:
						DrawString(descX, descY + 25, "閃光を発生させる魔法。\n必ず閃光状態にする。\n威力はない。", 0xFFFFFF);
						break;
					case 23:
						DrawString(descX, descY + 25, "闇の爪で攻撃する魔法。\n超低確率で呪い状態にする。\n威力は普通。", 0xFFFFFF);
						break;
					case 24:
						DrawString(descX, descY + 25, "命を刈り取る剣を呼び出す魔法。\n低確率で呪い状態にする。\n威力は高い。", 0xFFFFFF);
						break;
					}
				}
				else
				{
					// 魔法知識が足りない場合は詳細を隠す
					DrawString(descX, descY, "【魔法の効果】", 0xffffff);
					DrawFormatString(descX, descY + 25, 0xffffff, "詳細不明", hoverMagic.reqMagicKnowledge);
				}
			}
		}
	}
	else if (battleStep_ == BATTLE_STEP::RESULT)
	{
		DrawString(350,510, "遠征クリア！経験値を獲得した！\nレベルが上がった！基礎ステータスが上がった！", 0xFFFFFF);
		DrawString(350,545, locationRewardMsg_.c_str(), 0xFFFF00);
	}

	if (activeEnemy_ != nullptr && !activeEnemy_->IsDead())
	{
		DrawFormatString(BATTLE_MSG_X, BATTLE_MSG_Y, 0xFF0000, battleMessage_.c_str());
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
			playerStatus_->hasChallengedHellQuest_ = true;//二度と選べないようにフラグを回収
			location_ = QUEST_LOCATION::EXTRA;			  //場所を魔大陸に設定

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

		//バトル開始時のダメージ
		if (playerStatus_->hasStartDamage)activeEnemy_->Damage(playerStatus_->magic_);

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
		playerStatus_->hp_ = playerStatus_->GetMaxHp();
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
	actionOrder_.push_back({ playerStatus_->GetName().c_str(), playerStatus_->GetSpeed(), true, 0, (int)command_, 0});
	if (command_ == COMMAND::ATTACK)
	{
		actionOrder_.back().skillName = "単体攻撃";
	}
	// 魔法攻撃の場合
	if (command_ == COMMAND::MAGIC)
	{
		// サブメニューの選択肢に応じて魔法の性質を分ける！
		if (subMenuCursor_ == static_cast<int>(MAGIC_TYPE::ATTACK))
		{
			actionOrder_.back().magicType = MAGIC_TYPE::ATTACK; //攻撃魔法
		}
		else if (subMenuCursor_ == static_cast<int>(MAGIC_TYPE::HEAL))
		{
			actionOrder_.back().magicType = MAGIC_TYPE::HEAL;   //回復魔法
		}
		else if (subMenuCursor_ == static_cast<int>(MAGIC_TYPE::DEBUFF)) 
		{
			actionOrder_.back().magicType = MAGIC_TYPE::DEBUFF; //弱化魔法
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
					battleMessage_ = "氷が溶けてうごけるようになった!\n";
					statusEffect_ = STATUS_EFFECT::NONE;
				}
			}
			//閃光(命中低下)
			if (!skipAction && statusEffect_ == STATUS_EFFECT::FLASH)
			{
				if (command_ == COMMAND::ATTACK || (command_ == COMMAND::MAGIC && (unit.magicType == MAGIC_TYPE::ATTACK || unit.magicType == MAGIC_TYPE::DEBUFF)))
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

					//基本の攻撃力に、スキルがあれば魔力を足す
					int attackPow = playerStatus_->Attack();
					if (playerStatus_->hasMagicToAttack)
					{
						attackPow += playerStatus_->MagicAttack()/2;
						battleMessage_ += "\n【カドゥケウス】攻撃の威力が上がった！";
					}

					//会心判定
					//計算式：武術のステータス÷5
					int criticalChance = playerStatus_->martialArts_ / 5;
					if (GetRand(99) < criticalChance)
					{
						battleMessage_ += "クリティカルヒット！";
						int critMultiplier = playerStatus_->hasCritBoost ? 7.5 : 2.5;
						playerStatus_->AttackAnimation();
						activeEnemy_->ChangeAnim(ANIM_DAMAGE);
						if (playerStatus_->hasCritBoost)battleMessage_ += "\n【極聖光】会心倍率が上がった！";
						activeEnemy_->Damage(attackPow * critMultiplier);
					}
					else
					{
						playerStatus_->AttackAnimation();
						activeEnemy_->ChangeAnim(ANIM_DAMAGE);
						activeEnemy_->Damage(attackPow);
					}
				}
				else if (command_ == COMMAND::MAGIC)
				{
					//selectedMagic_ は、前のメニュー選択画面で確定した MagicData 構造体とします
					battleMessage_ += unit.name + "の" + selectedMagic_.name + "！";

					//①ダメージ処理（威力が0より大きければダメージを与える）
					if (selectedMagic_.powerMultiplier > 0 && (selectedMagic_.type == MAGIC_TYPE::ATTACK||selectedMagic_.type == MAGIC_TYPE::DEBUFF))
					{
						//魔力 × (魔法の威力) など、威力を反映させた計算式にする
						int magicDamage = playerStatus_->MagicAttack() * selectedMagic_.powerMultiplier;
						activeEnemy_->Damage(magicDamage);
					}

					//②カテゴリ別の特殊処理（回復や状態異常）
					switch (selectedMagic_.type)
					{
					case MAGIC_TYPE::HEAL:
						//powerの数値を回復量として使う
						//通常の回復処理（威力が0より大きい場合だけ回復メッセージが出る）
						if (!selectedMagic_.isDrain && selectedMagic_.powerMultiplier > 0.0f)
						{
							int healAmount = static_cast<int>(playerStatus_->MagicAttack() * selectedMagic_.powerMultiplier);
							playerStatus_->Heal(healAmount);
							battleMessage_ += "\n体力が回復した！";
						}
						//状態異常治療フラグが true だったら治す
						if (selectedMagic_.curesStatus)
						{
							statusEffect_ = STATUS_EFFECT::NONE; //プレイヤーの状態異常を治す
							statusTurns_ = 4;
							poisonCnt_ = 0;
							battleMessage_ += "\n状態異常が回復した！";
						}
						break;
					case MAGIC_TYPE::DEBUFF:
						//状態異常を付与する魔法の場合
						if (selectedMagic_.ailment != STATUS_EFFECT::NONE)
						{
							//魔法ごとに設定された「状態異常確率」で判定
							if (GetRand(99) < selectedMagic_.ailmentChance)
							{
								enemyStatusEffect_ = selectedMagic_.ailment;
								battleMessage_ += "\n敵に状態異常を与えた！";
							}
						}
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
					|| unit.skillName == "自己再生")
				{
					//Power分回復
					int healAmount = activeEnemy_->GetPower3();
					activeEnemy_->Heal(healAmount);
					battleMessage_ += "\n" + unit.name + "の体力が" + std::to_string(healAmount) + "回復した";
				}
				else if(unit.skillName == "遡行")
				{
					//Power分回復
					int healAmount = activeEnemy_->GetPower3();
					activeEnemy_->Heal(healAmount);
					enemyStatusEffect_ = STATUS_EFFECT::NONE; //状態異常を治す
					enemyCurs_ = ENEMY_CURS_TURN;
					battleMessage_ += "\n" + unit.name + "の肉体が巻き戻る";
				}
				else if (unit.skillName == "まもる" || unit.skillName == "守る"
					|| unit.skillName == "守りの構え" || unit.skillName == "受流しの構え")
				{
					//Power分ダメージ軽減
					activeEnemy_->SetGuard(activeEnemy_->GetPower3());
					battleMessage_ += "\n" + unit.name + "は身構えている";
				}
				else {
					//回避判定
					//計算式：占星術のステータス÷5　　※運の数値に合わせて調整
					int evasionChance = playerStatus_->astrology_ / 5;

					//バランス崩壊を防ぐための安全装置（最大回避率を90%でストップさせる）
					if (evasionChance > 90) evasionChance = 90;
					int roll = GetRand(99);

					if (roll < evasionChance)
					{
						//回避成功！ダメージ処理はスキップしてメッセージだけ上書き
						battleMessage_ = unit.name + "の" + unit.skillName;
						battleMessage_ += "\n攻撃を回避した！";
					}
					else if (!activeEnemy_->IsDead())
					{
						//初撃無効スキルを持っていて、まだ使っていない場合
						if (playerStatus_->hasFirstHitNull && !playerStatus_->isFirstHitUsed)
						{
							playerStatus_->isFirstHitUsed = true; //消費する
							battleMessage_ += playerStatus_->GetName() + "クラススキル【神秘の護り】\n攻撃を防いだ！";
							return; //ダメージ処理に行かずに関数を抜ける
						}

						if (unit.skillName == "蛇にらみ" || unit.skillName == "石化の魔眼"
							|| unit.skillName == "金縛り" || unit.skillName == "発狂")
						{
							//プレイヤーを凍結状態にする
							int damage = activeEnemy_->GetPower3();
							playerStatus_->Damage(damage);
							if (statusEffect_ == STATUS_EFFECT::NONE)
							{
								statusEffect_ = STATUS_EFFECT::FREEZE;
								battleMessage_ += "\n"+playerStatus_->GetName() + "は凍りついた！";
							}
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
								battleMessage_ += "\n" + playerStatus_->GetName() + "は沈黙になった！";
							}
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
								battleMessage_ += "\n" + playerStatus_->GetName() + "は毒状態になった！";
							}
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
								battleMessage_ += "\n" + playerStatus_->GetName() + "は呪われた！";
							}
						}
						else if (unit.skillName == "ばくはつ" || unit.skillName == "電撃斬"
							|| unit.skillName == "雷連斬" || unit.skillName == "エレキビーム" || unit.skillName == "斬撃"
							|| unit.skillName == "魔の威光")
						{
							//プレイヤーを閃光状態にする
							int damage = activeEnemy_->GetPower2();
							playerStatus_->Damage(damage);
							if (statusEffect_ == STATUS_EFFECT::NONE)
							{
								statusEffect_ = STATUS_EFFECT::FLASH;
								battleMessage_ += "\n" + playerStatus_->GetName() + "は目がくらんだ！";
							}
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

							//回避失敗 通常通りダメージを受ける
							playerStatus_->DamageAnimation();
							playerStatus_->Damage(damage);
						}
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
	if (activeEnemy_->IsAnimFinished() == true &&
		ins_.IsTrgDown(KEY_INPUT_RETURN) ||
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

		//毎ターンHP回復（状態異常ダメージの前に回復させる）
		if (playerStatus_->hasAutoRegen && playerStatus_->hp_ > 0 && playerStatus_->hp_ < playerStatus_->GetMaxHp())
		{
			int regenAmount = playerStatus_->GetMaxHp() / 10;
			playerStatus_->hp_ += regenAmount;
			if (playerStatus_->hp_ > playerStatus_->GetMaxHp())
			{
				playerStatus_->hp_ = playerStatus_->GetMaxHp();
			}
			battleMessage_ = "【エリクサー】\n"+playerStatus_->GetName() + "の傷が塞がっていく！";
			hasEffectMessage = true;
		}

		//定数ダメージ(poison)
		else if (enemyStatusEffect_ == STATUS_EFFECT::POISON)
		{
			//ターンの最後にダメージを受ける（例：1ダメージ）
			battleMessage_ = activeEnemy_->GetName() + "は毒のダメージを受けた\n";
			activeEnemy_->ChangeAnim(ANIM_DAMAGE);
			activeEnemy_->Damage(activeEnemy_->GetCurrentHp()/16);
			hasEffectMessage = true;
		}
		else if (enemyStatusEffect_ == STATUS_EFFECT::CURSE)
		{
			enemyCurs_--;
			if (enemyCurs_ <= 0)
			{
				//HPが2以上の時だけダメージを与える（HPがすでに1以下の時のバグ防止）
				if (activeEnemy_->GetCurrentHp() > 1)
				{
					battleMessage_ = activeEnemy_->GetName() + "にかかった呪いが発動した……";
					activeEnemy_->ChangeAnim(ANIM_DAMAGE); //ダメージアニメーション
					activeEnemy_->Damage(activeEnemy_->GetCurrentHp() - 1); //HPを1にする
				}
				//発動後は呪い状態を解除する
				enemyStatusEffect_ = STATUS_EFFECT::NONE;
				enemyCurs_ = ENEMY_CURS_TURN;
			}
			else
			{
				battleMessage_ = "呪いまで あと" + std::to_string(enemyCurs_) + "ターン";
			}
			hasEffectMessage = true;
		}

		//定数ダメージ(poison)
		if (statusEffect_ == STATUS_EFFECT::POISON)
		{
			poisonCnt_++;
			//ターンの最後にダメージを受ける（例：1ダメージ）
			battleMessage_ += playerStatus_->GetName() + "は毒のダメージを受けた";
			playerStatus_->Damage(playerStatus_->GetMaxHp()*(poisonCnt_/16));
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
	if (activeEnemy_->IsAnimFinished() == true && 
		ins_.IsTrgDown(KEY_INPUT_RETURN) ||
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
		playerStatus_->isFirstHitUsed = false;

		//連戦時のダメージ判定！
		if (playerStatus_->hasStartDamage)
		{
			activeEnemy_->Damage(playerStatus_->magic_);
		}

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
			locationRewardMsg_ += "追加で経験値を" + std::to_string(rand) + "獲得した";
			break;
		case QUEST_LOCATION::FOREST:
			statusBonus = playerStatus_->AddSkillPoint(PlayerStatus::SkillType::Pharmacy, rand);
			locationRewardMsg_ += "追加で薬学を" + std::to_string(statusBonus) + "獲得した";
			break;
		case QUEST_LOCATION::SHRINE:
			statusBonus = playerStatus_->AddSkillPoint(PlayerStatus::SkillType::MartialArts, rand);
			locationRewardMsg_ += "追加で武術を" + std::to_string(statusBonus) + "獲得した";
			break;
		case QUEST_LOCATION::CONTINENT:
			statusBonus = playerStatus_->AddSkillPoint(PlayerStatus::SkillType::MagicKnowledge, rand);
			locationRewardMsg_ += "追加で魔法知識を" + std::to_string(statusBonus) + "獲得した";
			break;
		case QUEST_LOCATION::CATHEDRAL:
			statusBonus = playerStatus_->AddSkillPoint(PlayerStatus::SkillType::Faith, rand);
			locationRewardMsg_ += "追加で信仰を" + std::to_string(statusBonus) + "獲得した";
			break;
		case QUEST_LOCATION::RUINS:
			statusBonus = playerStatus_->AddSkillPoint(PlayerStatus::SkillType::Archaeology, rand);
			locationRewardMsg_ += "追加で考古学を" + std::to_string(statusBonus) + "獲得した";
			break;
		case QUEST_LOCATION::HILL:
			statusBonus = playerStatus_->AddSkillPoint(PlayerStatus::SkillType::Astrology, rand);
			locationRewardMsg_ += "追加で占星術を" + std::to_string(statusBonus) + "獲得した";
			break;
		}

		if (isHellQuest_)playerStatus_->GetExp(450);//激ムズクエストは莫大な経験値を付与

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
		//魔法連続で使おうとしたとき
		if (command_ == COMMAND::MAGIC && wasMagicUsedLastTurn_ && !playerStatus_->hasMagicUnlock)
		{
			battleMessage_ = "魔法詠唱中...";
			return; //魔法の連続使用制限
		}
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
			subActionMessages_ = { "攻撃","回復","状態異常付与" };
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
		//この数値を、後の DetermineActionOrder や ActionUnit に引き渡す

		magicMenuCursor_ = 0;

		//一度リストを空にする
		magicTypeMessages_.clear();
		availableMagics_.clear(); //裏側で持っておく魔法データのリストも空にする

		// ① カーソルの位置から「どのカテゴリを選んだか」を判定
		MAGIC_TYPE selectedCategory; switch (subMenuCursor_)
		{
		case 0: selectedCategory = MAGIC_TYPE::ATTACK;  break;//攻撃
		case 1: selectedCategory = MAGIC_TYPE::HEAL;    break;//回復
		case 2: selectedCategory = MAGIC_TYPE::DEBUFF;  break;//状態異常付与
		}

		//②データベースから、選んだカテゴリの魔法をすべて探す
		//（※ MAGIC_DATABASE は、前回作った全魔法のリストです）
		for (const auto& magic : MAGIC_DATABASE)
		{
			if (magic.type == selectedCategory)
			{
				//裏側のデータリストにはそのまま保存（次のステップで使うため）
				availableMagics_.push_back(magic);

				//画面に表示するテキストの判定
				if (playerStatus_->magicKnowledge_ >= magic.reqMagicKnowledge)
				{
					//魔法知識が足りているなら本名を表示
					magicTypeMessages_.push_back(magic.name);
				}
				else
				{
					//魔法知識が足りない場合は「???」にする
					//必要数値を横に添えてあげる
					std::string hiddenName = "? ? ? (知識:" + std::to_string(magic.reqMagicKnowledge) + ")";
					magicTypeMessages_.push_back(hiddenName);
				}
			}
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
	//選択肢の数を、埋め込まれた配列のサイズから自動取得
	int maxMagItems = static_cast<int>(magicTypeMessages_.size());
	if (maxMagItems == 0) return;

	//カーソル移動処理
	Utility::ProcessCommandMenuSelection(magicMenuCursor_, maxMagItems);

	//決定処理
	if (ins_.IsTrgDown(KEY_INPUT_RETURN) ||
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
	{
		//今カーソルが合っている魔法の「データ」を取り出す
		auto& chosenMagic = availableMagics_[magicMenuCursor_];

		//プレイヤーの魔法知識が、その魔法の必要知識に達しているかチェック！
		if (playerStatus_->magicKnowledge_ >= chosenMagic.reqMagicKnowledge)
		{
			//【条件クリア】使える場合

			//先ほどの ActionLoop (実際の行動処理) でダメージ計算などに使うため、
			//選ばれた魔法のデータそのものを変数に記憶しておく
			selectedMagic_ = chosenMagic;

			//（※チームメイトのコードの他部分で idx が必要な場合のために残す）
			chosenMagicIdx_ = magicMenuCursor_;

			battleStep_ = BATTLE_STEP::DETERMINE;
		}
		else
		{
			//【条件未達】使えない魔法（「???」）を選ぼうとした場合
			//ここでは battleStep_ を進めない（決定させない）

			//エラー音（ブブーッ）を鳴らす
			//PlaySoundMem(errorSound_, DX_PLAYTYPE_BACK); 
		}
	}

	//キャンセル処理
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

	SetFontSize(20);
	if (battleStep_ == BATTLE_STEP::DIFFICULTY_SELECTION)
	{
		DrawGraph(MESSAGE_BOX_X, MESSAGE_BOX_Y, messageBoxImg_, true);
		DrawString(TUTORIAL_X, TUTORIAL_Y
			, "ステージを選択できます\n選ぶステージによって上昇\nする技能が異なります。"
			, Color::BLACK);
		SetFontSize(DEFAULT_FONT_SIZE);
	}
	if (battleStep_ != BATTLE_STEP::DIFFICULTY_SELECTION
		&& battleTurn_ == 1)
	{
		tutorialMessage_ = "攻撃を選びましょう！\n攻撃を選んだ状態で、決定\nボタンを押しましょう";
	}
	else if (battleStep_ != BATTLE_STEP::DIFFICULTY_SELECTION
		&& battleTurn_ == 2)
	{
		tutorialMessage_ = "次は魔法を選びましょう！\n魔法を選んで好きな行動を\n選びましょう\nキャンセルで戻る";
	}
	else if (battleStep_ != BATTLE_STEP::DIFFICULTY_SELECTION
		&& battleTurn_ == 3)
	{
		tutorialMessage_ = "魔法を使うと次のターンは\n使えなくなります\n攻撃を選びましょう";
	}
	else if (battleStep_ != BATTLE_STEP::DIFFICULTY_SELECTION
		&& battleTurn_ == 4)
	{
		tutorialMessage_ = "あとは考えながら\n好きなように戦いましょう";
	}
	//チュートリアルの内容をここに書く
	//例：最初のターンだけ特別な説明を表示するなど
	if (gameScene_.GetTurn() == 1 && battleStep_ != BATTLE_STEP::DIFFICULTY_SELECTION
		&& battleTurn_ <= 4)
	{
		DrawGraph(10, 35, messageBoxImg_, true);
		DrawFormatString(25, 65, Color::BLACK, tutorialMessage_.c_str());
	}
	SetFontSize(DEFAULT_FONT_SIZE);
}