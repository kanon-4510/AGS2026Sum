#pragma once

#include "PhaseBase.h"
#include "../../Scene/GameScene.h"
#include "../../Object/PlayerStatus.h"
#include <vector>
#include <string>

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
		MAGIC,	//魔法攻撃
		MAX
	};

	//効果の種類
	enum class MAGIC_TYPE
	{
		MAGIC_ATTACK,	//魔法攻撃
		HEAL,			//回復
		BUFF,			//強化
		DEBUFF,			//弱体化
		MAX
	};

	//状態異常の種類
	enum class STATUS_EFFECT
	{
		NONE,			//なし
		POISON,			//毒
		FREEZE,			//凍結
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
		MAGIC_TYPE magicType;		//魔法の効果の種類を記憶しておく
	};

	//経験値（仮）
	static constexpr int EXP_GAIN = 10;

	static constexpr int MAX_HP = 30; //
	static constexpr int HP_RECOVERY_AMOUNT = 20; //アイテム使用時のHP回復量

	//敵の強さの上がり幅
	static constexpr float ENEMY_INCREASE = 0.7f;
	static constexpr float HARD_INCREASE = 1.5f;

	QuestPhase(PlayerStatus* playerStatus,GameScene& gameScene,bool isHellQuest=false);		//デフォルトコンストラクタ
	virtual~QuestPhase(void);		//デストラクタ

	void Update(void) override;		//更新処理
	void Draw(void) override;		//描画処理

	//フェーズが終了したかどうかを親に伝える
	virtual bool IsFinished() const override;
private:
	std::vector<ActionUnit> actionOrder_;//行動の順番を管理するためのリスト
	std::string battleMessage_;			 //画面に表示する文字列
	std::vector<std::string> subActionMessages_; //サブアクションのメッセージを管理するリスト
	
	PlayerStatus* playerStatus_;//プレイヤーのステータスの情報を渡す
	GameScene& gameScene_;		//親の情報を渡す
	InputManager& ins_ = InputManager::GetInstance();//inputManagerのインスタンスを取得

	//難易度を管理する変数
	DIFFICULTY difficulty_ = DIFFICULTY::EASY;

	//現在のバトルステップを管理する変数
	BATTLE_STEP battleStep_ = BATTLE_STEP::COMMAND_SELECTION;

	//プレイヤーの選択したコマンドを管理する変数
	COMMAND command_; 

	//状態異常を管理する変数
	STATUS_EFFECT statusEffect_;

	//魔法攻撃のインターバル(1ターン)
	bool wasMagicUsedLastTurn_ = false; //前のターンに魔法を使ったか
	bool magicUsedThisTurn_ = false;    //今のターンに魔法を使ったか（更新用）

	Enemy* activeEnemy_ = nullptr; //現在戦っている敵のポインタ
	int currentWave_ = 1;          //現在の連戦数（1戦目からスタート）
	const int MAX_WAVES = 3;       //1回の遠征での最大連戦数（例：3連戦）

	std::vector<std::string> difficultyMenu_; //表示する難易度のリスト
	int difficultyCursor_ = 0;                //難易度選択用のカーソル
	
	//------フラグ---------
	bool isFinished_ = false; //フェーズが終了したかどうかを管理するフラグ
	bool isHellQuest_ = false;//激ムズクエスト突入したか

	//-------変数---------
	int currentActionIdx_ = 0;	//行動リストの何番目かを指す
	int subMenuCursor_ = 0;		// サブメニューのカーソル

	//------関数---------
	//ターンを管理する関数
	void ManageTurn(void);
	
	//難易度選択の処理
	void ProcessDifficulty(void);
	
	//プレイヤーの行動の処理などをここに書く
	void ProcessPlayerAction(void);
	
	//プレイヤーの行動の処理などをここに書く
	void ProcessPlayerSubAction(void);
	
	//行動の順番を決定する関数
	void DetermineActionOrder(void);
	
	//行動の順番に従って処理を行う関数
	void ProcessActionLoop(void);
	
	//状態異常の処理を行う関数
	void ProcessStatusEffect(void);

	//結果を表示する関数
	void DisplayResult(void);

	//Draw関数内でコマンド選択の描画を行う関数
	void DrawCommandSelection(void);

	//結果のメッセージを配列に格納する関数
	void DrawResultMessage();
};