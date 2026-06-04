#pragma once

#include "PhaseBase.h"
#include "../../Object/PlayerStatus.h"

class FinalPhase : public PhaseBase
{
public:
	//バトルのステップ
	enum class BATTLE_STEP
	{
		COMMAND_SELECTION,		//コマンド選択
		COMMAND_SUB_SELECTION,	//サブコマンド選択
		DETERMINE,				//コマンド決定
		ACTION_LOOP,			//行動ループ
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

	static constexpr int MAX_HP = 999; //bossEnemyの最大HPの仮の値
	static constexpr int POWER = 50;	//bossEnemyの最大POWの仮の値
	static constexpr int SPEED = 50;	//bossEnemyの最大SPEEDの仮の値

	int bossEnemyMaxHp_ = MAX_HP;	//敵の最大HPを管理する変数（仮）
	int bossEnemyHp_ = MAX_HP;		//敵のHPを管理する変数（仮）
	int bossEnemyPow_ = POWER;		//敵のPOWを管理する変数（仮）
	int bossEnemySpeed_ = SPEED;	//敵のSPEEDを管理する変数（仮）
	bool bossEnemyFlag_ = true;		//敵の行動のフラグ（仮）

	FinalPhase(PlayerStatus* playerStatus);		//デフォルトコンストラクタ
	//~FinalPhase(void);		//デストラクタ

	void Update(void) override;		//更新処理
	void Draw(void) override;		//描画処理

	// フェーズが終了したかどうかを親に伝える
	virtual bool IsFinished() const override;

private:

	//行動の順番を管理するためのリスト
	std::vector<ActionUnit> actionOrder_;//行動の順番を管理するためのリスト
	std::string battleMessage_;			 //画面に表示する文字列
	std::vector<std::string> subActionMessages_; //サブアクションのメッセージを管理するリスト

	//プレイヤーのステータスの情報を渡す
	PlayerStatus* playerStatus_;

	//inputManagerのインスタンスを取得
	InputManager& ins_ = InputManager::GetInstance();

	//現在のバトルステップを管理する変数
	BATTLE_STEP battleStep_ = BATTLE_STEP::COMMAND_SELECTION;

	//プレイヤーの選択したコマンドを管理する変数
	COMMAND command_;

	//------フラグ---------
	bool isFinished_ = false; //フェーズが終了したかどうかを管理するフラグ

	//魔法攻撃のインターバル(1ターン)
	bool wasMagicUsedLastTurn_ = false; // 前のターンに魔法を使ったか
	bool magicUsedThisTurn_ = false;    // 今のターンに魔法を使ったか（更新用）

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

	//結果を表示する関数
	void DisplayResult(void);

	//Draw関数内でコマンド選択の描画を行う関数
	void DrawCommandSelection(void);
};
