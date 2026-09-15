#include <DxLib.h>
#include "Stage.h"

Stage::Stage()
{
}

Stage::~Stage()
{
}

void Stage::Init(void)
{

	stageBoard_[STAGE_TYPE::PLAINS] = LoadGraph("Data/Image/Board/Board_1.png");
	stageBoard_[STAGE_TYPE::FOREST] = LoadGraph("Data/Image/Board/Board_2.png");
	stageBoard_[STAGE_TYPE::SHRINE] = LoadGraph("Data/Image/Board/Board_3.png");
	stageBoard_[STAGE_TYPE::CONTINENT] = LoadGraph("Data/Image/Board/Board_4.png");
	stageBoard_[STAGE_TYPE::CATHEDRAL] = LoadGraph("Data/Image/Board/Board_5.png");
	stageBoard_[STAGE_TYPE::RUINS] = LoadGraph("Data/Image/Board/Board_6.png");
	stageBoard_[STAGE_TYPE::HILL] = LoadGraph("Data/Image/Board/Board_7.png");
	stageBoard_[STAGE_TYPE::EXTRA] = LoadGraph("Data/Image/Board/Board_8.png");

	//ステージの背景画像をロードしてマップに格納
	bgImageHandle_[STAGE_TYPE::PLAINS] = LoadGraph("Data/Image/Stage/Stage_1.png");
	bgImageHandle_[STAGE_TYPE::FOREST] = LoadGraph("Data/Image/Stage/Stage_2.png");
	bgImageHandle_[STAGE_TYPE::SHRINE] = LoadGraph("Data/Image/Stage/Stage_3.png");
	bgImageHandle_[STAGE_TYPE::CONTINENT] = LoadGraph("Data/Image/Stage/Stage_4.png");
	bgImageHandle_[STAGE_TYPE::CATHEDRAL] = LoadGraph("Data/Image/Stage/Stage_5.png");
	bgImageHandle_[STAGE_TYPE::RUINS] = LoadGraph("Data/Image/Stage/Stage_6.png");
	bgImageHandle_[STAGE_TYPE::HILL] = LoadGraph("Data/Image/Stage/Stage_7.png");
	bgImageHandle_[STAGE_TYPE::EXTRA] = LoadGraph("Data/Image/Stage/Stage_8.png");
}

void Stage::Update(void)
{

}

void Stage::DrawStageBoard(void)
{
	

	//現在選択されているステータスの画像を描画
	int img = stageBoard_[stageType_];
	DrawRotaGraph(DRAW_STAGE_BOARD_POS_X, DRAW_STAGE_BOARD_POS_Y, 1, 0, img, true);
}

void Stage::DrawBackground(void)
{
	//現在選択されているステータスの画像を描画
	int img = bgImageHandle_[stageType_];
	DrawGraph(DRAW_BACKGROUND_POS_X, DRAW_BACKGROUND_POS_Y, img, true);
}

void Stage::Release(void)
{

}

void Stage::SetStageType(STAGE_TYPE type)
{
	stageType_ = type;
}