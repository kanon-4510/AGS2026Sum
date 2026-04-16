#include <DxLib.h>
#include <sstream>
#include "../Common/Vector2.h"
#include "../Common/Vector2F.h"
#include "AsoUtility.h"


//文字列の分割
std::vector<std::string> AsoUtility::Split(std::string& line, char delimiter)
{
	//動的配列の戻り値格納領域確保
	std::vector<std::string> result;

	//文字列操作用のクラスに置き換える
	std::istringstream lineStream(line);

	//1データの格納領域
	std::string field;

	//streamを順に読込し
	//getlineを使って
	// 第3引数で指定したデリミタ記号までのデータを
	// 第2引数の変数に格納する
	while (getline(lineStream, field, delimiter))
	{
		//動的配列に取得したデータを追加する
		result.push_back(field);
	}

	return result;
}


//四捨五入（float）
int AsoUtility::Round(float value)
{
	return static_cast<int>(roundf(value));
}

//オーバーロード関数
//引数と戻り値を変更している関数
Vector2 AsoUtility::Round(Vector2F value)
{
	Vector2 ret;

	ret.x = static_cast<int>(roundf(value.x));
	ret.y = static_cast<int>(roundf(value.y));

	return ret;
}
