#pragma once

class BaseStatus
{
public:
	int maxHp_;
	int hp_;
	int power_;
	int magic_;
	int speed_;
	int luck_;

	BaseStatus();
	virtual ~BaseStatus() {};

	//ƒ_ƒ[ƒWˆ—
	virtual void Damage(int damage);

	virtual void Death() {};
};

