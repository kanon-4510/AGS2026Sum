#include <DxLib.h>
#include <sstream>
#include "../Common/Vector2.h"
#include "../Common/Vector2F.h"
#include "../Manager/InputManager.h"
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

void Utility::DrawCommandMenu(int x, int y, const std::vector<std::string>& labels, int cursor)
{
	for (int i = 0; i < (int)labels.size(); ++i) {
		int color = GetColor(255, 255, 255);
		int selectColor = GetColor(255, 255, 0); //選択中は黄色にする
		DrawFormatString(x, y + i * 40, (cursor == i ? selectColor : color), labels[i].c_str());
	}
}

void Utility::ProcessCommandMenuSelection(int& cursor, int maxItems)
{
	if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_UP) ||
		InputManager::GetInstance().IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DG_UP))
	{
		cursor = (cursor - 1 + maxItems) % maxItems; //上にループ
	}
	if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_DOWN) ||
		InputManager::GetInstance().IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DG_DOWN))
	{
		cursor = (cursor + 1) % maxItems; //下にループ
	}
}
