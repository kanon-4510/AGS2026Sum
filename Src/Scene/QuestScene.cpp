#include <DxLib.h>
#include "../Application.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "QuestScene.h"

void QuestScene::Init(void)
{
}

void QuestScene::Update(void)
{
	auto& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_RETURN))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::CLEAR);
	}
}

void QuestScene::Draw(void)
{
	DrawString(0, 0, "Scene : Quest", 0xFFFFFF);
}

void QuestScene::Release(void)
{
}
