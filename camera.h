#ifndef CAMERA_H
#define CAMERA_H  1

#include "v4l2.h"
#include "timer.h"

#include <WindowsManager.h>
#include <thread>

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
    
    float                       hz;
    float                       previousTime;

protected:
    ivec2                        vCameraSize;

public :
    Camera();
    Camera(int, int);
    
    void                        init();
    
    void                        addControl();
    void                        createControlID(PanelSimple * p, int x, int y, char* str);
    void                        createControlIDbyID(PanelSimple * p, int x, int y, char* str, int id);
    void                        CreateControl();

    virtual void                CreatePreview();
    virtual void                resizePreview(int, int);
    virtual void                fullSizePreview(int, int);
    virtual void                resizeControl(int, int);

    void                        threadExtractImg();
    virtual void                change_background_camera();
    
    void                        keyboard( char );
    
    std::thread                 memberThread();
    
    void                        setVisible(bool b);
    
    inline PanelWindow *        getPanelPreview()               { return panelPreview; }
    inline float                getHertz()                      { return hz; }
private:
};

#endif

