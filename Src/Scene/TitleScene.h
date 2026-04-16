//#pragma once

#include "../Application.h"
#include "SceneBase.h"

class TitleScene : public SceneBase
{
public:
	//TitleScene(void);		//デフォルトコンストラクタ
	//~TitleScene(void);	//デストラクタ

	void Init(void) override;		//初期化処理
	void Update(void) override;		//更新処理
	void Draw(void) override;		//描画処理
	void Release(void) override;	//解放処理

private:
	//メンバー変数
	int titleImage_;		//タイトル画像の格納領域
};
