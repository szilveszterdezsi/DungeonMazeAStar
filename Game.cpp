#include "Game.h"
//--------------------------------------------------------------------------------
Game::Game(void)
{
    InitMtxFont();
    mW = GRID_SIZE_X * SPACING + WINDOW_MARGIN;
    mH = GRID_SIZE_Y * SPACING + WINDOW_MARGIN;
    mMouseX = mMouseY = 0;
    mMouseButton = mMouseState = 0;

    start = false;

    Partition p = Partition(GRID_SIZE_X, GRID_SIZE_Y);
    Tree* tree = new Tree(&p, MIN_P_WIDTH + (ROOM_MARGIN * 2),
        MIN_P_HEIGHT + (ROOM_MARGIN * 2), P_WH_RATIO, P_HW_RATIO);
    gpV = tree->GetAllPartitions();
    lpV = tree->GetLeafPartitions();
    int rW, rH, rX, rY, mX, mY;
    for (Partition* r : lpV)
    {
        rW = RandomInt(MIN_P_WIDTH, r->w - (ROOM_MARGIN * 2));
        rH = RandomInt(MIN_P_HEIGHT, r->h - (ROOM_MARGIN * 2));
        rX = r->x + RandomInt(ROOM_MARGIN, r->w - rW - ROOM_MARGIN);
        rY = r->y + RandomInt(ROOM_MARGIN, r->h - rH - ROOM_MARGIN);
        if (rX % 2 == 0) rX++, rW--;
        if (rY % 2 == 0) rY++, rH--;
        if (rW % 2 == 0) rW--;
        if (rH % 2 == 0) rH--;
        rpV.push_back(new Partition(rX, rY, rW, rH));
    }

    For(x, GRID_SIZE_X)
    {
        std::vector<Cell*> y(GRID_SIZE_Y);
        cG.push_back(y);
    }
    For(x, GRID_SIZE_X) For(y, GRID_SIZE_Y) cG[x][y] = new Cell(x, y);
    For(x, GRID_SIZE_X) For(y, GRID_SIZE_Y)
    {
        if (y + 1 < GRID_SIZE_Y)
            cG[x][y]->s = cG[x][y + 1];
        if (y - 1 >= 0)
            cG[x][y]->n = cG[x][y - 1];
        if (x - 1 >= 0)
        {
            cG[x][y]->w = cG[x - 1][y];
            if (y - 1 >= 0)
                cG[x][y]->nw = cG[x - 1][y - 1];
            if (y + 1 < GRID_SIZE_Y)
                cG[x][y]->sw = cG[x - 1][y + 1];
        }
        if (x + 1 < GRID_SIZE_X)
        {
            cG[x][y]->e = cG[x + 1][y];
            if (y - 1 >= 0)
                cG[x][y]->ne = cG[x + 1][y - 1];
            if (y + 1 < GRID_SIZE_Y)
                cG[x][y]->se = cG[x + 1][y + 1];
        }  
    }

    For(i, rpV.size()) For(x, rpV[i]->w) For(y, rpV[i]->h)
    {
        Cell* c = cG[rpV[i]->x + x][rpV[i]->y + y];
        c->isPassable = true;
        c->id = i;
        rV.push_back(c);
    }
    
    std::vector<Cell*> stack;
    int id = 0;
    while (GridHasValidStartCell())
    {
        int x = (RandomInt(1, (GRID_SIZE_X - 1) / 2) * 2) - 1;
        int y = (RandomInt(1, (GRID_SIZE_Y - 1) / 2) * 2) - 1;
        Cell* s = cG[x][y];
        if (!s->isPassable)
        {
            stack.push_back(s);
            mV.push_back(s);
            while (!stack.empty())
            {
                Cell* c = stack.back();
                std::vector<Cell*> nV = c->NextOverVonNeumannNeighbours(false);
                if (nV.size() > 0)
                {
                    Cell* r = nV[RandomInt(nV.size() - 1)];
                    int x = r->x - ((r->x - c->x) / 2);
                    int y = r->y - ((r->y - c->y) / 2);
                    Cell* e = cG[x][y];
                    c->isPassable = r->isPassable = e->isPassable = true;
                    c->id = r->id = e->id = rpV.size() + id;
                    stack.push_back(r);
                    mV.push_back(e);
                    mV.push_back(r);
                }
                else
                {
                    stack.pop_back();
                }
            }
            id++;
        }
    }

    for (Partition* p : rpV)
    {
        std::vector<Cell*> rc;
        For (x, p->w + 2) For (y, p->h + 2)
        {
            if ((x < 1 || x > p->w) != (y < 1 || y > p->h))
            {
                Cell* c = cG[(p->x + x) - 1][(p->y + y) - 1];
                if (c->VonNeumannNeighbourhood(true).size() >= 2 
                    && !VectorContains(acV, c))
                {
                    acV.push_back(c);
                    rc.push_back(c);
                }
            }
        }
        rcV.push_back(rc);
    }

    for (std::vector<Cell*> rc : rcV)
    {
        while (!rc.empty())
        {
            std::vector<Cell*> uc;
            Cell* c = rc.back();
            uc.push_back(c);
            rc.pop_back();
            for (int i = rc.size() - 1; i >= 0; i--)
            {
                int idA1 = c->VonNeumannNeighbourhood(true)[0]->id;
                int idA2 = c->VonNeumannNeighbourhood(true)[1]->id;
                int idB1 = rc[i]->VonNeumannNeighbourhood(true)[0]->id;
                int idB2 = rc[i]->VonNeumannNeighbourhood(true)[1]->id;
                if ((idA1 == idB1 || idA1 == idB2) &&
                    (idA2 == idB1 || idA2 == idB2))
                {
                    uc.push_back(rc[i]);
                    rc.erase(rc.begin() + i);
                }
            }
            ucV.push_back(uc);
            Cell* d1 = uc[RandomInt(uc.size() - 1)];
            d1->isPassable = true;
            ccV.push_back(d1);
        }
    }

    For(i, ucV.size())
    {
        colors.push_back(new unsigned char[3]
            { unsigned char(RandomInt(150, 225)),
              unsigned char(RandomInt(150, 225)),
              unsigned char(RandomInt(150, 225)) });
    }

    For(x, GRID_SIZE_X) For(y, GRID_SIZE_Y)
    {
        Cell* c = cG[x][y];
        std::vector<Cell*> cN = c->VonNeumannNeighbourhood(true);
        if (c->isPassable && cN.size() == 1)
        {
            while (cN.size() == 1)
            {
                c->isPassable = false;
                deV.push_back(c);
                c = cN.back();
                cN = c->VonNeumannNeighbourhood(true);
            }
        }
    }

    std::vector<Cell*> open;
    Cell* a = rV.front();
    Cell* b = rV.back();
    open.push_back(a);
    while (a != b && !open.empty())
    {
        std::vector<Cell*> aso;
        a = open.front();
        for (int i = 1; i < open.size(); i++)
            if (open[i]->f <= a->f)
                if (open[i]->h <= a->h)
                    a = open[i];
        open.erase(std::remove(open.begin(), open.end(), a), open.end());
        for (Cell* n : a->MooreNeighbourhood(true, false))
      //for (Cell* n : a->VonNeumannNeighbourhood(true))
        {
            if (!VectorContains(ascV, n))
            {
                int g = a->g + DistanceHeuristic(a->x, a->y, n->x, n->y, 14);
              //int g = a->g + DistanceHeuristic(a->x, a->y, n->x, n->y, 20);
                if (g < n->g || !VectorContains(open, n))
                {
                    n->g = g;
                    n->h = DistanceHeuristic(n->x, n->y, b->x, b->y, 20);
                    n->f = n->g + n->h;
                    n->p = a;
                    if (!VectorContains(open, n))
                    {
                        open.push_back(n);
                        aso.push_back(n);
                    } 
                }
            }
        }
        ascV.push_back(a);
        asoV.push_back(aso);
    }
    
    maxG = 0;
    For (i, ascV.size())
        if (ascV[i]->g >= maxG)
            maxG = ascV[i]->g;

    while (b != rV.front())
    {
        b = b->p;
        aspV.push_back(b);
    }

    printf("A* step count: %d\n", ascV.size());
    printf("A* path count: %d\n", aspV.size());
}
//--------------------------------------------------------------------------------
void Game::ChangeSize(int w, int h)
{
    mW = w, mH = h;
    offsetX = (mW - (GRID_SIZE_X * SPACING) + SPACING) / 2;
    offsetY = (mH - (GRID_SIZE_Y * SPACING) + SPACING) / 7;
    glViewport(0, 0, mW, mH);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, double(mW), double(mH), 0.0f, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
//--------------------------------------------------------------------------------
void Game::Draw(void)
{
    glClear(GL_COLOR_BUFFER_BIT);

    if (start)
    {
        if (cgI == 0)
        {
            glColor3ub(50, 75, 50);
            For(i, pI)
                DrawRectangle(gpV[i]->x, gpV[i]->y, gpV[i]->w, gpV[i]->h);
            glColor3ub(100, 200, 100);
            DrawRectangle(0, 0, GRID_SIZE_X, GRID_SIZE_Y);
            if (pI < gpV.size())
                pI++;
        }
        glColor3ub(100, 200, 100);
        DrawString(offsetX - 8, mH - (offsetY * 6) + 6,
            "GENERATING BINARY SPACE PARTITION TREE..............................%3.0lf%%",
            100. * pI / gpV.size());
    }

    if (pI >= gpV.size())
    {
        glColor3ub(50, 150, 50);
        DrawString(offsetX - 8, mH - (offsetY * 6) + 18,
   "GENERATING ROOMS FROM BSP TREE LEAF SPACES..........................%3.0lf%%",
   100. * rI / rV.size());
        For (i, rI)
            DrawCell(rV[i]->x, rV[i]->y);
        if (rI < rV.size())
            rI++;
    }
    
    if (rI >= rV.size())
    {
        glColor3ub(150, 200, 50);
        DrawString(offsetX - 8, mH - (offsetY * 6) + 30,
   "GENERATING MAZES FROM REMAINING SPACE USING PRIM'S ALGORITHM........%3.0lf%%",
   100. * mI / mV.size());
        For(i, mI)
            DrawCell(mV[i]->x, mV[i]->y);
        if (mI < mV.size())
            mI++;
    }

    if (mI >= mV.size())
    {
        glColor3ub(255, 255, 100);
        DrawString(offsetX - 8, mH - (offsetY * 6) + 42,
   "IDENTIFYING POSSIBLE ROOM->ROOM AND ROOM->MAZE CONNECTIONS..........%3.0lf%%",
   100. * acI / acV.size());
        For(i, acI)
            DrawCell(acV[i]->x, acV[i]->y);
        if (acI < acV.size())
            acI++;
    }

    if (acI >= acV.size())
    {
        For(i, ucI)
        {
            glColor3ubv(colors[i]);
            For(y, ucV[i].size())
                DrawCell(ucV[i][y]->x, ucV[i][y]->y);
        }
        DrawString(offsetX - 8, mH - (offsetY * 6) + 54,
   "SEPARATING CONNECTIONS INTO UNIQE SETS..............................%3.0lf%%",
   100. * ucI / ucV.size());
        if (ucI < ucV.size())
            ucI++;
    }

    if (ucI >= ucV.size())
    {
        glColor3ub(0, 0, 0);
        For(i, ccI) For(y, ucV[i].size())
            DrawCell(ucV[i][y]->x, ucV[i][y]->y);
        glColor3ub(100, 255, 100);
        DrawString(offsetX - 8, mH - (offsetY * 6) + 66,
   "STOCHASTICALLY SELECTING A CONNECTION FROM EACH SET.................%3.0lf%%",
   100. * ccI / ccV.size());
        For(i, ccI)
        {
            DrawCell(ccV[i]->x, ccV[i]->y);
        }
        if (ccI < ccV.size())
            ccI++;
    }

    if (ccI >= ccV.size())
    {
        glColor3ub(200, 100, 100);
        DrawString(offsetX - 8, mH - (offsetY * 6) + 78,
   "TRIMMING MAZE DEAD ENDS.............................................%3.0lf%%",
   100. * deI / deV.size());
        glColor3ub(0, 0, 0);
        For(i, deI)
        {
            DrawCell(deV[i]->x, deV[i]->y);
        }
        if (deI < deV.size())
            deI++;
    }

    if (deI >= deV.size())
    {
        glColor3ub(200, 200, 200);
        DrawString(offsetX - 8, mH - (offsetY * 6) + 90,
   "FINALIZING DUNGEON..................................................%3.0lf%%",
   100. * cgI / cG.size());
        For(x, cgI) For(y, cgI)
        {
            if (cG[x][y]->isPassable)
                glColor3ub(0, 0, 0);
            else
                glColor3ub(200, 200, 200);
            DrawCell(x, y);
        }
        if (cgI < cG.size())
            cgI++;
    }

    if (cgI >= cG.size())
    {
        For(i, ascI)
        {
            For(y, asoV[i].size())
            {
                glColor3ub(0, 0, 100);
                DrawCell(asoV[i][y]->x, asoV[i][y]->y);
            }
            int s = 100 * ascV[i]->g / maxG;
            glColor3ub(100 - s, s, 0);
            DrawCell(ascV[i]->x, ascV[i]->y);
            glColor3ub(200 - s, 100 + s, 0);
            if (ascV[i]->p != nullptr)
                DrawLine(ascV[i]->x, ascV[i]->y, ascV[i]->p->x, ascV[i]->p->y, 2);
        }
        DrawString(offsetX - 8, mH - (offsetY * 6) + 102,
   "RUNNING A* SEARCH ALGORITHM FROM      TO     .......................%3.0lf%%",
   100. * ascI / ascV.size());
        glColor3ub(100, 200, 0);
        DrawString(offsetX - 8, mH - (offsetY * 6) + 102,
   "                                 A->+");
        DrawCell(ascV[0]->x, ascV[0]->y);
        glColor3ub(200, 100, 0);
        DrawString(offsetX - 8, mH - (offsetY * 6) + 102,
   "                                         B->+");
        DrawCell(ascV[ascV.size() - 1]->x, ascV[ascV.size() - 1]->y);
        if (ascI < ascV.size())
            ascI++;
    }

    if (ascI >= ascV.size())
    {
        glColor3ub(100, 100, 200);
        For(i, aspI - 1)
        {
            DrawCell(aspV[i]->x, aspV[i]->y);
            if (aspV[i]->p != nullptr)
                DrawLine(aspV[i]->x, aspV[i]->y, aspV[i]->p->x, aspV[i]->p->y, 2);
        }
        if (aspI < aspV.size())
            aspI++;
        DrawString(offsetX - 8, mH - (offsetY * 6) + 114,
            "BACKTRACKING TO DETERMINE SHORTEST PATH.............................%3.0lf%%",
            100. * aspI / aspV.size());
    }

    if ((pI < gpV.size() || acI >= acV.size()) && deI == 0)
        SDL_Delay(50);
    else
        SDL_Delay(5);
}
//--------------------------------------------------------------------------------
void Game::NormalKeys(unsigned char key, int state)
{
    if (key >= SDLK_0 && key <= SDLK_9){}
    if (key == SDLK_RETURN) { start = true; }   //Return
    if (key == SDLK_SPACE) { Reset(); }         //Space
}
//--------------------------------------------------------------------------------
void Game::SpecialKeys(int key, int state)
{
    if (key == SDLK_LEFT) {}
    if (key == SDLK_RIGHT) {}
    if (key == SDLK_UP) {}
    if (key == SDLK_DOWN) {}
}
//--------------------------------------------------------------------------------
void Game::Mouse(int button, int state, int x, int y)
{
    mMouseButton = button;//SDL_BUTTON_LEFT/SDL_BUTTON_MIDDLE/SDL_BUTTON_RIGHT
    mMouseState = state;//SDL_PRESSED/SDL_RELEASED
    mMouseX = x; mMouseY = y;
}
//--------------------------------------------------------------------------------
void Game::Reset()
{
    pI = rI = mI = acI = ucI = ccI = deI = cgI = ascI = aspI = 0;
    start = false;
}
//--------------------------------------------------------------------------------
void Game::InitMtxFont()
{
    mMtxFont = new char[128][7][5];
    For (i, 128) For (j, 7) For (k, 5) mMtxFont[i][j][k] = 1;
    const char F0[] =
        "11000" "11001" "00010" "00100" "01000" "10011" "00011"//%
        "11111" "11111" "11111" "11111" "11111" "11111" "11111"//
        "00100" "00100" "00100" "00000" "00000" "00000" "00000"//'
        "00001" "00010" "00100" "00100" "00100" "00010" "00001"//(
        "10000" "01000" "00100" "00100" "00100" "01000" "10000"//)
        "00100" "11111" "00100" "01010" "01010" "00000" "00000"//*
        "00000" "11111" "11111" "11111" "11111" "11111" "00000"//cell
        "11111" "11111" "11111" "11111" "11111" "11111" "11111"//
        "00000" "00000" "00000" "11111" "00000" "00000" "00000"//-
        "00000" "00000" "00000" "00000" "00000" "01100" "01100"//.
        "11111" "11111" "11111" "11111" "11111" "11111" "11111"//
        "01110" "10001" "10011" "10101" "11001" "10001" "01110"//0
        "00100" "01100" "00100" "00100" "00100" "00100" "01110"//1
        "01110" "10001" "00001" "00010" "00100" "01000" "11111"//2
        "01110" "10001" "00001" "00110" "00001" "10001" "01110"//3
        "00010" "00110" "01010" "10010" "11111" "00010" "00111"//4
        "11111" "10000" "11110" "00001" "00001" "10001" "01110"//5
        "01110" "10001" "10000" "11110" "10001" "10001" "01110"//6
        "11111" "10001" "00010" "00010" "00100" "00100" "00100"//7
        "01110" "10001" "10001" "01110" "10001" "10001" "01110"//8
        "01110" "10001" "10001" "01111" "00001" "00001" "01110"//9
        "00000" "01100" "01100" "00000" "01100" "01100" "00000"//:
        "00000" "00110" "00110" "00000" "00110" "01100" "01000"//;
        "00001" "00010" "00100" "01000" "00100" "00010" "00001"//<
        "00000" "00000" "11111" "00000" "11111" "00000" "00000"//=
        "10000" "01000" "00100" "00010" "00100" "01000" "10000"//>
        "01110" "10001" "10001" "00010" "00100" "00000" "00100"//?
        "01110" "10001" "10011" "10101" "10111" "10000" "01110"//@
        "01110" "10001" "10001" "11111" "10001" "10001" "10001"//A
        "11110" "10001" "10001" "11110" "10001" "10001" "11110"//B
        "01110" "10001" "10000" "10000" "10000" "10001" "01110"//C
        "11110" "10001" "10001" "10001" "10001" "10001" "11110"//D
        "11111" "10000" "10000" "11110" "10000" "10000" "11111"//E
        "11111" "10000" "10000" "11110" "10000" "10000" "10000"//F
        "01110" "10001" "10000" "10111" "10001" "10001" "01110"//G
        "10001" "10001" "10001" "11111" "10001" "10001" "10001"//H
        "01110" "00100" "00100" "00100" "00100" "00100" "01110"//I
        "00001" "00001" "00001" "00001" "10001" "10001" "01110"//J
        "10001" "10010" "10100" "11000" "10100" "10010" "10001"//K
        "10000" "10000" "10000" "10000" "10000" "10000" "11111"//L
        "10001" "11011" "10101" "10101" "10001" "10001" "10001"//M
        "10001" "10001" "11001" "10101" "10011" "10001" "10001"//N
        "01110" "10001" "10001" "10001" "10001" "10001" "01110"//O
        "11110" "10001" "10001" "11110" "10000" "10000" "10000"//P
        "01110" "10001" "10001" "10001" "10101" "10010" "01101"//Q
        "11110" "10001" "10001" "11110" "10100" "10010" "10001"//R
        "01111" "10000" "10000" "01110" "00001" "00001" "11110"//S
        "11111" "00100" "00100" "00100" "00100" "00100" "00100"//T
        "10001" "10001" "10001" "10001" "10001" "10001" "01110"//U
        "10001" "10001" "10001" "10001" "10001" "01010" "00100"//V
        "10001" "10001" "10001" "10101" "10101" "10101" "01010"//W
        "10001" "10001" "01010" "00100" "01010" "10001" "10001"//X
        "10001" "10001" "10001" "01010" "00100" "00100" "00100"//Y
        "11111" "00001" "00010" "00100" "01000" "10000" "11111";//Z
    For (i, 54) For (j, 7) For (k, 5)
        mMtxFont[i + 37][j][k] = F0[35 * i + 5 * j + k];
}
//--------------------------------------------------------------------------------
void Game::DrawString(int x, int y, const char* fmt, ...)
{
    char text[256];
    va_list	ap;
    if (fmt == NULL) return;
    va_start(ap, fmt);
#ifdef _WIN32 //32-bit and 64-bit Windows
    vsprintf_s(text, fmt, ap);
#else //Typically Unix such as Mac OSX or Linux
    vsprintf(text, fmt, ap);
#endif
    va_end(ap);
    int n = 0;
    glPushMatrix();
    glLoadIdentity();
    glPointSize(1.4f);
    glBegin(GL_POINTS);
    while (text[n] != 0)
        DrawChar(x + 7 * n, y, text[n++]);
    glEnd();
    glPopMatrix();
}
//--------------------------------------------------------------------------------
void inline Game::DrawChar(int x, int y, Uint8 c)
{
    if (c == ' ') return;
    For (m, 7) For(n, 5)
    {
        if (mMtxFont[c][m][n] == '0') continue;
        glVertex2i(x + n, y + m);
    }
}
//--------------------------------------------------------------------------------
void Game::DrawRectangle(int x, int y, int width, int height)
{
    glLineWidth(LINE_WIDTH);
    glBegin(GL_LINE_LOOP);
    glVertex2i(offsetX - (POINT_SIZE / 2) + (SPACING * x),
               offsetY - (SPACING / 2) + (SPACING * y));
    glVertex2i(offsetX - (POINT_SIZE / 2) + (SPACING * (x + width)),
               offsetY - (SPACING / 2) + (SPACING * y));
    glVertex2i(offsetX - (POINT_SIZE / 2) + (SPACING * (x + width)),
               offsetY - (SPACING / 2) + (SPACING * (y + height)));
    glVertex2i(offsetX - (POINT_SIZE / 2) + (SPACING * x),
               offsetY - (SPACING / 2) + (SPACING * (y + height)));
    glEnd();
}
//--------------------------------------------------------------------------------
void Game::DrawCell(int x, int y)
{
    glPointSize(POINT_SIZE);
    glBegin(GL_POINTS);
    glVertex2i(offsetX + SPACING * x, offsetY + SPACING * y);
    glEnd();
}
//--------------------------------------------------------------------------------
void Game::DrawLine(int x1, int y1, int x2, int y2, int lineWidth)
{
    glLineWidth(lineWidth);
    glBegin(GL_LINES);
    glVertex2i(offsetX + SPACING * x1, offsetY + SPACING * y1);
    glVertex2i(offsetX + SPACING * x2, offsetY + SPACING * y2);
    glEnd();
}
//--------------------------------------------------------------------------------
bool Game::GridHasValidStartCell()
{
    for (int x = 1; x < GRID_SIZE_X; x += 2)
        for (int y = 1; y < GRID_SIZE_Y; y += 2)
            if (!cG[x][y]->isPassable)
                return true;
    return false;
}
//--------------------------------------------------------------------------------
int Game::DistanceHeuristic(int x1, int y1, int x2, int y2, int diagonalCost)
{
    int dX = abs(x1 - x2);
    int dY = abs(y1 - y2);
    if (dX > dY)
        return diagonalCost * dY + 10 * (dX - dY);
    return diagonalCost * dX + 10 * (dY - dX);
}
//--------------------------------------------------------------------------------