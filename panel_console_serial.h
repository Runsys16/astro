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

using namespace std;

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
/*
template <class T> class Singl
{
public:
        static T& getInstance()         { if ( m_i != NULL )    m_i = new T(); return *m_i; }
        static void Kill()              { if ( m_i != NULL )    delete m_i; m_i = NULL; }
protected:
        static T* m_i;
private:
        T& operator= (const T&){}
};
*/
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
class Callback : public PanelConsoleCallBack
{
public:
    Callback();
	virtual void					callback_cmd(std::string);
};
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
SINGLETON_BEGIN( PanelConsoleSerial )

public :
    PanelConsoleSerial();

    void                            afficheHelp();    
    bool                            keyboard(char key, int x, int y);
    void                            writeln( char * );
inline PanelConsole*                getConsole()                { return pc; }

inline bool                         getVisible()                { return pw->getVisible(); }
inline void                         setVisible(bool b)          { bVisible=b;  pw->setVisible(b); }
    
protected:

    PanelWindow*                    pw;
    PanelConsole*                   pc;
    Callback*                       cb;
    bool                            bVisible;

    
SINGLETON_END()

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#endif
