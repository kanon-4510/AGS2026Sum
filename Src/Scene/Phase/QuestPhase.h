#pragma once

#include "PhaseBase.h"
#include "../../Scene/GameScene.h"
#include "../../Object/PlayerStatus.h"
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

//効果の種類
enum class MAGIC_TYPE
{
	ATTACK,//魔法攻撃
	HEAL,  //回復
	DEBUFF,//弱化
	MAX
};

//状態異常の種類
enum class STATUS_EFFECT
{
	NONE,	//なし
	POISON,	//毒　(紫)
	FREEZE,	//凍結(青)
	FLASH,	//閃光(黄)
	CURSE,	//呪い(赤)
	SILENCE,//沈黙(緑)
	MAX
};

struct MagicData
{
	int id;                     //識別ID
	std::string name;           //魔法名
	MAGIC_TYPE type;            //魔法の種類
	float powerMultiplier;      //魔力に乗算する倍率（低:0.7, 中:1.3, 高:2.2 など）
	STATUS_EFFECT ailment;      //付与する状態異常（なければ NONE）
	int ailmentChance;          //状態異常の確率（0〜100%）
	int reqMagicKnowledge;      //習得に必要な「魔法知識」の数値
	bool isDrain = false;       //HP吸収魔法かどうかのフラグ
	bool curesStatus = false;   //状態異常も一緒に治すかどうかのフラグ
};

class GameScene;
class Enemy;
class QuestPhase : public PhaseBase
{
public:
	//難易度
	enum class DIFFICULTY
	{
		EASY,
		NORMAL,
		HARD,
		MAX
	};
	
	//バトルのステップ
	enum class BATTLE_STEP
	{
		DIFFICULTY_SELECTION,	//難易度選択
		COMMAND_SELECTION,		//コマンド選択
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
	//メッセージの表示位置

	//難易度選択のメッセージの位置
	static constexpr int DIFFICULTY_MSG_X = 0;
	static constexpr int DIFFICULTY_MSG_Y = 40;

	//���ɐi�ރ��b�Z�[�W�̈ʒu
	static constexpr int DIFFICULTY_ENTER_MSG_X = 0;
	static constexpr int DIFFICULTY_ENTER_MSG_Y = 400;


	//���݂̃^�[�����b�Z�[�W�̈ʒu
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
	static constexpr int COMMAND_MSG_X = 0;
	static constexpr int COMMAND_MSG_Y = 210;

	//敵とプレイヤー行動を表示するメッセージの位置
	static constexpr int BATTLE_MSG_X = 0;
	static constexpr int BATTLE_MSG_Y = 170;

	//次に進むメッセージの位置
	static constexpr int NEXT_MSG_X = 0;
	static constexpr int NEXT_MSG_Y = 190;
	
	//�o���l�i���j

	//経験値（仮）
	static constexpr int EXP_GAIN = 10;

	static constexpr int MAX_HP = 30; //
	static constexpr int HP_RECOVERY_AMOUNT = 20; //アイテム使用時のHP回復量

	static constexpr int MAX_DISPLAY = 6;//同時に画面に表示したい最大件数（枠のサイズ）

	//�A�j���[�V�����i���o�[
	static constexpr int ANIM_ACT_1 = 0;
	static constexpr int ANIM_ACT_2 = 1;
	static constexpr int ANIM_ACT_3 = 2;
	static constexpr int ANIM_DAMAGE = 3;
	static constexpr int ANIM_DEAD = 4;

	QuestPhase(PlayerStatus* playerStatus,GameScene& gameScene,bool isHellQuest = false);//デフォルトコンストラクタ
	virtual~QuestPhase(void);		//デストラクタ

	void Update(void) override;		//更新処理
	void Draw(void) override;		//描画処理

	//フェーズが終了したかどうかを親に伝える
	virtual bool IsFinished() const override;
private:
	std::vector<ActionUnit> actionOrder_;	//行動の順番を管理するためのリスト
	std::string tutorialMessage_;			//チュートリアル用のメッセージ
	std::string battleMessage_;				//バトル用のメッセージ
	std::vector<std::string> subActionMessages_; //サブアクションのメッセージを管理するリスト
	std::vector<std::string> magicTypeMessages_; //魔法の種類のメッセージを管理するリスト
	
	PlayerStatus* playerStatus_;//プレイヤーのステータスの情報を渡す
	GameScene& gameScene_;		//親の情報を渡す
	InputManager& ins_ = InputManager::GetInstance();//inputManagerのインスタンスを取得

	//難易度を管理する変数
	DIFFICULTY difficulty_ = DIFFICULTY::EASY;

	//現在のバトルステップを管理する変数
	BATTLE_STEP battleStep_ = BATTLE_STEP::COMMAND_SELECTION;

	//プレイヤーの選択したコマンドを管理する変数
	COMMAND command_; 

	//状態異常系
	STATUS_EFFECT statusEffect_;//状態異常を管理する変数
	STATUS_EFFECT enemyStatusEffect_ = STATUS_EFFECT::NONE;	//敵の状態異常を管理する変数
	int statusTurns_ = 4;		//状態異常の残りターン数
	int enemyCurs_ = 6;			//敵の呪いカウント 

	//クエスト場所系
	QUEST_LOCATION location_;		//配列
	std::string locationRewardMsg_;	//メッセージ用
	int bgImageHandle_;				//背景画像
	std::vector<std::string> locationMenu_;
	std::vector<QUEST_LOCATION> selectableLocations_; //メニューに対応するenumを記憶する用
	int difficultyCursor_ = 0;                //難易度選択用のカーソル

	//魔法
	bool wasMagicUsedLastTurn_ = false;		 //前のターンに魔法を使ったか
	bool magicUsedThisTurn_ = false;		 //今のターンに魔法を使ったか（更新用）
	std::vector<MagicData> availableMagics_; //現在のカテゴリの魔法リスト（裏管理用）
	MagicData selectedMagic_;                //プレイヤーが最終決定した魔法データ

	Enemy* activeEnemy_ = nullptr; //現在戦っている敵のポインタ
	int battleTurn_ = 1;		   //現在のバトルターン数
	int currentWave_ = 1;          //現在の連戦数（1戦目からスタート）
	const int MAX_WAVES = 3;       //1回の遠征での最大連戦数（例：3連戦）

	//------フラグ---------
	bool isFinished_ = false; //フェーズが終了したかどうかを管理するフラグ
	bool isHellQuest_ = false;//激ムズクエスト突入したか

	//-------変数---------
	int currentActionIdx_ = 0;	//行動リストの何番目かを指す
	int subMenuCursor_ = 0;		//サブメニューのカーソル

	int magicMenuCursor_ = 0;		//魔法メニューのカーソル
	int chosenMagicIdx_ = 0;		//選択した魔法の種類のインデックス

	//------関数---------
	//ターンを管理する関数
	void ManageTurn(void);
	
	//難易度選択の処理
	void ProcessDifficulty(void);
	
	//プレイヤーの行動の処理などをここに書く
	void ProcessPlayerAction(void);
	
	//プレイヤーの行動の処理などをここに書く
	void ProcessPlayerSubAction(void);
	
	//魔法選択の処理
	void MagicSelection();

	//行動の順番を決定する関数
	void DetermineActionOrder(void);
	
	//行動の順番に従って処理を行う関数
	void ProcessActionLoop(void);

	//�G�̎��S����
	void CheckEnemyDeath(void);
	void ProcessStatusEffect(void);//状態異常

	//結果を表示する関数
	void DisplayResult(void);

	//チュートリアル
	void ProcessTutorial(void);

	//Draw
	//--------------
	//チュートリアルの描画処理
	void DrawTutorial(void);

	//Draw関数内でコマンド選択の描画を行う関数
	void DrawCommandSelection(void);
};