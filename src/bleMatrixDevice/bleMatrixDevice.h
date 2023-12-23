
#pragma once

#include "battHandler.h"

void bleMatrixLoop();
void bleMatrixDrawBattState();

void bleMatrixInit();
void bleMatrixRedraw();


class bleMatrixDevice
{
    public:
    bleMatrixDevice(){};

    private:
    virtual void init();
    virtual void loop();
    virtual void redraw();
    virtual void drawBattState();
};


#ifdef CBADGE_BT_CLASSIC
    #include "bleMatrixDevice/bleMatrixDevice1_0.h"
#else
    #include "bleMatrixDevice/bleMatrixDevice2_0.h"
#endif