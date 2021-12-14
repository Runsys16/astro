#ifndef CAMERA_H
#define CAMERA_H  1

#include "v4l2.h"
#include "stars.h"
#include "timer.h"
#include "panel_camera.h"

#include <WindowsManager.h>
#include <thread>
#include <atomic>
#include "button_callback.h"




using namespace std;
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
class Camera : public Device_cam
{
protected:
    PanelWindow *               panelControl;
    PanelCamera *               panelPreview;
    PanelText*                  pPanelName;
    PanelText*                  pCamFilename;
    PanelText*                  pNbStars;

    int                         xCam, yCam, dxCam, dyCam;
    
    std::atomic<bool>           bCharging;
    thread                      thread_chargement;
    atomic<bool>                bStartThread;
    atomic<bool>                bExitThread;
    
    float                       hz;
    float                       previousTime;
    int                         nb_images;

    ivec2                       vCameraSize;
    string                      sSauveDir;

    //Stars                       stars;
    
    
    
    
public :
virtual    ~Camera();
    Camera();
    Camera(int, int);
    
    void                        init();
    
    int                         addControl();
    void                        createControlID(PanelSimple * p, int x, int y, char* str);
    void                        createControlIDbyID(PanelSimple * p, int x, int y, char* str, int id);
    void                        CreateControl();

//    void                        update();

    virtual void                start_thread();
    virtual void                CreatePreview();
    virtual void                resizePreview(int, int);
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
    
    void                        setColor(long);

inline bool                     getControlVisible()                 { return panelControl!= NULL  ? panelControl->getVisible() : false; }
inline PanelCamera *            getPanelPreview()                   { return panelPreview; }
inline float                    getHertz()                          { return hz; }
inline PanelText *              getPanelName()                      { return pPanelName; }
inline PanelText *              getpCamFilename()                   { return pCamFilename; }
inline PanelText *              getPanelNbStars()                   { return pNbStars; }
inline int                      getNbStars()                        { return panelPreview->getNbStars(); }

inline void                     add_catalogue(StarCatalog* p)       { panelPreview->add_catalogue(p); }
inline void                     setRefCatalog(double _0, double _1) { panelPreview->setRefCatalog(_0, _1); }

};

#endif

