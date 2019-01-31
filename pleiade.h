#ifndef PLEIADE_H
#define PLEIADE_H  1

#include "camera.h"

#include <WindowsManager.h>



class Pleiade : public Camera
{
protected:
    thread                      thread_chargement_pleiade;
    bool                        bChargingPleiades;
    bool                        bFreePtr;
    bool                        bFirst;
    char                        sPleiade[255];
    struct readBackground       readB;
    
    GLubyte*                    ptr;

public :
    Pleiade();

    virtual void                change_background_camera();

    void                        threadExtractImgPleiade();
    std::thread                 memberThreadPleiade();

};


#endif
