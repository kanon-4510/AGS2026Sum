#include "Player.h"

Player::Player()
{
	//プレイヤー生成時にステータスをメモリ確保
	status = new PlayerStatus();
}

Player::~Player()
{
	delete status;
}

const PlayerStatus* Player::getStatus() const
{
	return status;
}
