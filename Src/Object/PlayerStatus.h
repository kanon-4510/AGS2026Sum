#pragma once
#include "BaseStatus.h"

class PlayerStatus : public BaseStatus
{
public:
	int level;

	PlayerStatus();

	void Draw();

	void Death() override;
};

