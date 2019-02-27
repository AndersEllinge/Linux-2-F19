#include "sprite.h"

Pixel::Pixel()
{
    r = 0; g = 0; b = 0; a = 255;
}

Pixel::Pixel(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
    r = red; g = green; b = blue; a = alpha;
}

Pixel::Pixel(uint32_t p)
{
    n = p;
}


Sprite::Sprite()
{
    pColData = nullptr;
    width = 0;
    height = 0;
}

Sprite::Sprite(int32_t w, int32_t h)
{
    if(pColData) delete[] pColData;
    width = w; height = h;
    pColData = new Pixel[width * height];
    for(int32_t i = 0; i < width*height; i++)
        pColData[i] = Pixel();
}

Sprite::Sprite(int num)
{
    if(num == 0)
    {
        if(pColData) delete[] pColData;
        width = 4; height = 7;
        pColData = new Pixel[width * height];
        pColData[0]  = Pixel(BLACK); pColData[1]  = Pixel(WHITE); pColData[2]  = Pixel(WHITE); pColData[3]  = Pixel(BLACK);
        pColData[4]  = Pixel(WHITE); pColData[5]  = Pixel(BLACK); pColData[6]  = Pixel(BLACK); pColData[7]  = Pixel(WHITE);
        pColData[8]  = Pixel(WHITE); pColData[9]  = Pixel(BLACK); pColData[10] = Pixel(BLACK); pColData[11] = Pixel(WHITE);
        pColData[12] = Pixel(WHITE); pColData[13] = Pixel(BLACK); pColData[14] = Pixel(BLACK); pColData[15] = Pixel(WHITE);
        pColData[16] = Pixel(WHITE); pColData[17] = Pixel(BLACK); pColData[18] = Pixel(BLACK); pColData[19] = Pixel(WHITE);
        pColData[20] = Pixel(WHITE); pColData[21] = Pixel(BLACK); pColData[22] = Pixel(BLACK); pColData[23] = Pixel(WHITE);
        pColData[24] = Pixel(BLACK); pColData[25] = Pixel(WHITE); pColData[26] = Pixel(WHITE); pColData[27] = Pixel(BLACK);
    }
    if(num == 1)
    {
        if(pColData) delete[] pColData;
        width = 4; height = 7;
        pColData = new Pixel[width * height];
        pColData[0]  = Pixel(BLACK); pColData[1]  = Pixel(BLACK); pColData[2]  = Pixel(WHITE); pColData[3]  = Pixel(BLACK);
        pColData[4]  = Pixel(BLACK); pColData[5]  = Pixel(WHITE); pColData[6]  = Pixel(WHITE); pColData[7]  = Pixel(BLACK);
        pColData[8]  = Pixel(WHITE); pColData[9]  = Pixel(BLACK); pColData[10] = Pixel(WHITE); pColData[11] = Pixel(BLACK);
        pColData[12] = Pixel(BLACK); pColData[13] = Pixel(BLACK); pColData[14] = Pixel(WHITE); pColData[15] = Pixel(BLACK);
        pColData[16] = Pixel(BLACK); pColData[17] = Pixel(BLACK); pColData[18] = Pixel(WHITE); pColData[19] = Pixel(BLACK);
        pColData[20] = Pixel(BLACK); pColData[21] = Pixel(BLACK); pColData[22] = Pixel(WHITE); pColData[23] = Pixel(BLACK);
        pColData[24] = Pixel(BLACK); pColData[25] = Pixel(BLACK); pColData[26] = Pixel(WHITE); pColData[27] = Pixel(BLACK);
    }
    if(num == 2)
    {
        if(pColData) delete[] pColData;
        width = 4; height = 7;
        pColData = new Pixel[width * height];
        pColData[0]  = Pixel(BLACK); pColData[1]  = Pixel(WHITE); pColData[2]  = Pixel(WHITE); pColData[3]  = Pixel(BLACK);
        pColData[4]  = Pixel(WHITE); pColData[5]  = Pixel(BLACK); pColData[6]  = Pixel(BLACK); pColData[7]  = Pixel(WHITE);
        pColData[8]  = Pixel(BLACK); pColData[9]  = Pixel(BLACK); pColData[10] = Pixel(BLACK); pColData[11] = Pixel(WHITE);
        pColData[12] = Pixel(BLACK); pColData[13] = Pixel(BLACK); pColData[14] = Pixel(WHITE); pColData[15] = Pixel(BLACK);
        pColData[16] = Pixel(BLACK); pColData[17] = Pixel(WHITE); pColData[18] = Pixel(BLACK); pColData[19] = Pixel(BLACK);
        pColData[20] = Pixel(WHITE); pColData[21] = Pixel(BLACK); pColData[22] = Pixel(BLACK); pColData[23] = Pixel(BLACK);
        pColData[24] = Pixel(WHITE); pColData[25] = Pixel(WHITE); pColData[26] = Pixel(WHITE); pColData[27] = Pixel(WHITE);
    }
    if(num == 3)
    {
        if(pColData) delete[] pColData;
        width = 4; height = 7;
        pColData = new Pixel[width * height];
        pColData[0]  = Pixel(BLACK); pColData[1]  = Pixel(WHITE); pColData[2]  = Pixel(WHITE); pColData[3]  = Pixel(BLACK);
        pColData[4]  = Pixel(WHITE); pColData[5]  = Pixel(BLACK); pColData[6]  = Pixel(BLACK); pColData[7]  = Pixel(WHITE);
        pColData[8]  = Pixel(BLACK); pColData[9]  = Pixel(BLACK); pColData[10] = Pixel(BLACK); pColData[11] = Pixel(WHITE);
        pColData[12] = Pixel(BLACK); pColData[13] = Pixel(WHITE); pColData[14] = Pixel(WHITE); pColData[15] = Pixel(BLACK);
        pColData[16] = Pixel(BLACK); pColData[17] = Pixel(BLACK); pColData[18] = Pixel(BLACK); pColData[19] = Pixel(WHITE);
        pColData[20] = Pixel(WHITE); pColData[21] = Pixel(BLACK); pColData[22] = Pixel(BLACK); pColData[23] = Pixel(WHITE);
        pColData[24] = Pixel(BLACK); pColData[25] = Pixel(WHITE); pColData[26] = Pixel(WHITE); pColData[27] = Pixel(BLACK);
    }
    if(num == 4)
    {
        if(pColData) delete[] pColData;
        width = 4; height = 7;
        pColData = new Pixel[width * height];
        pColData[0]  = Pixel(BLACK); pColData[1]  = Pixel(BLACK); pColData[2]  = Pixel(BLACK); pColData[3]  = Pixel(WHITE);
        pColData[4]  = Pixel(BLACK); pColData[5]  = Pixel(BLACK); pColData[6]  = Pixel(WHITE); pColData[7]  = Pixel(WHITE);
        pColData[8]  = Pixel(BLACK); pColData[9]  = Pixel(WHITE); pColData[10] = Pixel(BLACK); pColData[11] = Pixel(WHITE);
        pColData[12] = Pixel(WHITE); pColData[13] = Pixel(BLACK); pColData[14] = Pixel(BLACK); pColData[15] = Pixel(WHITE);
        pColData[16] = Pixel(WHITE); pColData[17] = Pixel(WHITE); pColData[18] = Pixel(WHITE); pColData[19] = Pixel(WHITE);
        pColData[20] = Pixel(BLACK); pColData[21] = Pixel(BLACK); pColData[22] = Pixel(BLACK); pColData[23] = Pixel(WHITE);
        pColData[24] = Pixel(BLACK); pColData[25] = Pixel(BLACK); pColData[26] = Pixel(BLACK); pColData[27] = Pixel(WHITE);
    }
    if(num == 5)
    {
        if(pColData) delete[] pColData;
        width = 4; height = 7;
        pColData = new Pixel[width * height];
        pColData[0]  = Pixel(WHITE); pColData[1]  = Pixel(WHITE); pColData[2]  = Pixel(WHITE); pColData[3]  = Pixel(WHITE);
        pColData[4]  = Pixel(WHITE); pColData[5]  = Pixel(BLACK); pColData[6]  = Pixel(BLACK); pColData[7]  = Pixel(BLACK);
        pColData[8]  = Pixel(WHITE); pColData[9]  = Pixel(WHITE); pColData[10] = Pixel(WHITE); pColData[11] = Pixel(BLACK);
        pColData[12] = Pixel(BLACK); pColData[13] = Pixel(BLACK); pColData[14] = Pixel(BLACK); pColData[15] = Pixel(WHITE);
        pColData[16] = Pixel(BLACK); pColData[17] = Pixel(BLACK); pColData[18] = Pixel(BLACK); pColData[19] = Pixel(WHITE);
        pColData[20] = Pixel(WHITE); pColData[21] = Pixel(BLACK); pColData[22] = Pixel(BLACK); pColData[23] = Pixel(WHITE);
        pColData[24] = Pixel(BLACK); pColData[25] = Pixel(WHITE); pColData[26] = Pixel(WHITE); pColData[27] = Pixel(BLACK);
    }
    if(num == 6)
    {
        if(pColData) delete[] pColData;
        width = 4; height = 7;
        pColData = new Pixel[width * height];
        pColData[0]  = Pixel(BLACK); pColData[1]  = Pixel(WHITE); pColData[2]  = Pixel(WHITE); pColData[3]  = Pixel(BLACK);
        pColData[4]  = Pixel(WHITE); pColData[5]  = Pixel(BLACK); pColData[6]  = Pixel(BLACK); pColData[7]  = Pixel(WHITE);
        pColData[8]  = Pixel(WHITE); pColData[9]  = Pixel(BLACK); pColData[10] = Pixel(BLACK); pColData[11] = Pixel(BLACK);
        pColData[12] = Pixel(WHITE); pColData[13] = Pixel(WHITE); pColData[14] = Pixel(WHITE); pColData[15] = Pixel(BLACK);
        pColData[16] = Pixel(WHITE); pColData[17] = Pixel(BLACK); pColData[18] = Pixel(BLACK); pColData[19] = Pixel(WHITE);
        pColData[20] = Pixel(WHITE); pColData[21] = Pixel(BLACK); pColData[22] = Pixel(BLACK); pColData[23] = Pixel(WHITE);
        pColData[24] = Pixel(BLACK); pColData[25] = Pixel(WHITE); pColData[26] = Pixel(WHITE); pColData[27] = Pixel(BLACK);
    }
    if(num == 7)
    {
        if(pColData) delete[] pColData;
        width = 4; height = 7;
        pColData = new Pixel[width * height];
        pColData[0]  = Pixel(WHITE); pColData[1]  = Pixel(WHITE); pColData[2]  = Pixel(WHITE); pColData[3]  = Pixel(WHITE);
        pColData[4]  = Pixel(BLACK); pColData[5]  = Pixel(BLACK); pColData[6]  = Pixel(BLACK); pColData[7]  = Pixel(WHITE);
        pColData[8]  = Pixel(BLACK); pColData[9]  = Pixel(BLACK); pColData[10] = Pixel(BLACK); pColData[11] = Pixel(WHITE);
        pColData[12] = Pixel(BLACK); pColData[13] = Pixel(BLACK); pColData[14] = Pixel(WHITE); pColData[15] = Pixel(BLACK);
        pColData[16] = Pixel(BLACK); pColData[17] = Pixel(WHITE); pColData[18] = Pixel(BLACK); pColData[19] = Pixel(BLACK);
        pColData[20] = Pixel(BLACK); pColData[21] = Pixel(WHITE); pColData[22] = Pixel(BLACK); pColData[23] = Pixel(BLACK);
        pColData[24] = Pixel(BLACK); pColData[25] = Pixel(WHITE); pColData[26] = Pixel(BLACK); pColData[27] = Pixel(BLACK);
    }
    if(num == 8)
    {
        if(pColData) delete[] pColData;
        width = 4; height = 7;
        pColData = new Pixel[width * height];
        pColData[0]  = Pixel(BLACK); pColData[1]  = Pixel(WHITE); pColData[2]  = Pixel(WHITE); pColData[3]  = Pixel(BLACK);
        pColData[4]  = Pixel(WHITE); pColData[5]  = Pixel(BLACK); pColData[6]  = Pixel(BLACK); pColData[7]  = Pixel(WHITE);
        pColData[8]  = Pixel(WHITE); pColData[9]  = Pixel(BLACK); pColData[10] = Pixel(BLACK); pColData[11] = Pixel(WHITE);
        pColData[12] = Pixel(BLACK); pColData[13] = Pixel(WHITE); pColData[14] = Pixel(WHITE); pColData[15] = Pixel(BLACK);
        pColData[16] = Pixel(WHITE); pColData[17] = Pixel(BLACK); pColData[18] = Pixel(BLACK); pColData[19] = Pixel(WHITE);
        pColData[20] = Pixel(WHITE); pColData[21] = Pixel(BLACK); pColData[22] = Pixel(BLACK); pColData[23] = Pixel(WHITE);
        pColData[24] = Pixel(BLACK); pColData[25] = Pixel(WHITE); pColData[26] = Pixel(WHITE); pColData[27] = Pixel(BLACK);
    }
    if(num == 9)
    {
        if(pColData) delete[] pColData;
        width = 4; height = 7;
        pColData = new Pixel[width * height];
        pColData[0]  = Pixel(BLACK); pColData[1]  = Pixel(WHITE); pColData[2]  = Pixel(WHITE); pColData[3]  = Pixel(BLACK);
        pColData[4]  = Pixel(WHITE); pColData[5]  = Pixel(BLACK); pColData[6]  = Pixel(BLACK); pColData[7]  = Pixel(WHITE);
        pColData[8]  = Pixel(WHITE); pColData[9]  = Pixel(BLACK); pColData[10] = Pixel(BLACK); pColData[11] = Pixel(WHITE);
        pColData[12] = Pixel(BLACK); pColData[13] = Pixel(WHITE); pColData[14] = Pixel(WHITE); pColData[15] = Pixel(WHITE);
        pColData[16] = Pixel(BLACK); pColData[17] = Pixel(BLACK); pColData[18] = Pixel(BLACK); pColData[19] = Pixel(WHITE);
        pColData[20] = Pixel(WHITE); pColData[21] = Pixel(BLACK); pColData[22] = Pixel(BLACK); pColData[23] = Pixel(WHITE);
        pColData[24] = Pixel(BLACK); pColData[25] = Pixel(WHITE); pColData[26] = Pixel(WHITE); pColData[27] = Pixel(BLACK);
    }
}


Sprite::~Sprite()
{
    if(pColData) delete pColData;
}

Pixel* Sprite::GetData() { return pColData; }

Pixel Sprite::GetPixel(int32_t x, int32_t y)
{
    if(x >= 0 && x < width && y >= 0 && y < height)
        return pColData[y*width + x];
    else
        return Pixel(0,0,0,0);
}

bool Sprite::SetPixel(int32_t x, int32_t y, Pixel p)
{
    if (x >= 0 && x < width && y >= 0 && y < height)
    {
        pColData[y*width + x] = p;
        return true;
    }
    else
        return false;
}

