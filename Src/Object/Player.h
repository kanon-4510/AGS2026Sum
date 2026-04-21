#pragma once
#include "PlayerStatus.h"

class Player
{
public:

	Player();
	~Player();

	const PlayerStatus& getStatus() const;

private:

	PlayerStatus status;
};

