//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#ifndef PANELGRAPH_H
#define PANELGRAPH_H  1
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#include <GL/glew.h>
#include <GL/glut.h>

#include <stdio.h>
#include <complex>
#include <valarray>
 
#include "MathlibD.h"
#include <WindowsManager.h>
#include "main.h"
#include "MathlibD.h"
#include "console.h"
#include "var_mgr.h"
#include "timer.h"
//#include "panel_spin_edit_text.h"



using namespace std;
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
class PanelGraph : public PanelWindow
{


private:
	double				dXmin;
	double				dXmax;
	double				dYmin;
	double				dYmax;

public :
    ~PanelGraph();
    PanelGraph();

    void                init_var();

    void                glCarre( int, int, int, int );
    void                glEchelleAxe( int, int, float, float, PanelText*, PanelText* );
    void                glEchelle();

    void                build_unites_text(void);
    int                 sc2winX(int);
    int                 sc2winY(int);
    
virtual void            displayGL( void );
virtual void            updatePos( void );

virtual void            clickLeft( int, int);
virtual void            motionLeft( int, int);
virtual void            releaseLeft( int, int);

virtual void            clickMiddle( int, int);
virtual void            motionMiddle( int, int);
virtual void            releaseMiddle( int, int);
    

};
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#endif
