#ifndef PANEL_APN_H
#define PANEL_APN_H  1

//#include "camera.h"
#include <dirent.h>
#include <algorithm>

#include "main.h"
#include "var_mgr.h"
#include <WindowsManager.h>
#include "surveillance.h"


using namespace std;

class PanelApn : public PanelWindow
{

protected:
    PanelEditText*                  pTime;
    PanelEditText*                  pIso;
    PanelEditText*                  pFrames;
    PanelEditText*                  pTimeOut;
    PanelEditText*                  pNum;

    string                          sTime;
    int                             iIso;
    int                             iFrames;
    int                             iTimeOut;
    int                             iNum;

public:
    ~PanelApn();
    PanelApn();
    
    void                            setVisible(bool);

    void                            supCallBacks();
    bool                            keyboard(char key, int x, int y);
    void                            saveValues();
    void                            commande_photo();
    void                            photo();
};


#endif
