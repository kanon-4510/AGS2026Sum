#include <DxLib.h>
#include "../../Application.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/InputManager.h"
#include "JobChangePhase.h"

void JobChangePhase::Update(void)
{
	auto& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_SPACE))
	{
		isFinished_ = true;
	}
}

void JobChangePhase::Draw(void)
{
	DrawString(0, 0, "Scene : Job Change", 0xFFFFFF);
}

bool JobChangePhase::IsFinished() const
{
	return isFinished_;
}
