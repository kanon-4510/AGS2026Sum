//#pragma once

#include "SceneBase.h"

class GameScene : public SceneBase
{
public:
	//GameScene(void);		//デフォルトコンストラクタ
	//~GameScene(void);		//デストラクタ

	void Init(void) override;		//初期化処理
	void Update(void) override;		//更新処理
	void Draw(void) override;		//描画処理
	void Release(void) override;	//解放処理

private:
	int bgImage_;			//背景画像格納領域
};