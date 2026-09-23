#pragma once
#include "../../Object/PlayerStatus.h"
#include "PhaseBase.h"

class GameScene;

class ClassWorkPhase : public PhaseBase
{
public:
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


	//描画座標・レイアウト関連定数
	static constexpr int BG_POS_X = 0;
	static constexpr int BG_POS_Y = 0;
	static constexpr int PLAYER_POS_X = 700;
	static constexpr int PLAYER_POS_Y = 200;

	static constexpr int TEXT_POS_X = 150;	//授業のテキスト描画位置X
	static constexpr int TEXT_POS_Y = 150;	//授業のテキスト描画位置Y
	static constexpr int MENU_Y_INTERVAL = 50;  // 選択項目の縦間隔

	static constexpr int BONUS_POS_X = 300;     // 技能ボーナス描画位置X
	static constexpr int BONUS_POS_Y = 350;     // 技能ボーナス描画位置Y

	static constexpr int BONUS_DESC_OFFSET_Y = 200;       // 説明文のYオフセット
	static constexpr int BONUS_EFFECT_OFFSET_Y = 30;       // 効果内容のYオフセット
	static constexpr int BONUS_NEXT_LEVEL_OFFSET_Y = 55;   // 次のレベルまでの表示Yオフセット

	//フォントサイズ関連定数
	static constexpr int MENU_FONT_SIZE = 24;
	static constexpr int DESC_FONT_SIZE = 23;
	static constexpr int TUTORIAL_FONT_SIZE = 20;

	//ゲーム計算・バランス数値
	static constexpr int SKILL_RAND_RANGE = 30;   // 上昇幅の乱数範囲
	static constexpr int SKILL_RAND_OFFSET = 15;  // 乱数のオフセット (-15 ~ +14)

	// 技能計算の除数
	constexpr static int PHARMACY_DIVISOR = 7;       // 薬学ボーナス除数
	constexpr static int MARTIAL_ARTS_DIVISOR = 5;   // 武術ボーナス除数
	constexpr static int MAGIC_DIVISOR = 50;		 // 魔法ボーナス除数
	constexpr static int FAITH_DIVISOR = 15;         // 信仰ボーナス除数
	constexpr static int ARCHAEOLOGY_DIVISOR = 8;    // 考古学ボーナス除数
	constexpr static int ASTROLOGY_DIVISOR = 5;      // 占星術ボーナス除数

	static constexpr int SKILL_UP = 50;	//技能上昇幅

	//チュートリアル関連
	static constexpr int TUTORIAL_TARGET_TURN = 2; // チュートリアル対象ターン

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