#ifndef CAMERA_MGR_H
#define CAMERA_MGR_H  1


#include "camera.h"
#include "serial.h"

#include <WindowsManager.h>
#include <thread>




//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#include "Singleton.h"

SINGLETON_BEGIN( Camera_mgr )

public:
    Camera_mgr();
    bool                        camera_exist( string dev_name );
    void                        add( string dev_name );
    void                        add( Camera* );
    void                        addImages( Camera* );
    void                        sup( string );
    void                        sup( Camera * );

    void                        change_background_camera();
    void                        start_camera();
    void                        stop_camera();
    bool                        keyboard( char key );
    //void                        idleGL();
    void                        onBottom();
    void                        resize( int, int );
    void                        active();
    void                        reOrder();
    void                        togglePanel();

    GLubyte*                    getPtr();

    int                         get_xCam();
    int                         get_yCam();
    int                         get_dxCam();
    int                         get_dyCam();
    ivec2                       get_vCameraSize();
    
    int                         getSize();
    int                         getNum(Camera *);
    Camera*                     getCamera(string dev);
    
    void                        print_list();
 
    bool                        haveNewFrame();
    void                        haveUseFrame(bool);
    
    void                        findAllStars();
    void                        deleteAllStars();
    
    void                        suivi();

    rb_t *                      getRB();

    inline Camera*              getCurrent()                                { return pCurrent; }   
   
    void                        update();   
   
   
private:
    vector<Camera*>             pCameras;
    Camera*                     pCurrent;
    
    int                         nActive;
    
    
SINGLETON_END()

#endif

