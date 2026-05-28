#include "Item.h"
#include "../../Object/PlayerStatus.h"

Item::Item(ItemType type) : type_(type), effectValue_(0)
{
    switch (type_) {
    case ItemType::HealHP:
        name_ = "傷薬";
        description_ = "HPを5回復する。薬学によって効果が上がる。";
        effectValue_ = 5; //基本回復量
        break;

    case ItemType::CurePoison:
        name_ = "毒消し草";
        description_ = "体の毒をきれいに洗い流す草。";
        effectValue_ = 0;  //固定の回復量はなし
        break;
    }
}

int Item::Use(PlayerStatus* player)
{
    if (!player) return 0;

    switch (type_) {
    case ItemType::HealHP:
        //すでにHPが満タンなら使用しない
        if (player->hp_ >= player->maxHp_) return 0;

        //プレイヤーの「薬学」ボーナスを適用
        int finalHeal = player->SkillBonus(PlayerStatus::BonusType::ItemBonus, effectValue_);

        //実際に回復した量を計算するために回復前のHPを記録
        int oldHp = player->hp_;

        //HPを回復
        player->hp_ += finalHeal;

        //最大HPを超えないように制限
        if (player->hp_ > player->maxHp_) {
            player->hp_ = player->maxHp_;
        }

        //実際の回復量を返す（例：HPが 80/100 で 30 回復したら、20 が返る）
        return player->hp_ - oldHp;

    /*case ItemType::CurePoison:
        //プレイヤーに毒状態フラグ（isPoison_等）があると仮定した拡張例
        
        if (player->isPoison_) {
            player->isPoison_ = false; // 毒を解除
            return 1; // 成功の印として1を返す
        }
        
        return 0; //毒になっていなければ使用失敗
    */
    }

    return 0;
}
