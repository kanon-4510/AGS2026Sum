#include <DxLib.h>
#include "../Application.h"
#include "Fader.h"

Fader::Fader(void)
{
	state_ = STATE::NONE;
	alpha_ = 0.0f;
	isPreEnd_ = true;
	isEnd_ = true;
}

Fader::~Fader(void)
{
}

void Fader::Init(void)
{
	state_ = STATE::NONE;
	alpha_ = 0.0f;
	alpha_ = 0;		//int
	isPreEnd_ = true;
	isEnd_ = true;
}

void Fader::Update(void)
{

	if (isEnd_)
	{
		// フェード処理が終了していたら何もしない
		return;
	}
	
	if (isPreEnd_)
	{
		// 1フレーム後(Draw後)に終了とする
		isEnd_ = true;
		return;
	}

	switch (state_)
	{

	case STATE::NONE:
		return;

	case STATE::FADE_OUT:
		alpha_ += SPEED_ALPHA;
		if (alpha_ > 255)
		{
			// フェード終了
			alpha_ = 255;
			isPreEnd_ = true;
		}
		break;

	case STATE::FADE_IN:
		alpha_ -= SPEED_ALPHA;
		if (alpha_ < 0)
		{
			// フェード終了
			alpha_ = 0;
			isPreEnd_ = true;
		}
		break;

	default:
		return;

	}

}

void Fader::Draw(void)
{

	switch (state_)
	{
	case STATE::NONE:
		break;
	case STATE::FADE_OUT:
	case STATE::FADE_IN:
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(alpha_));
		DrawBox(
			0, 0,
			Application::SCREEN_SIZE_X,
			Application::SCREEN_SIZE_Y,
			0x000000, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		break;
	}

}

Fader::STATE Fader::GetState(void)
{
	return state_;
}

bool Fader::IsEnd(void)
{
	return isEnd_;
}

void Fader::SetFade(STATE state)
{
	state_ = state;
	if (state_ != STATE::NONE)
	{
		isPreEnd_ = false;
		isEnd_ = false;
	}

	if (state_ == STATE::FADE_IN)
	{
		//フェードインの時の処理を追加しましょう。
		alpha_ = 255.0f;
	}
	else if (state_ == STATE::FADE_OUT)
	{
		//フェードアウトの時の処理を追加しましょう。
		alpha_ = 0.0f;
	}
}
