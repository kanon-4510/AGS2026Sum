#pragma once
#include "../../Object/PlayerStatus.h"
#include "PhaseBase.h"

class GameScene;

class ClassWorkPhase : public PhaseBase
{
public:
	//メッセージボックスの表示位置
	static constexpr int MESSAGE_BOX_X = 900;
	static constexpr int MESSAGE_BOX_Y = 80;

	//チュートリアルの表示位置
	static constexpr int TUTORIAL_X = MESSAGE_BOX_X + 15;
	static constexpr int TUTORIAL_Y = MESSAGE_BOX_Y + 20;

	static constexpr int TEXT_POS_X = 150;	//授業のテキスト描画位置X
	static constexpr int TEXT_POS_Y = 150;	//授業のテキスト描画位置Y
	
	static constexpr int bonusX = 300;	//技能ボーナスの描画位置X
	static constexpr int bonusY = 350;	//技能ボーナスの描画位置Y

	enum CLASSWORK_SELECT
	{
		PHARMACY,       //薬学
		MARTIALARTS,    //武術
		MAGICKNOWLEDGE, //魔法知識
		FAITH,          //信仰
		ARCHAEOLOGY,    //考古学
		ASTROLOGY,      //占星術
		MAX				//最大値
	};

	static constexpr int SKILL_UP = 50;	//技能上昇幅

	ClassWorkPhase(PlayerStatus* playerstatus, GameScene& gameScene);		//デフォルトコンストラクタ
	~ClassWorkPhase(void);		//デストラクタ

	void Update(void) override;		//更新処理
	void Draw(void) override;		//描画処理

	void DrawSkillBonus(void);	//技能ボーナスの描画処理

	// フェーズが終了したかどうかを親に伝える
	virtual bool IsFinished() const override;
private:
	GameScene& gameScene_;
	PlayerStatus* playerStatus_;
	InputManager& ins_ = InputManager::GetInstance();
	int timer_ = 0; //フェーズの経過時間を管理するタイマー
	int bgImg_ = -1; //背景画像のハンドル
	int playerImg_ = -1; //プレイヤーの画像ハンドル
	int messageBoxImg_ = -1; //メッセージボックスの画像ハンドル

	bool isFinished_ = false; // フェーズが終了したかどうかを管理するフラグ

	CLASSWORK_SELECT select_;

	void ProcessClassworkSelection(void);	//履修選択の処理
	void ProcessClassworkDecision(void);	//履修選択の確定

	//チュートリアル
	void ProcessTutorial(void);
	
	//チュートリアルの描画処理
	void DrawTutorial(void);
};