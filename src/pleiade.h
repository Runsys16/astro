#ifndef PLEIADE_H
#define PLEIADE_H  1

#include "camera.h"

#include <WindowsManager.h>



class Pleiade : public Camera
{
protected:
    //bool                        bCharged;
    //bool                        bCharging;
    bool                        bFreePtr;
    bool                        bFirst;
    string                      sPleiade;
    string                      sPleiades;
    
    GLubyte*                    ptr;

    int                         plus;
    int                         count_png;
    bool                        bNewBackground;

public :
    Pleiade();
    ~Pleiade();

    //virtual void                update();
    virtual void                start_thread();
    void                        charge_background();
    virtual void                change_background();

    std::thread                 startThread();
    virtual GLubyte*            getPtr();
    virtual bool                haveNewFrame();
    virtual void                haveUseFrame(bool);

	virtual void				togglePanel()			{};

};


#endif
