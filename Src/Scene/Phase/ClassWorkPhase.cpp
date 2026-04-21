#include <DxLib.h>
#include "../../Application.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/InputManager.h"
#include "ClassWorkPhase.h"

void ClassWorkPhase::Update(void)
{
	auto& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_RETURN))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
	}
	if (ins.IsTrgDown(KEY_INPUT_SPACE))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
	}
}

void ClassWorkPhase::Draw(void)
{
	DrawString(0, 0, "Scene : Class Work", 0xFFFFFF);
}

bool ClassWorkPhase::IsFinished() const
{
	return false;
}
