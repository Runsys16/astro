#ifndef PANEL_GRAPH_CPP
#define PANEL_GRAPH_CPP

#include "panel_graph.h"

#include <sys/stat.h>
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelGraph::~PanelGraph()
{
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelGraph::PanelGraph()
{
    logf( (char*)" Constructeur  PanelGraph::PanelGraph()" );
    log_tab(true);
    
	WindowsManager&     wm  = WindowsManager::getInstance();
    VarManager&         var = VarManager::getInstance();

    init_var();
    setSize( 600, 400 );

    
    setBackground((char*)"images/background.tga");

    
    setExtraString("PanelGraph");

    log_tab(false);
    logf( (char*)" Constructeur  PanelGraph::PanelGraph()     -----END-----" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::init_var()
{
    VarManager&         var = VarManager::getInstance();

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::glCarre( int x, int y, int dx,  int dy )
{
    int delta_x = 0;
    int delta_y = 0;
    
    x += getPosX();// + dx_screen;
    y += getPosY();// + dy_screen;
    
	glBegin(GL_LINES);
        x = x-dx;
        y = y-dy;
        
        glVertex2i(x,y);                glVertex2i(x+2*dx,y);
        glVertex2i(x+2*dx,y);           glVertex2i(x+2*dx,y+2*dy);
        glVertex2i(x+2*dx,y+2*dy);      glVertex2i(x,y+2*dy);
        glVertex2i(x,y+2*dy);           glVertex2i(x,y);

    glEnd();        
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::glEchelleAxe( int AXE, int SIZE, float max, float min, PanelText* pMax, PanelText* pMin )
{
    VarManager& var = VarManager::getInstance();

    float gris = 0.3;
    vcf4 color, colorLimit, colorAxe;
    
    if ( var.getb("bNuit") )
    {
        color       = vcf4( gris, 0.0, 0.0, 1.0 );    
        colorLimit  = vcf4( 0.8, 0.0, 0.0, 1.0 );    
        colorAxe    = vcf4( 1.0, 0.0, 0.0, 1.0 );
    }
    else                            
    {
        //gris = 0.4;
        color       = vcf4( gris, gris, gris, 1.0 );
        colorLimit  = vcf4( 1.0, 0.0, 0.0, 1.0 );
        colorAxe    = vcf4( 1.0, 1.0, 1.0, 1.0 );
    }
    
    glBegin(GL_LINES);

    glEnd();        


    glBegin(GL_LINES);
    glEnd();        
        glDisable(GL_LINE_STIPPLE);    
        glColor4fv( (GLfloat*)&colorAxe );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::glEchelle()
{
    VarManager& var = VarManager::getInstance();

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::displayGL(void)
{
	if ( !visible )			return;

	WindowsManager& wm  = WindowsManager::getInstance();
    VarManager&     var = VarManager::getInstance();

	/*
    if ( var.getb("bNuit") )        glColor4f( 1.0, 0.0, 0.0, 1.0 );
    else                            glColor4f( 1.0, 1.0, 1.0, 1.0 );

    unsigned int color; 
    if ( var.getb("bNuit") )        color = 0xff0000ff;
    else                            color = 0xffffffff;
    */
    PanelWindow::displayGL();
    

    float height = (float)wm.getHeight();
	int   scx  = getX();
	int   scy  = height - getDY() - getY();
	int   scdx = getDX();
	int   scdy = getDY();
    
    glScissor( scx, scy, scdx, scdy );
    glEnable( GL_SCISSOR_TEST );

    //glCourbes();

    glDisable( GL_SCISSOR_TEST );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::updatePos()
{
    PanelSimple::updatePos();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::clickLeft( int xm, int ym )
{
    logf( (char*)"PanelGraph::clickLeft( %d, %d )", xm, ym );

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::motionLeft( int xm, int ym )
{
    //logf( (char*)"PanelGraph::motionLeft( %d, %d )", xm, ym );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::releaseLeft( int xm, int ym )
{
    logf( (char*)"PanelGraph::releaseLeft( %d, %d )", xm, ym );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::clickMiddle( int xm, int ym )
{
    //logf( (char*)"PanelGraph::clickMiddle( %d, %d )", xm, ym );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::motionMiddle( int xm, int ym )
{
    //logf( (char*)"PanelGraph::motionMiddle( %d, %d )", sc2winX(xm), sc2winY(ym) );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::releaseMiddle( int xm, int ym )
{
    logf( (char*)"PanelGraph::releaseMiddle( %d, %d )", xm, ym );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#endif


