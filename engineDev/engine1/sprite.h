#ifndef SPRITE_H
#define SPRITE_H

#include <iostream>
#include <unistd.h>

struct Pixel
    {
        union
        {
            uint32_t n = 0xFF000000;
            struct
            {
                uint8_t r;	uint8_t g;	uint8_t b;	uint8_t a;
            };
        };

        Pixel();
        Pixel(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255);
        Pixel(uint32_t p);
        enum Mode { NORMAL, MASK, ALPHA, CUSTOM };
    };

    // Some constants for symbolic naming of Pixels
    static const Pixel
        WHITE(255, 255, 255),
        GREY(192, 192, 192), DARK_GREY(128, 128, 128), VERY_DARK_GREY(64, 64, 64),
        RED(255, 0, 0), DARK_RED(128, 0, 0), VERY_DARK_RED(64, 0, 0),
        YELLOW(255, 255, 0), DARK_YELLOW(128, 128, 0), VERY_DARK_YELLOW(64, 64, 0),
        GREEN(0, 255, 0), DARK_GREEN(0, 128, 0), VERY_DARK_GREEN(0, 64, 0),
        CYAN(0, 255, 255), DARK_CYAN(0, 128, 128), VERY_DARK_CYAN(0, 64, 64),
        BLUE(0, 0, 255), DARK_BLUE(0, 0, 128), VERY_DARK_BLUE(0, 0, 64),
        MAGENTA(255, 0, 255), DARK_MAGENTA(128, 0, 128), VERY_DARK_MAGENTA(64, 0, 64),
        BLACK(0, 0, 0),
        BLANK(0, 0, 0, 0);



class Sprite
{
public:
        Sprite();
        Sprite(int32_t w, int32_t h);
        Sprite(int num);
        ~Sprite();

public:
        int32_t width = 0;
        int32_t height = 0;

public:
       Pixel GetPixel(int32_t x, int32_t y);
       bool SetPixel(int32_t x, int32_t y, Pixel p);
       Pixel* GetData();

private:
       Pixel *pColData = nullptr;

};

#endif // SPRITE_H
