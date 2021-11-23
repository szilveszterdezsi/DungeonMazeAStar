#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <cmath>
#include <vector>
#include "Tree.h"
#include "Cell.h"
#include "Tools.h"
//--------------------------------------------------------------------------------
#define For(i,N) for (int (i) = 0; (i) < (N); (i)++)
//--------------------------------------------------------------------------------
class Game {
public:
    static const int GRID_SIZE_X   = 101,   GRID_SIZE_Y  = 101;
    static const int SPACING       = 5,     POINT_SIZE   = 4;
    static const int WINDOW_MARGIN = 200,   LINE_WIDTH   = 1;
    static const int MIN_P_WIDTH   = 5,     MIN_P_HEIGHT = 5;
    const double     P_WH_RATIO    = 0.45f, P_HW_RATIO   = 0.45f;
    static const int ROOM_MARGIN   = 0;
    Game(void);
    void  ChangeSize(int w, int h);
    void  Draw();
    void  NormalKeys(unsigned char key, int state);
    void  SpecialKeys(int key, int state);
    void  Mouse(int button, int state, int x, int y);
    int   mW, mH, offsetX, offsetY;

private:
    void  Reset();
    void  InitMtxFont();
    void  DrawString(int x, int y, const char* fmt, ...);
    void  DrawChar(int x, int y, Uint8 c);
    void  DrawRectangle(int x, int y, int width, int height);
    void  DrawCell(int x, int y);
    void  DrawLine(int x1, int y1, int x2, int y2, int lineWidth = LINE_WIDTH);
    bool  GridHasValidStartCell();
    int   DistanceHeuristic(int x1, int y1, int x2, int y2, int diagonalCost);
    int   mMouseX, mMouseY;
    int   mMouseButton, mMouseState;
    char  (*mMtxFont)[7][5];

    bool  start;
    int   pI, rI, mI, acI, ucI = 1, ccI, cgI, deI, ascI, maxG, aspI;
    std::vector<Partition*> gpV, lpV, rpV;
    std::vector<std::vector<Cell*>> cG, rcV, ucV, asoV;
    std::vector<Cell*> rV, mV, acV, ccV, deV, ascV, aspV;
    std::vector<unsigned char*> colors;
};
//--------------------------------------------------------------------------------