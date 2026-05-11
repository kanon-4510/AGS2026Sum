#pragma once

#include "PhaseBase.h"
#include "../../Object/PlayerStatus.h"

class QuestPhase : public PhaseBase
{
public:

	enum class COMMAND
	{
		ATTACK,
		MAGIC,
		ITEM,
		MAX
	};

	static constexpr int COUNT_MAX = 100; // フェーズが終了するまでのカウントの最大値

	QuestPhase(PlayerStatus* playerStatus);		//デフォルトコンストラクタ
	//~QuestPhase(void);		//デストラクタ

	void Update(void) override;		//更新処理
	void Draw(void) override;		//描画処理

	// フェーズが終了したかどうかを親に伝える
	virtual bool IsFinished() const override;

private:

	//プレイヤーのステータスの情報を渡す
	PlayerStatus* playerStatus_;

	//inputManagerのインスタンスを取得
	InputManager& ins_ = InputManager::GetInstance();

	COMMAND command_; // プレイヤーの選択したコマンドを管理する変数

	int timer_ = 0; // フェーズの経過時間を管理するタイマー
	int enemyHp_ = 20; //敵のHPを管理する変数（仮）

	bool isFinished_ = false; //フェーズが終了したかどうかを管理するフラグ

	//プレイヤーの行動の処理などをここに書く
	void ProcessPlayerAction();

	//ダメージ処理などをここに書く
	void ProcessDamage();
};

