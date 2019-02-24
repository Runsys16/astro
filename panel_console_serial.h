//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#ifndef PANELCONSOLE_SERIAL_H
#define PANELCONSOLE_SERIAL_H  1
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#include <GL/glew.h>
#include <GL/glut.h>

#include <stdio.h>

#include <WindowsManager.h>
#include "Singleton.h"
#include "serial.h"
#include "main.h"
#include "console.h"
#include "serveur_mgr.h"

using namespace std;

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
class Console;
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
SINGLETON_BEGIN( PanelConsoleSerial )

public :
    PanelConsoleSerial();

    bool                            keyboard(char key, int x, int y);
    void                            writeln( char * );
    void                            idleGL();
    
    
inline PanelConsole*                getConsole()                { return pc; }

inline bool                         getVisible()                { return pw->getVisible(); }
inline void                         setVisible(bool b)          { bVisible=b;  pw->setVisible(b); }
    
protected:

    PanelWindow*                    pw;
    PanelConsole*                   pc;
    Console*                        cb;
    bool                            bVisible;
    float                           ad;
    float                           dc;
    float                           ad_change;
    float                           dc_change;


    
SINGLETON_END()

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#endif
