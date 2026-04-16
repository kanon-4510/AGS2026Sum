#pragma once

class Vector2;

class Vector2F
{
public:

	//デフォルトコンストラクタ
	Vector2F(void);

	//コンストラクタ
	Vector2F(float ix, float iy);

	//デストラクタ
	~Vector2F(void);

	//Vector2へ変換する関数
	Vector2 ToVector2(void);

	float x;
	float y;

private:
};