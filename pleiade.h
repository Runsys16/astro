#ifndef PLEIADE_H
#define PLEIADE_H  1

#include "camera.h"

#include <WindowsManager.h>



class Pleiade : public Camera
{
protected:
    thread                      thread_chargement_pleiade;
    bool                        bCharged;
    bool                        bCharging;
    bool                        bFreePtr;
    bool                        bFirst;
    //char                        sPleiade[255];
    string                      sPleiade;
    string                      sPleiades;
    
    GLubyte*                    ptr;

    int                         plus;
    int                         count_png;
    bool                        bNewBackground;

public :
    Pleiade();
    ~Pleiade();

    virtual void                change_background_camera();

    void                        charge_background();
    std::thread                 startThread();
    virtual GLubyte*            getPtr();
    virtual bool                haveNewFrame();
    virtual void                haveUseFrame(bool);


};


#endif
