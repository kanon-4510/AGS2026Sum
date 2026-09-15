#pragma once
#include <string>
#include <vector>

//魔法の種類
enum class MAGIC_TYPE {
	ATTACK,
	HEAL,
	BUFF,
	DEBUFF
};

//状態異常の種類
enum class STATUS_EFFECT {
	NONE,		//状態異常なし
	POISON,		//毒
	FLASH,		//閃光
	FREEZE,		//凍結
	CURSE,		//呪い
	SILENCE		//沈黙
};

struct MagicData {
	int id = 0;	//識別ID
	std::string name;	//魔法名
	MAGIC_TYPE type = MAGIC_TYPE::ATTACK;	//魔法の種類
	float powerMultiplier = 1.0f;	//魔力に乗算する倍率（低:0.7, 中:1.3, 高:2.2 など）
	STATUS_EFFECT ailment = STATUS_EFFECT::NONE;	//付与する状態異常（なければ NONE）
	int ailmentChance = 0;	//状態異常の確率（0〜100%）
	int reqMagicKnowledge = 0;	//習得に必要な「魔法知識」の数値
	bool isDrain = false;	//HP吸収魔法かどうかのフラグ
	bool curesStatus = false;	//状態異常も一緒に治すかどうかのフラグ
	std::string description = "";	//魔法の説明文
};

class MagicDataBase
{
public:

	MagicDataBase();	//コンストラクタ
	
	const std::vector<MagicData>& GetAll() const { return magicData_; }	//全魔法データを返す

private:
	std::vector<MagicData> magicData_;	//魔法データベース
};

