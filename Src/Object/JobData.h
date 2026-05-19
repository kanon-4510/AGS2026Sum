#pragma once
#include <string>

class JobData
{
public:
    struct JobDatas {
        std::string name;   //職業名

        //基礎ステータスの条件
        int reqLevel_;   //レベル
        int reqPower_;   //攻撃力
        int reqMagic_;   //魔法攻撃力

        //技術ステータスの条件
        int reqPharmacy_;       //薬学
        int reqMartialArts_;    //武術
        int reqMagicKnowledge_; //魔法知識
        int reqFaith_;          //信仰
        int reqArchaeology_;    //考古学
        int reqAstrology_;      //占星術
    };

    JobDatas status;

    JobData(std::string n, int lv = 0, int pow = 0, int mag = 0,
        int pha = 0, int mar = 0, int mkn = 0,
        int fai = 0, int arc = 0, int ast = 0);
};
