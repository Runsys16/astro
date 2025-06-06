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

#include "MathlibD.h"
#include <WindowsManager.h>
#include "Singleton.h"
#include "serial.h"
#include "main.h"
#include "console.h"
#include "serveur_mgr.h"
#include "var_mgr.h"

using namespace std;

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
class Console;
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
SINGLETON_BEGIN( PanelConsoleSerial )
protected:

    PanelWindow*                    pw;
    PanelConsole*                   pc;
    Console*                        cb;
    bool                            bVisible;
    double                          ad;
    double                          dc;
    double                          ad_change;
    double                          dc_change;

public :
    PanelConsoleSerial();

    bool                            keyboard(char key, int x, int y);
    void                            writeln( char * );
    void                            idleGL();
    void                            setVisible(bool);

inline bool                         getVisible()                { return pw->getVisible(); }
inline PanelWindow*                 getWindow()                 { return pw; }
inline PanelConsole*                getConsole()                { return pc; }
inline void                         setPrompt(string s)         { pc->setPrompt(s); }

    void                            setColor(long l )           { pw->setColor(l); }

   
SINGLETON_END()

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#endif
