#pragma once
#include "BaseStatus.h"
#include "Skill.h"
#include "JobData.h"
#include <string>
#include <vector>

class PlayerStatus : public BaseStatus
{
public:
	int level_;
	Skill skill_;	//‹Z”\
	std::string currentJob;	//Œ»İ‚ÌE‹Æ

	PlayerStatus();

	//•`‰æˆ—
	void Draw();

	//E‹Æ‚Ì‰Šú‰»
	void InitJob();

	//€–Sˆ—
	void Death() override;

	//ƒŒƒxƒ‹ƒAƒbƒv‚µ‚½‚Ìˆ—
	void LevelUp();

	//“]E‰Â”\‚©‚Ç‚¤‚©‚ğ”»’è‚·‚éŠÖ”
	bool CanEvolve(const JobData& job);
};

