#ifndef CAMERA_H
#define CAMERA_H  1

#include "v4l2.h"
#include "timer.h"

#include <WindowsManager.h>
#include <thread>


using namespace std;
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
class Camera : public Device_cam
{
protected:
    PanelWindow *               panelControl;
    PanelWindow *               panelPreview;
    //PanelSimple *               panelPreview;

    PanelText*                  pCamFilename;
    int                         xCam, yCam, dxCam, dyCam;
    
    bool                        bChargingCamera;
    thread                      thread_chargement_camera;
    bool                        startThread;
    
    float                       hz;
    float                       previousTime;
    int                         nb_images;

    ivec2                       vCameraSize;

public :
virtual    ~Camera();
    Camera();
    Camera(int, int);
    
    void                        init();
    
    int                         addControl();
    void                        createControlID(PanelSimple * p, int x, int y, char* str);
    void                        createControlIDbyID(PanelSimple * p, int x, int y, char* str, int id);
    void                        CreateControl();

    virtual void                CreatePreview();
    virtual void                resizePreview(int, int);
    virtual void                fullSizePreview(int, int);

    virtual void                resizeControl(int, int);
    void                        resizeControlFirst(int, int, int, int);

    void                        threadExtractImg();
    virtual void                change_background_camera();
    
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

inline bool                     getControlVisible()             { return panelControl!= NULL  ? panelControl->getVisible() : false; }
inline PanelWindow *            getPanelPreview()               { return panelPreview; }
inline float                    getHertz()                      { return hz; }

    //inline int                  getFd()                         { return getFd
};

#endif

