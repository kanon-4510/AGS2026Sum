#include "MagicDataBase.h"

MagicDataBase::MagicDataBase()
{
	magicData_ = {
	{1, "ファイア", MAGIC_TYPE::ATTACK, 1.5f, STATUS_EFFECT::NONE, 0, 0, false, false, "火を放ち敵を攻撃する魔法。\n威力は小さい。" },
	{2, "フレイム", MAGIC_TYPE::ATTACK, 2.0f, STATUS_EFFECT::NONE, 0, 80, false, false, "炎で敵を攻撃する魔法。\n威力は中くらい。" },
	{3,"プロミネンス",	   MAGIC_TYPE::ATTACK, 4.0f,STATUS_EFFECT::NONE,    0,330, false ,false, "灼熱で敵を焼き尽くす魔法。\n威力は大きい。"},
	{4,"ビッグバン",	   MAGIC_TYPE::ATTACK,10.0f,STATUS_EFFECT::NONE,    0,500, false, false, "目の前の敵を影ごと消し去る魔法。\n威力は絶大。"},
	//【回復魔法】高中低、状態異常回復＋回復
	{5, "ドレイン",		   MAGIC_TYPE::HEAL,   0.7f,STATUS_EFFECT::NONE,    0,190,true,false,"敵の生命力を奪い、\n自身の体力を回復する魔法。\n効果はまあまあ。"},
	{6, "ヒール",		   MAGIC_TYPE::HEAL,   0.3f,STATUS_EFFECT::NONE,    0,  0, false, false, "自分の体力を回復する魔法。\n効果はちょこっと。"},
	{7, "ハイヒール",	   MAGIC_TYPE::HEAL,   0.5f,STATUS_EFFECT::NONE,    0, 80, false, false, "自分の体力を回復する魔法。\n効果はなかなか。"},
	{8, "メガヒール",	   MAGIC_TYPE::HEAL,   1.0f,STATUS_EFFECT::NONE,    0,350, false, false, "自分の体力を回復する魔法。\n効果はけっこう。"},
	{9, "キュア",		   MAGIC_TYPE::HEAL,   0.0f,STATUS_EFFECT::NONE,    0, 50,false,true, "自分の状態異常を治療する魔法。\n回復効果はない。"},
	{10,"レスキュー",	   MAGIC_TYPE::HEAL,   0.5f,STATUS_EFFECT::NONE,    0,240,false,true, "自分の体力を回復し、\nさらに状態も治す魔法。\n効果はぼちぼち。"},
	//【状態異常攻撃】高中低（確率は低30%, 中50%, 高70%,100%）
	{11,"アシッドニードル",MAGIC_TYPE::DEBUFF, 1.0f,STATUS_EFFECT::POISON, 30, 30, false, false, "毒の針で突き刺す魔法。\n確率で毒状態にする。\n威力と確率は低い。"},
	{12,"ヴェノムスピア",  MAGIC_TYPE::DEBUFF, 2.5f,STATUS_EFFECT::POISON, 50,100, false, false,"毒の槍で貫く魔法。\n確率で毒状態にする。\n威力と確率は普通。"},
	{13,"トキシックランス",MAGIC_TYPE::DEBUFF, 4.0f,STATUS_EFFECT::POISON, 70,290, false, false,"毒の巨戟で穿つ魔法。\n確率で毒状態にする。\n威力と確率は高い。"},
	{14,"ポイズン",		   MAGIC_TYPE::DEBUFF, 0.0f,STATUS_EFFECT::POISON,100,160, false, false,"毒液を浴びせる魔法。\n必ず毒状態にする。\n威力はない。"},
	{15,"コールドブレス",  MAGIC_TYPE::DEBUFF, 1.0f,STATUS_EFFECT::FREEZE, 30, 30, false, false,"冷たい風を吹かせる魔法。\n確率で凍結状態にする。\n威力と確率は低い。"},
	{16,"スノーテンペスト",MAGIC_TYPE::DEBUFF, 2.5f,STATUS_EFFECT::FREEZE, 50,100, false, false,"凍える嵐を起こす魔法。\n確率で凍結状態にする。\n威力と確率は普通。"},
	{17,"アイシクルノヴァ",MAGIC_TYPE::DEBUFF, 4.0f,STATUS_EFFECT::FREEZE, 70,290, false, false,"氷の爆風を作り出す魔法。\n確率で凍結状態にする。\n威力と確率は高い。"},
	{18,"フリーズ",		   MAGIC_TYPE::DEBUFF, 0.0f,STATUS_EFFECT::FREEZE,100,160, false, false,"凍らせる魔法。\n必ず凍結状態にする。\n威力はない。"},
	{19,"フォトンシュート",MAGIC_TYPE::DEBUFF, 1.0f,STATUS_EFFECT::FLASH,  30, 30, false, false,"光る球をぶつける魔法。\n確率で閃光状態にする。\n威力と確率は低い。"},
	{20,"プリズムレーザー",MAGIC_TYPE::DEBUFF, 2.5f,STATUS_EFFECT::FLASH,  50,100, false, false,"輝く光線で攻撃する魔法。\n確率で閃光状態にする。\n威力と確率は普通。"},
	{21,"ミラージュレイン",MAGIC_TYPE::DEBUFF, 4.0f,STATUS_EFFECT::FLASH,  70,290, false, false,"煌めく雨を降らせる魔法。\n確率で閃光状態にする。\n威力と確率は高い。"},
	{22,"フラッシュ",	   MAGIC_TYPE::DEBUFF, 0.0f,STATUS_EFFECT::FLASH, 100,160, false, false,"閃光を発生させる魔法。\n必ず閃光状態にする。\n威力はない。"},
	{23,"リーサルクロー",  MAGIC_TYPE::DEBUFF, 3.0f,STATUS_EFFECT::CURSE,   5,130, false, false,"闇の爪で攻撃する魔法。\n超低確率で呪い状態にする。\n威力は普通。"},
	{24,"フェイタルソード",MAGIC_TYPE::DEBUFF, 4.5f,STATUS_EFFECT::CURSE,  10,390, false, false,"命を刈り取る剣を呼び出す魔法。\n低確率で呪い状態にする。\n威力は高い。"},
	};
}