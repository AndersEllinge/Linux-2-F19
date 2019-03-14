#ifndef ENGINE_H
#define ENGINE_H

#include "sprite.h"

#include <thread>
#include <atomic>
#include <chrono>
#include <vector>
#include <math.h>

class Engine
{
public:
       Engine();

public:
       void Construct(uint32_t screen_w, uint32_t screen_h);
       void Start();

public:
       void clear();
       bool Draw(int32_t x, int32_t y, Pixel p = WHITE);
       void DrawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, Pixel p = WHITE);
       void DrawCircle(int32_t x, int32_t y, int32_t radius, Pixel p = WHITE);
       void DrawTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, Pixel p);
       void FillTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, Pixel p);
       void DrawSprite(int32_t x, int32_t y, Sprite *sprite, uint32_t scale = 1);

       virtual bool OnUserCreate();
       virtual bool OnUserUpdate(float fElapsedTime);

       Sprite *pDrawTarget = nullptr;
       uint32_t nScreenWidth;
       uint32_t nScreenHeight;

private:
       int ret;
       static std::atomic<bool> active;

       void EngineThread();
};

#endif // ENGINE_H
