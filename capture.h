#ifndef CAPtuRE_H
#define CAPtuRE_H  1

#include "camera.h"

#include <WindowsManager.h>



class Capture : public Camera
{
protected:
    bool                        bNewBackground;
    bool                        bFirst;

public :
    Capture();

    virtual void                change_background_camera();

    void                        threadExtractImgPleiade();
    std::thread                 startThread();
    virtual GLubyte*            getPtr();
    virtual bool                haveNewFrame();
    virtual void                haveUseFrame(bool);


};


#endif
