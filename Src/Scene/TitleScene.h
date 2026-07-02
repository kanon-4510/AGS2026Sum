//#pragma once

#include "../Application.h"
#include "../Manager/InputManager.h"
#include "SceneBase.h"

class TitleScene : public SceneBase
{
public:
	enum TITLE_SELECTION
	{
		START_GAME,	//ゲームスタート
		TUTORIAL,	//チュートリアル
		MAX			//最大値
	};

	//TitleScene(void);		//デフォルトコンストラクタ
	//~TitleScene(void);	//デストラクタ

	void Init(void) override;		//初期化処理
	void Update(void) override;		//更新処理
	void Draw(void) override;		//描画処理
	void Release(void) override;	//解放処理

private:
	InputManager& ins_ = InputManager::GetInstance();

	TITLE_SELECTION titleSelection_;	//タイトルの選択肢

	//メンバー変数
	int titleImage_;		//タイトル画像の格納領域
	int currentSelection_;	//現在の選択肢

	void ProcessTitleSelection(void);	//タイトル選択の処理
	void ProcessTitleDecision(void);	//タイトル決定の処理
};
