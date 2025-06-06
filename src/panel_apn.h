#ifndef PANEL_APN_H
#define PANEL_APN_H  1

//#include "camera.h"
#include <dirent.h>
#include <algorithm>

#include "main.h"
#include "var_mgr.h"
#include <WindowsManager.h>
#include "surveillance.h"
//#include "panel_spin_edit_text.h"


using namespace std;

class PanelApn : public PanelWindow
{

protected:
    PanelSpinEditText*              pTime;
    PanelSpinEditText*              pIso;
    PanelSpinEditText*              pFrames;
    PanelSpinEditText*              pTimeOut;
    PanelSpinEditText*              pNum;

    string                          sTime;
    float                           fTime;
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
    
virtual void                        displayGL();
};


#endif
