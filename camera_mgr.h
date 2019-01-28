#ifndef CAMERA_MGR_H
#define CAMERA_MGR_H  1


#include "camera.h"

#include <WindowsManager.h>
#include <thread>




//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#include "Singleton.h"

SINGLETON_BEGIN( Camera_mgr )

public:
    Camera_mgr();
    void                add( string dev_name );

    void                change_background_camera();
    void                start_camera();
    void                stop_camera();
    
    
private:
    vector<Camera*>     pCameras;
    Camera*             pCurrent;
    
    
SINGLETON_END()

#endif

