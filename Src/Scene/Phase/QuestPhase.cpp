#include <DxLib.h>
#include "../../Application.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/InputManager.h"
#include "QuestPhase.h"

void QuestPhase::Update(void)
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

void QuestPhase::Draw(void)
{
	DrawString(0, 0, "Scene : Quest", 0xFFFFFF);
}

bool QuestPhase::IsFinished() const
{
	return false;
}
