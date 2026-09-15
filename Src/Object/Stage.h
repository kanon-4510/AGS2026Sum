#pragma once
#include <map>


class Stage
{
public:

	enum class STAGE_TYPE
	{
		PLAINS,		//平原
		FOREST,		//魔法の森
		SHRINE,		//岩山の道場
		CONTINENT,	//魔大陸
		CATHEDRAL,	//壊れた聖堂
		RUINS,		//古代遺跡
		HILL,		//星の丘
		EXTRA,		//エクストラ
		MAX
	};

	static constexpr int DRAW_STAGE_BOARD_POS_X = 630;	//ステージの描画位置X
	static constexpr int DRAW_STAGE_BOARD_POS_Y = 360;	//ステージの描画位置X

	static constexpr int DRAW_BACKGROUND_POS_X = 0;		//ステージの描画位置X
	static constexpr int DRAW_BACKGROUND_POS_Y = -100;	//ステージの描画位置X

	Stage();	//コンストラクタ
	~Stage();	//デストラクタ

	void Init(void);	//初期化処理
	void Update(void);	//更新処理
	void DrawStageBoard(void);	//ステージ選択の描画処理
	void DrawBackground(void);	//クエスト中の背景の描画処理
	void Release(void);	//解放処理

	void SetStageType(STAGE_TYPE type);	//ステージの種類を設定する

private:
	STAGE_TYPE stageType_;	//ステージの種類


	std::map<STAGE_TYPE, int> stageBoard_;	//ステージのボード画像を管理する配列
	std::map<STAGE_TYPE, int> bgImageHandle_;		//クエスト用の背景画像ハンドルを管理するマップ
};

