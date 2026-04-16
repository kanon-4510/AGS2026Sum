#include <DxLib.h>
#include <stdarg.h>
#include "FontManager.h"

int FontManager::akazuki_ = -1;
int FontManager::currentSize_ = -1;

void FontManager::Init()
{
    //Windowsにフォントがインストール済みならこっちを使う
    //akazuki_ = CreateFontToHandle("07あかずきんポップ Heavy", FONT_SIZE_AKAZUKI, 1);

    //フォントファイルをWindowsの一時的なリソースとして登録する
    const char* fontPath = "Data/Font/AkazukiPOP.otf";

    //FR_PRIVATEを指定すると、このアプリが動いている間だけ有効になり、
    //アプリ終了時に自動的に登録が解除されます。
    AddFontResourceEx(fontPath, FR_PRIVATE, NULL);
    //登録に成功したら、フォント名でハンドルを作成する
    //インストール済みの場合と同じく、ここはファイル名ではなくフォント名
    // akazuki_ = CreateFontToHandle("07あかずきんポップ Heavy", FONT_SIZE_AKAZUKI, 3, DX_FONTTYPE_ANTIALIASING_4X4);
}

/// <summary>
/// 登録したフォントを使用する
/// </summary>
/// <param name="x">X座標</param>
/// <param name="y">Y座標</param>
/// <param name="text">出力する内容</param>
/// <param name="color">色</param>
/// <param name="size">フォントサイズ</param>
void FontManager::DrawString(int x, int y, const char* text, int color, int size)
{
    //InitでCreateFontToHandleを行う場合、int sizeは消してください
    //※その代わりサイズの設定が固定になってしまう
   /* if (akazuki_ != -1) {
        // サイズごとにフォントを作る（ハンドル管理簡易版）
        DrawStringToHandle(x, y, text, color, akazuki_);
    }*/

    //サイズが変わった時だけ、古いハンドルを捨てて作り直す
    if (size != currentSize_) {
        if (akazuki_ != -1) DeleteFontToHandle(akazuki_);

        akazuki_ = CreateFontToHandle("07あかずきんポップ Heavy", size, 3, DX_FONTTYPE_ANTIALIASING_4X4);
        currentSize_ = size;
    }

    //描画
    DrawStringToHandle(x, y, text, color, akazuki_);
}

void FontManager::Release(void)
{
    if (akazuki_ != -1) {
        DeleteFontToHandle(akazuki_);
        akazuki_ = -1;

        // 3. 使い終わったら登録を解除する（お作法として重要）
        RemoveFontResourceEx("Data/Font/AkazukiPOP.otf", FR_PRIVATE, NULL);
    }
}
