#pragma once


#include "SceneBase.h"

class QuestScene : public SceneBase
{
public:
	//QuestScene(void);		//デフォルトコンストラクタ
	//~QuestScene(void);		//デストラクタ

	void Init(void) override;		//初期化処理
	void Update(void) override;		//更新処理
	void Draw(void) override;		//描画処理
	void Release(void) override;	//解放処理

private:
};

