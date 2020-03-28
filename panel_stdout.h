#ifndef PANEL_STDOUT_H
#define PANEL_STDOUT_H  1

//#include "camera.h"
#include <dirent.h>
#include <algorithm>

#include "main.h"
#include "var_mgr.h"
#include <WindowsManager.h>
#include "surveillance.h"


using namespace std;

class PanelStdOut : public PanelWindow
{

protected:
    PanelScrollText*                pScroll;

public:
    ~PanelStdOut();
    PanelStdOut();

virtual     void                    displayGL();
virtual     void                    updatePos();
    
    void                            setColor(unsigned long);
    void                            affiche(char*);
};


#endif
