#ifndef PANEL_ENREGISTRE_H
#define PANEL_ENREGISTRE_H  1

//#include "camera.h"
#include <dirent.h>
#include <algorithm>

#include "main.h"
#include "var_mgr.h"
#include <WindowsManager.h>
#include "surveillance.h"
//#include "panel_spin_edit_text.h"


using namespace std;

class PanelEnregistre : public PanelWindow, public ChangeValue, public CallBackMouse, public ButtonCallBack
{
public:
	enum UNITE { MICRO=0, MILI, SEC };

protected:
    PanelSpinEditText*              pFrames;
    PanelSpinEditText*              pTempo;
    PanelButton*					pOK;
    PanelButton*					pQUIT;
    PanelEditText*					pDirname;
    PanelEditText*					pFilename;
    PanelCheckBox*					pCheckSuivi;
    PanelTextOmbre*					pCameraName;
    PanelTextOmbre*					pSuiviName;

	string							sDirname;
	string							sFilename;

    string                          sTime;
    double                          dTime;
    int                             iFrames;
    int                             iTempo;
    
    thread							th_shooting;
    bool							bShooting;
    bool							bExitShooting;
    bool							bCheckSuivi;
    
    Camera*							pCamera;
    Camera*							pCameraSuivi;

public:
    ~PanelEnregistre();
    PanelEnregistre();
    
	void							init();
	void							init_panel();
    void                            setVisible(bool);

    void                            supCallBacks();
    bool                            keyboard(char key, int x, int y);
    void                            saveValues();

	Camera*							find_camera();
	Camera*							find_camera_suivi();
volatile bool						check_suivi();
	void							shooting();
    void                            commande_photo();

inline	void						stop()				{ bExitShooting = true; }
inline	bool						getShooting()		{ return bShooting; }
inline	string&						getFilename()		{ return sFilename; }
inline	string&						getDirname()		{ return sDirname; }
    
virtual void						changeValueDouble( double val, void *p );
virtual void						changeValueDoubleClick( double val, void *p );

virtual void                        updatePos();
virtual void                        displayGL();
virtual void                        idle(float);

virtual void						releaseLeftCB( int, int, void*);
		void						start_filebrowser();
virtual void						callback( bool, int, char* );

};


#endif
