#pragma once

class BaseStatus
{
public:
	int maxHp;
	int hp;
	int pow;
	int mPow;
	int speed;
	int luck;

	BaseStatus();
	virtual ~BaseStatus() {};

	//ƒ_ƒ[ƒWˆ—
	virtual void Damage(int damage);

	virtual void Death() {};
};

