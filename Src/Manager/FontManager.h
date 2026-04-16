#pragma once
class FontManager
{
public:
    //フォントサイズ
	static constexpr int FONT_SIZE_AKAZUKI = 60;
	static constexpr int FONT_SIZE_NORMAL = 22;

    static void Init();

    //反映させたフォントを使ってテキストを出力
    static void DrawString(int x, int y, const char* text, int color, int size);

    static void Release(void);

    static int GetAkazuki() { return akazuki_; }

private:
    
    static int akazuki_;
    static int currentSize_;
};

