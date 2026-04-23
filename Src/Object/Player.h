#pragma once
#include "PlayerStatus.h"

class Player
{
public:

	Player();
	~Player();

	//ステータスのゲッター
	const PlayerStatus* getStatus() const;

private:

	PlayerStatus* status;
};

