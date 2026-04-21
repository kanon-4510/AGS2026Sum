#include <DxLib.h>
#include "../../Application.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/InputManager.h"
#include "JobChangePhase.h"

void JobChangePhase::Update(void)
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

void JobChangePhase::Draw(void)
{
	DrawString(0, 0, "Scene : Job Change", 0xFFFFFF);
}

bool JobChangePhase::IsFinished() const
{
	return false;
}
