#ifndef CAMERA_H
#define CAMERA_H  1

#include "v4l2.h"
#include "stars.h"
#include "timer.h"
#include "panel_camera.h"
//#include "panel_spin_edit_text.h"

#include <WindowsManager.h>
#include <thread>
#include <atomic>
#include "button_callback.h"




using namespace std;
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
class Camera : public Device_cam , public ChangeValue 
{
protected:
    PanelWindow *               panelControl;
    PanelCamera *               panelCamera;
    PanelText*                  pPanelName;
    PanelText*                  pCamFilename;
    PanelText*                  pNbStars;

    int                         xCam, yCam, dxCam, dyCam;
    
    thread                      thread_chargement;
    atomic<bool>				bCharging;
    atomic<bool>                bStartThread;
    atomic<bool>                bExitThread;
    
    float                       hz;
    float                       previousTime;
    int                         nb_images;

    ivec2                       vCameraSize;
    string                      sSauveDir;
    
    bool						bOnScreen;
    bool						bPanelControl;
    bool						bFullScreen;
    bool						bIconized;

    //Stars                       stars;
    
    
    
    
public :
virtual    ~Camera();
    Camera();
    Camera(int, int);
    
virtual void 					changeValueDouble( double val, void *p );

    void                        init();
    
    int                         addControlName(const string&, int, int);
    int                         addControl();
    void                        createControlID(PanelSimple * p, int x, int y, char* str);
    void                        createControlIDminmax(PanelSimple * p, int x, int y, char* str0 );
    void                        CreateControl();

//    void                        update();

    virtual void                start_thread();
    		void                stop_thread()								{ bExitThread = true; }
    virtual void                CreatePreview();
    virtual void                iconSizePreview(int, int);
    virtual void                fullSizePreview(int, int);

    virtual void                resizeControl(int, int);
    void                        resizeControlFirst(int, int, int, int);

    void                        threadExtractImg();
    virtual void                change_background();
    
    bool                        keyboard( char );
    
    //std::thread                 memberThread();
    
    void                        setVisible(bool b);
    void                        setControlVisible(bool b);
    void                        togglePanel();
    
    

    virtual GLubyte*            getPtr();
    virtual bool                haveNewFrame();
    virtual void                haveUseFrame(bool);

    int                         get_xCam();
    int                         get_yCam();
    int                         get_dxCam();
    int                         get_dyCam();
    ivec2                       get_vCameraSize();

    vec2*                       getSuivi();
    void						recentreSuivi();
    
    void                        setColor(long);

inline bool                     getControlVisible()                 { return panelControl!= NULL  ? panelControl->getVisible() : false; }
inline PanelCamera *            getPanelCamera()                   { return panelCamera; }
inline PanelWindow *            getPanelControl()                   { return panelControl; }
inline float                    getHertz()                          { return hz; }
inline PanelText *              getPanelName()                      { return pPanelName; }
inline PanelText *              getpCamFilename()                   { return pCamFilename; }
inline PanelText *              getPanelNbStars()                   { return pNbStars; }
inline int                      getNbStars()                        { return panelCamera->getNbStars(); }

inline void                     add_catalogue(StarCatalog* p)       { panelCamera->add_catalogue(p); }
inline void                     setRefCatalog(double _0, double _1) { panelCamera->setRefCatalog(_0, _1); }

inline	bool					getOnScreen()						{ return bOnScreen;}
inline	void					setOnScreen(bool b)					{ bOnScreen = b;}

};

#endif

