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
    vector<int>						tTabOld;
    vector<int>						tTabNew;
	bool							bChangeTab;// = false;

    

public:
    ~PanelStdOut();
    PanelStdOut();

virtual     void                    displayGL();
virtual     void                    updatePos();
    
    void                            setColor(unsigned long);
    void                            affiche(char*);
    void                            change_tab_size();
    void							add_tab_size(int);
    void                            restaure_tab_size();
};


#endif
