#pragma once
#include <memory>
#include "SceneBase.h"

class SceneManager;

class ClearScene : public SceneBase
{
public:
	//モデルの位置
	static constexpr float MODEL_SIZE = 1.3f;
	//モデルの位置
	static constexpr float MODEL_POS = 200.0f;

	//アニメ－ション
	static constexpr float ANIM_SPEED = 40.0f;
	static constexpr int ANIM_DANCE_INDEX = 9;

	//強制遷移
	static constexpr int FORCE_RETURN_TIME = 700;

	ClearScene(void);	//コンストラクタ
	~ClearScene(void);	//デストラクタ

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

private:
};