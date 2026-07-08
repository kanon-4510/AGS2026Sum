#pragma once
#include "PhaseBase.h"
#include "../../Object/PlayerStatus.h"
#include <vector>
#include <string>

class EventPhase : public PhaseBase
{
public:
	EventPhase(PlayerStatus* playerStatus);
	void Update(void) override;
	void Draw(void) override;
	bool IsFinished(void)const override;
private:
	PlayerStatus* playerStatus_;
	std::vector<std::string> routeMenu_;
	int cursor_ = 0;
	int waitTimer_ = 0; //入力待ち＆放置カウント用
	const int TIMEOUT_FRAMES = 1800;
	bool isFinished_ = false; // フェーズが終了したかどうかを管理するフラグ
};