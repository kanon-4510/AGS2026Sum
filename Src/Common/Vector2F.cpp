#include <tgmath.h>
#include "Vector2.h"
#include "Vector2F.h"


//デフォルトコンストラクタ
Vector2F::Vector2F(void)
{
	x = 0.0F;
	y = 0.0F;
}


//コンストラクタ
Vector2F::Vector2F(float ix, float iy)
{
	x = ix;
	y = iy;
}


//デストラクタ
Vector2F::~Vector2F(void)
{

}


//Vector2へ変換する関数
Vector2 Vector2F::ToVector2(void)
{
	Vector2 ret;

	ret.x = static_cast<int>(roundf(x));
	ret.y = static_cast<int>(roundf(y));

	return ret;
}
