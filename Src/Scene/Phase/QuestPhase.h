#pragma once

#include "PhaseBase.h"
#include "../../Object/Stage.h"
#include "../../Object/PlayerStatus.h"
#include "../../Object/MagicDataBase.h"
#include <vector>
#include <string>

// クエストの場所を定義
enum class QUEST_LOCATION
{
	PLAINS,		//平原
	FOREST,		//魔法の森
	SHRINE,		//岩山の道場
	CONTINENT,	//魔大陸
	CATHEDRAL,	//壊れた聖堂
	RUINS,		//古代遺跡
	HILL,		//星の丘
	EXTRA
};

class GameScene;
class Enemy;

class QuestPhase : public PhaseBase
{
public:
	//バトルのステップ
	enum class BATTLE_STEP
	{
		STAGE_SELECTION,		//ステージ選択
		COMMAND_SELECTION,		//メインコマンド選択
		COMMAND_SUB_SELECTION,	//サブコマンド選択
		MAGIC_SELECTION,		//魔法選択
		DETERMINE,				//コマンド決定
		ACTION_LOOP,			//行動ループ
		STATUS_EFFECT,			//状態異常の処理
		RESULT,					//結果表示
		MAX
	};

	//プレイヤーのコマンド
	enum class COMMAND
	{
		ATTACK,	//物理攻撃
		MAGIC,	//魔法
		MAX
	};

	struct ActionUnit
	{
		std::string name;		//表示用（プレイヤー or 敵の名前）
		int speed;				//素早さ（ソート用）
		bool isPlayer;			//誰の行動か判別用
		int id;					//敵が複数いる場合の識別番号
		int command;			//選択した行動（0:攻撃、1:魔法）
		int targetIdx;			//攻撃対象が誰か
		std::string skillName;  //使った技を記憶しておく
		MAGIC_TYPE magicType;	//魔法の効果の種類を記憶しておく
	};

	//------定数---------
	
	//フォントサイズ
	static constexpr int TUTORIAL_FONT_SIZE = 20;	//フォントサイズが20
	static constexpr int STATUS_FONT_SIZE = 22;		//フォントサイズが22
	static constexpr int WAVE_FONT_SIZE = 32;		//フォントサイズが22

	//チュートリアルターン
	static constexpr int ATTACK_TUTORIAL = 0;	//ターン数が１の時
	static constexpr int MAGIC_TUTORIAL = 1;	//ターン数が２の時
	static constexpr int MAGIC_INTARVAL_TUTORIAL_TURN = 2;	//ターン数が３の時
	static constexpr int TUTORIAL_MAX_TURN = 3;	//ターン数が４の時

	//難易度選択のメッセージの位置
	static constexpr int DIFFICULTY_MSG_X = 0;
	static constexpr int DIFFICULTY_MSG_Y = 40;

	//次に進むメッセージの位置
	static constexpr int DIFFICULTY_ENTER_MSG_X = 0;
	static constexpr int DIFFICULTY_ENTER_MSG_Y = 400;
	
	static constexpr int RESULT_MSG_X = 350;             // リザルトメッセージX座標
	static constexpr int RESULT_MSG_Y = 510;             // リザルトメッセージY座標
	static constexpr int RESULT_REWARD_Y = 545;          // 報酬メッセージY座標

	static constexpr int REWARD_RANDOM_BASE = 15;        // 報酬計算のベース値
	static constexpr int REWARD_RANDOM_RANGE = 20;       // 報酬計算の乱数範囲
	static constexpr int REWARD_RANDOM_OFFSET = 10;      // 報酬計算のオフセット値


	// スクロールガイド（▲▼）位置調整用オフセット
	static constexpr int SCROLL_UP_ARROW_OFFSET_Y = 20;     // 上矢印のY上方向オフセット
	static constexpr int SCROLL_DOWN_ARROW_OFFSET_Y = 10;     // 下矢印のY下方向オフセット

	//現在のターンメッセージの位置
	static constexpr int TURN_MSG_X = 0;
	static constexpr int TURN_MSG_Y = 80;

	//プレイヤーのHPメッセージの位置
	static constexpr int PLAYER_HP_MSG_X = 0;
	static constexpr int PLAYER_HP_MSG_Y = 120;

	//エネミーのHPメッセージの位置
	static constexpr int ENEMY_HP_MSG_X = 0;
	static constexpr int ENEMY_HP_MSG_Y = 140;

	//コマンドの位置
	static constexpr int COMMAND_MSG_X = 400;
	static constexpr int COMMAND_MSG_Y = 600;

	static constexpr int MAGIC_DESC_OFFSET_X = 170;      // 魔法説明欄のXオフセット
	static constexpr int MAGIC_LINE_HEIGHT = 30;         // 魔法一覧の1行の高さ
	static constexpr int SCROLL_ARROW_OFFSET_X = 50;     // スクロール矢印のXオフセット

	static constexpr int MAGIC_DESC_TEXT_OFFSET_Y = 25;   // 魔法説明本文のY座標オフセット

	//敵とプレイヤー行動を表示するメッセージの位置
	static constexpr int BATTLE_MSG_X = 350;
	static constexpr int BATTLE_MSG_Y = 510;

	//次に進むメッセージの位置
	static constexpr int NEXT_MSG_X = 0;
	static constexpr int NEXT_MSG_Y = 190;

	static constexpr int MAX_DISPLAY = 3;//同時に画面に表示したい最大件数（枠のサイズ）

	//アニメーションナンバー
	static constexpr int ANIM_ACT_1 = 0;
	static constexpr int ANIM_ACT_2 = 1;
	static constexpr int ANIM_ACT_3 = 2;
	static constexpr int ANIM_DAMAGE = 3;
	static constexpr int ANIM_DEAD = 4;

	static constexpr int ASTROLOGY_DIVISOR = 5;			//占星術ボーナス除数
	static constexpr int ASTROLOGY_RANDAM_VALUE = 90;	//ranndom関数の最大値

	static constexpr int MARTIALARTS_DIVISOR = 5;			//武術ボーナス除数
	
	//呪いのターンを６に戻す
	static constexpr int ENEMY_CURS_TURN = 6;

	//エクストラステージ出現ターン
	static constexpr int POP_UP_EXTRA_STAGE_TURN = 17;

	//エクストラステージの経験値
	static constexpr int EXTRA_STAGE_EXP = 450;
	
	static constexpr int RANDOM_MAX = 99;	//ランダム関数の最大値
	static constexpr int FREEZE_STUN_CHANCE = 60;        // 凍結時に関数・行動が不可になる確率(%)
	static constexpr int FLASH_MISS_CHANCE = 50;         // 閃光時に攻撃が外れる確率(%)
	static constexpr int MAGIC_ATTACK_DIVISOR = 2;         // 魔法攻撃力の半分を加算するための除数
	static constexpr int CRIT_MULTIPLIER_NORMAL = 2;     // 通常の会心倍率
	static constexpr int CRIT_MULTIPLIER_BOOSTED = 6;    // 極聖光発動時の会心倍率
	static constexpr int AUTO_REGEN_DIVISOR = 10;        // エリクサー自動回復の除数 (最大HP/10)
	static constexpr int POISON_DAMAGE_DIVISOR = 16;     // 毒ダメージの除数 (HP/16)
	static constexpr int DEFAULT_STATUS_TURNS = 4;       // 状態異常の基本継続ターン数


	QuestPhase(PlayerStatus* playerStatus,GameScene& gameScene,bool isHellQuest = false);//デフォルトコンストラクタ
	virtual~QuestPhase(void);		//デストラクタ

	void Update(void) override;		//更新処理
	void Draw(void) override;		//描画処理
	virtual bool IsFinished() const override;//フェーズが終了したかどうかを親に伝える

private:
	
	std::vector<ActionUnit> actionOrder_;	//行動の順番を管理するためのリスト
	std::string tutorialMessage_;			//チュートリアル用のメッセージ
	std::string battleMessage_;				//バトル用のメッセージ
	std::vector<std::string> subActionMessages_; //サブアクションのメッセージを管理するリスト
	std::vector<std::string> magicTypeMessages_; //魔法の種類のメッセージを管理するリスト
	
	GameScene& gameScene_;			//親の情報を渡す
	std::unique_ptr<Stage> stage_;	//ステージの情報を渡す
	PlayerStatus* playerStatus_;	//プレイヤーのステータスの情報を渡す
	std::unique_ptr<MagicDataBase> magicDataBase_;		//魔法データベースのポインタ
	InputManager& ins_ = InputManager::GetInstance();	//inputManagerのインスタンスを取得

	//現在のバトルステップを管理する変数
	BATTLE_STEP battleStep_ = BATTLE_STEP::COMMAND_SELECTION;

	//プレイヤーの選択したコマンドを管理する変数
	COMMAND command_; 

	//状態異常系
	STATUS_EFFECT enemyStatusEffect_ = STATUS_EFFECT::NONE;	//敵の状態異常を管理する変数
	int statusTurns_ = 4;		//状態異常の残りターン数
	int enemyCurs_ = 6;			//呪いの残りターン数
	int poisonCnt_ = 0;			//毒の呪いカウント 

	//クエスト場所系
	QUEST_LOCATION location_;		//配列
	std::string locationRewardMsg_;	//メッセージ用
	std::vector<std::string> locationMenu_;				//メニューに表示する文字列を記憶する用
	std::vector<QUEST_LOCATION> selectableLocations_;	//メニューに対応するenumを記憶する用
	int difficultyCursor_ = 0;                //難易度選択用のカーソル

	//魔法系
	std::vector<MagicData> availableMagics_; //現在のカテゴリの魔法リスト（裏管理用）
	MagicData selectedMagic_;                //プレイヤーが最終決定した魔法データ
	bool wasMagicUsedLastTurn_ = false;		 //前のターンに魔法を使ったか
	bool magicUsedThisTurn_ = false;		 //今のターンに魔法を使ったか（更新用）

	//敵の管理
	Enemy* activeEnemy_ = nullptr; //現在戦っている敵のポインタ
	int battleTurn_ = 1;		   //現在のバトルターン数
	int currentWave_ = 1;          //現在の連戦数（1戦目からスタート）
	const int MAX_WAVES = 3;       //1回の遠征での最大連戦数（例：3連戦）
	const int EXTER_WAVES = 5;     //エクストラモードの最大連戦数（例：5連戦）

	bool isFinished_ = false; //フェーズが終了したかどうかを管理するフラグ
	bool isHellQuest_ = false;//激ムズクエスト突入したか

	int messageBoxImg_;	//メッセージボックスの画像ハンドル

	int currentActionIdx_ = 0;	//行動リストの何番目かを指す
	int subMenuCursor_ = 0;		//サブメニューのカーソル

	int magicMenuCursor_ = 0;		//魔法メニューのカーソル
	int chosenMagicIdx_ = 0;		//選択した魔法の種類のインデックス

	int bgImg_ = -1; //背景画像のハンドル
	int playerImg_ = -1; //プレイヤーの画像ハンドル	

	void ManageTurn(void);//ターンを管理する関数
	
	void ProcessDifficulty(void);		//難易度選択の処理
	void ProcessPlayerAction(void);		//プレイヤーの行動の処理などをここに書く
	void ProcessPlayerSubAction(void);	//プレイヤーの行動の処理などをここに書く
	void MagicSelection();				//魔法選択の処理
	void DetermineActionOrder(void);	//行動の順番を決定する関数
	void ProcessActionLoop(void);		//行動の順番に従って処理を行う関数
	
	void playerturnAction(void);		//プレイヤーの行動処理
	void enemyturnAction(void);			//敵の行動処理

	void CheckEnemyDeath(void);		//敵の死亡判定
	void ProcessStatusEffect(void);	//状態異常
	void DisplayResult(void);		//結果を表示する関数

	void DrawMagicSelection();	//魔法選択の描画処理

	void ProcessTutorial(void);//チュートリアル
	void DrawTutorial(void);//チュートリアルの描画処理

	void DrawCommandSelection(void);//Draw関数内でコマンド選択の描画を行う関数
	
	bool IsConfirmPressed()const;	//決定ボタンが押されたかどうかを判定する関数
};