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
    void                        add( string dev_name );
    void                        add( Camera* );

    void                        change_background_camera();
    void                        start_camera();
    void                        stop_camera();
    void                        keyboard( char key );
    void                        idleGL();
    void                        onBottom();
    void                        resize( int, int );
    void                        active();

    GLubyte*                    getPtr();

    int                         get_xCam();
    int                         get_yCam();
    int                         get_dxCam();
    int                         get_dyCam();
    
   
private:
    vector<Camera*>     pCameras;
    Camera*             pCurrent;
    
    int                 nActive;
    
    
SINGLETON_END()

#endif

