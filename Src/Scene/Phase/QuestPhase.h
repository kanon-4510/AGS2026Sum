#pragma once

#include "PhaseBase.h"
#include "../../Object/PlayerStatus.h"

class QuestPhase : public PhaseBase
{
public:

	struct ActionUnit {
		std::string name; // 表示用（プレイヤー or 敵の名前）
		int speed;        // 素早さ（ソート用）
		bool isPlayer;    // 誰の行動か判別用
		int id;           // 敵が複数いる場合の識別番号
		int command;      // 選択した行動（0:攻撃, 1:防御, 2:アイテムなど）
		int targetIdx;    // 攻撃対象が誰か
	};


	enum class BATTLE_STEP
	{
		COMMAND_SELECTION,	//コマンド選択
		DETERMINE,			//コマンド決定
		ACTION_LOOP,		//行動ループ
		RESULT,				//結果表示
		MAX
	};

	enum class COMMAND
	{
		ATTACK,
		MAGIC,
		ITEM,
		MAX
	};


	static constexpr int MAX_HP = 30; //

	int enemyMaxHp_ = MAX_HP; //敵の最大HPを管理する変数（仮）
	int enemyHp_ = MAX_HP; //敵のHPを管理する変数（仮）
	int enemyPow_ = 3; //敵のPOWを管理する変数（仮）
	int enemySpeed_ = 20; //敵のSPEEDを管理する変数（仮）
	bool enemyFlag_ = true; //敵の行動のフラグ（仮）

	QuestPhase(PlayerStatus* playerStatus);		//デフォルトコンストラクタ
	//~QuestPhase(void);		//デストラクタ

	void Update(void) override;		//更新処理
	void Draw(void) override;		//描画処理

	// フェーズが終了したかどうかを親に伝える
	virtual bool IsFinished() const override;

private:

	//行動の順番を管理するためのリスト
	std::vector<ActionUnit> actionOrder_;

	//画面に表示する文字列
	std::string battleMessage_; 

	//プレイヤーのステータスの情報を渡す
	PlayerStatus* playerStatus_;

	//inputManagerのインスタンスを取得
	InputManager& ins_ = InputManager::GetInstance();

	//プレイヤーの選択したコマンドを管理する変数
	COMMAND command_; 
	
	//現在のバトルステップを管理する変数
	BATTLE_STEP battleStep_ = BATTLE_STEP::COMMAND_SELECTION;

	//------フラグ---------
	bool isFinished_ = false; //フェーズが終了したかどうかを管理するフラグ

	//魔法攻撃のインターバル(1ターン)
	bool wasMagicUsedLastTurn_ = false; // 前のターンに魔法を使ったか
	bool magicUsedThisTurn_ = false;    // 今のターンに魔法を使ったか（更新用）
	
	//-------変数---------
	int currentActionIdx_ = 0; //行動リストの何番目かを指す


	//------関数---------
	//ターンを管理する関数
	void ManageTurn(void);
	
	//行動の順番を決定する関数
	void DetermineActionOrder(void);
	//行動の順番に従って処理を行う関数
	void ProcessActionLoop(void);
	//結果を表示する関数
	void DisplayResult(void);
	//プレイヤーの行動の処理などをここに書く
	void ProcessPlayerAction(void);

};

