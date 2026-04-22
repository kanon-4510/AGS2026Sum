#include "BaseStatus.h"

BaseStatus::BaseStatus()
{
	maxHp = 10;
	hp = maxHp;
	pow = 10;
	mPow = 10;
	speed = 10;
	luck = 10;
}

void BaseStatus::Damage(int damage)
{
	hp -= damage;
	if (hp < 0)
	{
		hp = 0;
		Death();
	}
}
