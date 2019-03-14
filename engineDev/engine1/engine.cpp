#include "engine.h"

extern "C" {
    #include "drmImpl.h" // a C header, wrap it in extern "C"
}


Engine::Engine(){};

void Engine::Construct(uint32_t screen_w, uint32_t screen_h)
{
    nScreenWidth = screen_w;
    nScreenHeight = screen_h;
    pDrawTarget = new Sprite(nScreenWidth,nScreenHeight);
}

void Engine::Start()
{
    //start the thread
    active = true;
    std::thread t = std::thread(&Engine::EngineThread, this);

    //wait for thread to close
    t.join();
}

bool Engine::Draw(int32_t x, int32_t y, Pixel p)
{
    return pDrawTarget->SetPixel(x,y,p);
}

void Engine::clear()
{
   //pDrawTarget = new Sprite(nScreenWidth,nScreenHeight);

    for(int i = 0; i < pDrawTarget->width*pDrawTarget->height; i++)
        pDrawTarget->SetPixel(i % pDrawTarget->width,
                             i / pDrawTarget->width, BLACK);
}

void Engine::DrawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, Pixel p)
{
    int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
    dx = x2 - x1; dy = y2 - y1;

    // straight lines idea by gurkanctn
    if (dx == 0) // Line is vertical
    {
        if (y2 < y1) std::swap(y1, y2);
        for (y = y1; y <= y2; y++)
            Draw(x1, y, p);
        return;
    }

    if (dy == 0) // Line is horizontal
    {
        if (x2 < x1) std::swap(x1, x2);
        for (x = x1; x <= x2; x++)
            Draw(x, y1, p);
        return;
    }

    // Line is Funk-aye
    dx1 = abs(dx); dy1 = abs(dy);
    px = 2 * dy1 - dx1;	py = 2 * dx1 - dy1;
    if (dy1 <= dx1)
    {
        if (dx >= 0)
        {
            x = x1; y = y1; xe = x2;
        }
        else
        {
            x = x2; y = y2; xe = x1;
        }

        Draw(x, y, p);

        for (i = 0; x<xe; i++)
        {
            x = x + 1;
            if (px<0)
                px = px + 2 * dy1;
            else
            {
                if ((dx<0 && dy<0) || (dx>0 && dy>0)) y = y + 1; else y = y - 1;
                px = px + 2 * (dy1 - dx1);
            }
            Draw(x, y, p);
        }
    }
    else
    {
        if (dy >= 0)
        {
            x = x1; y = y1; ye = y2;
        }
        else
        {
            x = x2; y = y2; ye = y1;
        }

        Draw(x, y, p);

        for (i = 0; y<ye; i++)
        {
            y = y + 1;
            if (py <= 0)
                py = py + 2 * dx1;
            else
            {
                if ((dx<0 && dy<0) || (dx>0 && dy>0)) x = x + 1; else x = x - 1;
                py = py + 2 * (dx1 - dy1);
            }
            Draw(x, y, p);
        }
    }
}

void Engine::DrawCircle(int32_t x, int32_t y, int32_t radius, Pixel p)
    {
        int x0 = 0;
        int y0 = radius;
        int d = 3 - 2 * radius;
        if (!radius) return;

        while (y0 >= x0) // only formulate 1/8 of circle
        {
            Draw(x - x0, y - y0, p);//upper left left
            Draw(x - y0, y - x0, p);//upper upper left
            Draw(x + y0, y - x0, p);//upper upper right
            Draw(x + x0, y - y0, p);//upper right right
            Draw(x - x0, y + y0, p);//lower left left
            Draw(x - y0, y + x0, p);//lower lower left
            Draw(x + y0, y + x0, p);//lower lower right
            Draw(x + x0, y + y0, p);//lower right right
            if (d < 0) d += 4 * x0++ + 6;
            else d += 4 * (x0++ - y0--) + 10;
        }
}

void Engine::DrawTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, Pixel p)
    {
        DrawLine(x1, y1, x2, y2, p);
        DrawLine(x2, y2, x3, y3, p);
        DrawLine(x3, y3, x1, y1, p);
}

void Engine::FillTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, Pixel p) {

    auto SWAP = [](int &x, int &y) { int t = x; x = y; y = t; };
    auto drawline = [&](int sx, int ex, int ny) { for (int i = sx; i <= ex; i++) Draw(i, ny, p); };

    int t1x, t2x, y, minx, maxx, t1xp, t2xp;
    bool changed1 = false;
    bool changed2 = false;
    int signx1, signx2, dx1, dy1, dx2, dy2;
    int e1, e2;
    // Sort vertices
    if (y1>y2) { SWAP(y1, y2); SWAP(x1, x2); }
    if (y1>y3) { SWAP(y1, y3); SWAP(x1, x3); }
    if (y2>y3) { SWAP(y2, y3); SWAP(x2, x3); }

    t1x = t2x = x1; y = y1;   // Starting points
    dx1 = (int)(x2 - x1); if (dx1<0) { dx1 = -dx1; signx1 = -1; }
    else signx1 = 1;
    dy1 = (int)(y2 - y1);

    dx2 = (int)(x3 - x1); if (dx2<0) { dx2 = -dx2; signx2 = -1; }
    else signx2 = 1;
    dy2 = (int)(y3 - y1);

    if (dy1 > dx1) {   // swap values
        SWAP(dx1, dy1);
        changed1 = true;
    }
    if (dy2 > dx2) {   // swap values
        SWAP(dy2, dx2);
        changed2 = true;
    }

    e2 = (int)(dx2 >> 1);
    // Flat top, just process the second half
    if (y1 == y2) goto next;
    e1 = (int)(dx1 >> 1);

    for (int i = 0; i < dx1;) {
        t1xp = 0; t2xp = 0;
        if (t1x<t2x) { minx = t1x; maxx = t2x; }
        else { minx = t2x; maxx = t1x; }
        // process first line until y value is about to change
        while (i<dx1) {
            i++;
            e1 += dy1;
            while (e1 >= dx1) {
                e1 -= dx1;
                if (changed1) t1xp = signx1;//t1x += signx1;
                else          goto next1;
            }
            if (changed1) break;
            else t1x += signx1;
        }
        // Move line
    next1:
        // process second line until y value is about to change
        while (1) {
            e2 += dy2;
            while (e2 >= dx2) {
                e2 -= dx2;
                if (changed2) t2xp = signx2;//t2x += signx2;
                else          goto next2;
            }
            if (changed2)     break;
            else              t2x += signx2;
        }
    next2:
        if (minx>t1x) minx = t1x; if (minx>t2x) minx = t2x;
        if (maxx<t1x) maxx = t1x; if (maxx<t2x) maxx = t2x;
        drawline(minx, maxx, y);    // Draw line from min to max points found on the y
                                    // Now increase y
        if (!changed1) t1x += signx1;
        t1x += t1xp;
        if (!changed2) t2x += signx2;
        t2x += t2xp;
        y += 1;
        if (y == y2) break;

    }

next:
        // Second half
        dx1 = (int)(x3 - x2); if (dx1<0) { dx1 = -dx1; signx1 = -1; }
        else signx1 = 1;
        dy1 = (int)(y3 - y2);
        t1x = x2;

        if (dy1 > dx1) {   // swap values
            SWAP(dy1, dx1);
            changed1 = true;
        }
        else changed1 = false;

        e1 = (int)(dx1 >> 1);

        for (int i = 0; i <= dx1; i++) {
            t1xp = 0; t2xp = 0;
            if (t1x<t2x) { minx = t1x; maxx = t2x; }
            else { minx = t2x; maxx = t1x; }
            // process first line until y value is about to change
            while (i<dx1) {
                e1 += dy1;
                while (e1 >= dx1) {
                    e1 -= dx1;
                    if (changed1) { t1xp = signx1; break; }//t1x += signx1;
                    else          goto next3;
                }
                if (changed1) break;
                else   	   	  t1x += signx1;
                if (i<dx1) i++;
            }
        next3:
            // process second line until y value is about to change
            while (t2x != x3) {
                e2 += dy2;
                while (e2 >= dx2) {
                    e2 -= dx2;
                    if (changed2) t2xp = signx2;
                    else          goto next4;
                }
                if (changed2)     break;
                else              t2x += signx2;
            }
        next4:

            if (minx>t1x) minx = t1x; if (minx>t2x) minx = t2x;
            if (maxx<t1x) maxx = t1x; if (maxx<t2x) maxx = t2x;
            drawline(minx, maxx, y);
            if (!changed1) t1x += signx1;
            t1x += t1xp;
            if (!changed2) t2x += signx2;
            t2x += t2xp;
            y += 1;
            if (y>y3) return;
    }
}

void Engine::DrawSprite(int32_t x, int32_t y, Sprite *sprite, uint32_t scale)
    {
        if (sprite == nullptr)
            return;

        if (scale > 1)
        {
            for (int32_t i = 0; i < sprite->width; i++)
                for (int32_t j = 0; j < sprite->height; j++)
                    for (uint32_t is = 0; is < scale; is++)
                        for (uint32_t js = 0; js < scale; js++)
                            Draw(x + (i*scale) + is, y + (j*scale) + js, sprite->GetPixel(i, j));
        }
        else
        {
            for (int32_t i = 0; i < sprite->width; i++)
                for (int32_t j = 0; j < sprite->height; j++)
                    Draw(x + i, y + j, sprite->GetPixel(i, j));
        }
}

void Engine::EngineThread()
{

    ret = startGraphic(true);
    if(ret == 0) {
        std::cout << "Something went wrong" << std::endl;
        exit(0);
    }
    uint8_t r, g, b;
    bool r_up, g_up, b_up;
    srand(time(NULL));
    r_up = g_up = b_up = true;
    srand(time(NULL));
    r = rand() % 0xff;
    g = rand() % 0xff;
    b = rand() % 0xff;

    if (!OnUserCreate())
        active = false;

    auto tp1 = std::chrono::system_clock::now();
    auto tp2 = std::chrono::system_clock::now();

    while (active)
    {
        // Run as fast as possible
        while (active)
        {
            // Handle Timing
            tp2 = std::chrono::system_clock::now();
            std::chrono::duration<float> elapsedTime = tp2 - tp1;
            tp1 = tp2;

            // Our time per frame coefficient
            float fElapsedTime = elapsedTime.count();


            // Handle Frame Update
            if (!OnUserUpdate(fElapsedTime))
                active = false;

            // Display Graphics
            for(int i = 0; i < pDrawTarget->width*pDrawTarget->height; i++)
            {
                writeToBuf(i % pDrawTarget->width,
                           i / pDrawTarget->width,
                           pDrawTarget->GetPixel(i % pDrawTarget->width,i / pDrawTarget->width).r,
                           pDrawTarget->GetPixel(i % pDrawTarget->width,i / pDrawTarget->width).g,
                           pDrawTarget->GetPixel(i % pDrawTarget->width,i / pDrawTarget->width).b);
            }
            refresh(ret);
            // Present Graphics to screen



        }
    }
}

std::atomic<bool> Engine::active{ false };


bool Engine::OnUserUpdate(float fElapsedTime){ return false; }
bool Engine::OnUserCreate(){ return false; }

