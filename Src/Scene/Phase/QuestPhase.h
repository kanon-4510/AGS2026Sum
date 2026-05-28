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
	struct ActionUnit 
	{
		std::string name; //表示用（プレイヤー or 敵の名前）
		int speed;        //素早さ（ソート用）
		bool isPlayer;    //誰の行動か判別用
		int id;           //敵が複数いる場合の識別番号
		int command;      //選択した行動（0:攻撃, 1:防御, 2:アイテムなど）
		int targetIdx;    //攻撃対象が誰か
	};

	//難易度
	enum class DIFFICULTY{
		EASY,
		NORMAL,
		HARD,
		MAX
	};
	
	enum class BATTLE_STEP
	{
		DIFFICULTY_SELECTION,	//難易度選択
		COMMAND_SELECTION,		//コマンド選択
		DETERMINE,				//コマンド決定
		ACTION_LOOP,			//行動ループ
		RESULT,					//結果表示
		MAX
	};

	enum class COMMAND
	{
		ATTACK,
		MAGIC,
		ITEM,
		MAX
	};

	//経験値（仮）
	static constexpr int EXP_GAIN = 10;

	static constexpr int MAX_HP = 30; //
	static constexpr int HP_RECOVERY_AMOUNT = 20; //アイテム使用時のHP回復量

	//敵の強さの上がり幅
	static constexpr float ENEMY_INCREASE = 0.7f;
	static constexpr float HARD_INCREASE = 1.5f;

	static constexpr int ENEMY_HP = MAX_HP;		//敵のHPを管理する変数（仮）
	static constexpr int ENEMY_POW = 5;			//敵のPOWを管理する変数（仮）
	static constexpr int ENEMY_SPEED = 10;		//敵のSPEEDを管理する変数（仮）

	QuestPhase(PlayerStatus* playerStatus,GameScene& gameScene);		//デフォルトコンストラクタ
	virtual~QuestPhase(void);		//デストラクタ

	void Update(void) override;		//更新処理
	void Draw(void) override;		//描画処理

	//フェーズが終了したかどうかを親に伝える
	virtual bool IsFinished() const override;
private:
	std::vector<ActionUnit> actionOrder_;//行動の順番を管理するためのリスト
	std::string battleMessage_;			 //画面に表示する文字列

	PlayerStatus* playerStatus_;//プレイヤーのステータスの情報を渡す
	GameScene& gameScene_;		//親の情報を渡す
	InputManager& ins_ = InputManager::GetInstance();//inputManagerのインスタンスを取得

	//プレイヤーの選択したコマンドを管理する変数
	COMMAND command_; 
	
	//難易度を管理する変数
	DIFFICULTY difficulty_ = DIFFICULTY::EASY;

	//現在のバトルステップを管理する変数
	BATTLE_STEP battleStep_ = BATTLE_STEP::COMMAND_SELECTION;

	//魔法攻撃のインターバル(1ターン)
	bool wasMagicUsedLastTurn_ = false; //前のターンに魔法を使ったか
	bool magicUsedThisTurn_ = false;    //今のターンに魔法を使ったか（更新用）

	Enemy* activeEnemy_ = nullptr; //現在戦っている敵のポインタ
	int currentWave_ = 1;          //現在の連戦数（1戦目からスタート）
	const int MAX_WAVES = 3;       //1回の遠征での最大連戦数（例：3連戦）
	
	//------フラグ---------
	bool isFinished_ = false; //フェーズが終了したかどうかを管理するフラグ

	//-------変数---------
	int currentActionIdx_ = 0; //行動リストの何番目かを指す

	//------関数---------
	
	//ターンを管理する関数
	void ManageTurn(void);
	
	//難易度選択の処理
	void ProcessDifficulty(void);

	//行動の順番を決定する関数
	void DetermineActionOrder(void);
	//行動の順番に従って処理を行う関数
	void ProcessActionLoop(void);
	//結果を表示する関数
	void DisplayResult(void);
	//プレイヤーの行動の処理などをここに書く
	void ProcessPlayerAction(void);

	//Draw関数内でコマンド選択の描画を行う関数
	void DrawCommandSelection(void);

};

