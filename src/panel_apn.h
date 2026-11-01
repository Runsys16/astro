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

class PanelApn : public PanelWindow, public ChangeValue
{
public:
	enum UNITE { MICRO=0, MILI, SEC };

protected:
    PanelSpinEditText*              pTime;
    PanelSpinEditText*              pIso;
    PanelSpinEditText*              pFrames;
    PanelSpinEditText*              pTimeOut;
    PanelSpinEditText*              pNum;
    //PanelText*						pUnite;

    string                          sTime;
    double                          dTime;
    long							exposure;
    int                             iIso;
    int                             iFrames;
    int                             iTimeOut;
    int                             iNum;
    UNITE							iUnite;

public:
    ~PanelApn();
    PanelApn();
    
    void                            setVisible(bool);

    void                            supCallBacks();
    bool                            keyboard(char key, int x, int y);
    void                            saveValues();
    void                            commande_photo();
    void                            photo();
    void                            change_unite(bool, PanelSpinEditText*);
    
virtual void						changeValueDouble( double val, void *p );
virtual void						changeValueDoubleClick( double val, void *p );

virtual void                        updatePos();
virtual void                        displayGL();
};


#endif
