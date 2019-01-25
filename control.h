#ifndef CONTROL_H
#define CONTROL_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>

#include <GL/glew.h>
#include <GL/glut.h>

#include <linux/videodev2.h>

#include "Mathlib.h"
#include <WindowsManager.h>

#include "v4l2.h"


class Device_cam;

class Control
{
private:
    int             id;
    int             min;
    int             max;
    int             step;
    int             def;
    int             value;
    string          name;
    
    Device_cam*     dev;
    PanelText*      pt;    
    
public:
    Control(Device_cam * p);
    void            init( struct v4l2_queryctrl* queryctrl);
    bool            find(string);
    void            plus();
    void            moins();
    void            printValue();
    
    inline void     setPanelText( PanelText* p)         { pt = p; }
    inline int      getValue()                          { return value; }
    inline int      getId()                             { return id; }
    
    
};

#endif
