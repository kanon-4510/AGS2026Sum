#include "BaseStatus.h"

BaseStatus::BaseStatus()
{
	maxHp_ = 10;
	hp_ = maxHp_;
	pow_ = 10;
	magic_ = 10;
	speed_ = 10;
	luck_ = 10;
}

void BaseStatus::Damage(int damage)
{
	hp_ -= damage;
	if (hp_ < 0)
	{
		hp_ = 0;
		Death();
	}
}
