#ifndef PANEL_APN_H
#define PANEL_APN_H  1

//#include "camera.h"
#include <dirent.h>
#include <algorithm>

#include "main.h"
#include "var_mgr.h"
#include <WindowsManager.h>

using namespace std;

class PanelApn : public PanelWindow
{

protected:
    PanelEditText*                  pTime;
    PanelEditText*                  pIso;
    PanelEditText*                  pFrames;
    PanelEditText*                  pTimeOut;

public:
    ~PanelApn();
    PanelApn();

    bool                            keyboard(char key, int x, int y);
    
};


#endif
