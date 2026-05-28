#pragma once
#include <string>
#include "..//PlayerStatus.h"

class Item
{
public:
    //アイテムの種類
    enum class ItemType {
        HealHP,       //HP回復アイテム
        CurePoison,   //状態異常回復
    };

    Item(ItemType type);
    ~Item() {}

    //アイテムを使用する関数
    int Use(PlayerStatus* player);

    //ゲッター
    ItemType GetType() const { return type_; }
    std::string GetName() const { return name_; }
    std::string GetDescription() const { return description_; }

private:
    ItemType type_;             //アイテムの種類
    std::string name_;          //アイテム名
    std::string description_;   //説明文
    int effectValue_;           //基本の効果量
};

