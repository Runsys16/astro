#ifndef CAMERA_H
#define CAMERA_H  1

#include "v4l2.h"

#include <WindowsManager.h>
#include <thread>


//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
class Camera : public Device_cam
{
private:
    PanelWindow *               panelControl;
    PanelSimple *               panelPreview;

    PanelText*                  pCamFilename;
    ivec2                        vCameraSize;
    int                         xCam, yCam, dxCam, dyCam;
    
    bool                        bChargingCamera;
    thread*                     pthread_chargement_camera;

public :
    Camera();
    Camera(int, int);
    void                        createControlID(PanelSimple * p, int x, int y, char* str);
    void                        createControlIDbyID(PanelSimple * p, int x, int y, char* str, int id);
    void                        CreateControl();
    void                        CreatePreview();
    void                        resizePreview(int, int);
    void                        change_background_camera();
    //void                        charge_image_camera();
    
    void                        setVisible(bool b);
    
private:
};

#endif

