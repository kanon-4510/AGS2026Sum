#pragma once

#include <string>
#include <vector>

class Vector2;
class Vector2F;

class AsoUtility
{
public:
	//列挙型
	//方向
	enum class DIR
	{
		UP			//0
		, RIGHT		//1
		, DOWN		//2
		, LEFT		//3
		, MAX		//4
	};

	//文字列の分割
	static std::vector<std::string> Split(std::string& line, char delimiter);

	//四捨五入（float）
	static int Round(float value);
	//オーバーロード関数
	static Vector2 Round(Vector2F value);

};