#include "panel_zoom.h"

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelZoom::~PanelZoom()
{
    WindowsManager& wm = WindowsManager::getInstance();

    logf( (char*)"Destructeur PanelZoom()");
    wm.sup( this );
    if ( pFond!= NULL )
    {
        sup(pFond);
        pFond->setBackground( (_Texture2D*)NULL);
        //pFond->deleteBackground();
        delete pFond;
        pFond = NULL;
    }
    wm.sup(this);

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelZoom::PanelZoom()
{
    WindowsManager& wm = WindowsManager::getInstance();

    logf( (char*)"Constructeur PanelZoom()");
    echelle = 1.0;
    ratio   = 1.0;
    dx      = 0.0;
    dy      = 0.0;
    pReadBgr = NULL;
    
    pFond = new PanelSimple();
    add(pFond);
    pFond->setPos(0,0);
    pFond->setFantome(true);
    //setBackground( (_Texture2D*)NULL );
    
    this->setScissor(true);
    wm.add(this);
    setPosAndSize( 10, 10, 200, 200 );
    bFreePos = false;
    
    wm.add(this);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelZoom::setCentX(float f)
{
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelZoom::setCentY(float f)
{
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelZoom::setEchelle(float f)
{
    logf( (char*)"setEchelle(%0.2f)", f );
    echelle = f;
    float fDX = (float)pReadBgr->w / echelle;
    float fDY = (float)pReadBgr->h / echelle;
    
    int dxp = getParent()->getDX();
    int dyp = getParent()->getDY();

    if ( fDX < dxp )
    {
        echelle = (float)pReadBgr->w/(float)dxp;
        fDX = dxp;
        fDY = dyp;
        logf( (char*)"  Stop setEchelle(%0.2f)", echelle );
    }
    
    int deltax = (dxp-fDX)/2 + dx*echelle;
    int deltay = (dyp-fDY)/2 + dy*echelle;
    
    logf( (char*)"  setSize(%0.2f,%0.2f)", fDX, fDY );
    setSize( fDX, fDY );
    setPos( deltax, deltay );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelZoom::setRatio(float f)
{
    logf( (char*)"setRation(%0.2f)", f );

    ratio = f;
    dyFond = dxFond / ratio;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelZoom::setPosAndSize(int xx, int yy, int ddx, int ddy)
{

    xWin = xx;
    yWin = yy;
    dxWin = ddx;
    dyWin = ddy;
    #ifdef DEBUG    
    logf( (char*)"PanelZoom::setPosAndSize(%d, %d, %d, %d)", xx, yy, ddx, ddy);
    #endif
    if ( !bFreePos )
    {
        if ( bPanelResultat )    yWin += panelResultat->getDY()+10;
        PanelWindow::setPosAndSize(xWin, yWin, dxWin, dyWin);
        
    }
    else
    {
        //PanelWindow::setPosAndSize(xWin, yWin, dxWin, dyWin);
        if ( abs(xx-xPanel) > 5 || abs(yy-yPanel) > 5 )
        {
            
        }
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelZoom::setPos(int xx, int yy)
{
    xPanel = xWin = xx;
    xPanel = yWin = yy;
    
    //PanelWindow::setPos(xx, yy);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelZoom::setPosStar(float xx, float yy)
{
    //logf( (char*)"PanelZoom::setPosStar(%0.2f, %0.2f) ", xx, yy );
    vStar.x = xx;
    vStar.y = yy;
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelZoom::setRB(rb_t* p)
{
    pReadBgr = p;
    ratio = (float)pReadBgr->w / (float)pReadBgr->h;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelZoom::setBackground(_Texture2D* p)
{
    #ifdef DEBUG
    logf( (char*)"PanelZoom::setBackground(%lX)", (long)p );
    #endif
    pFond->setBackground(p);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelZoom::setBackground( GLubyte* ptr, unsigned int w, unsigned int h, unsigned int d )
{
    #ifdef DEBUG
    logf( (char*)"PanelZoom::setBackground(%lX, %d, %d, %d)", (long)ptr, w, h, d );
    #endif
    w = w;
    h = h;
    pFond->setBackground( ptr, w, h, d);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelZoom::update()
{
    if  ( pReadBgr==NULL )      logf( (char*)"PanelZoom::update()   pointeur RB NULL" );
    //else            logf((char*)"PanelZoom::update() w=%d", pReadBgr->w);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelZoom::glCroix( float xx, float yy, float l, vec4 c )
{
    float x = getPosX() + xx + (float)getDX()/2;
    float y = getPosY() + yy + (float)getDY()/2;


    float gris = 1.0;
    if ( bNuit )        glColor4f( gris,  0.0,  0.0, 1.0 );
    else                glColor4fv( c );    
    
	glBegin(GL_LINES);

	    //glVertex2i(xWin+0+xp, yWin+dyWin/2+yp);         glVertex2i(xWin+dxWin+xp, yWin+dyWin/2+yp);
	    //glVertex2i(xWin+dxWin/2+xp, yWin+0+yp);         glVertex2i(xWin+dxWin/2+xp, yWin+dyWin+yp);
	    glVertex2i( x-l/2, y);                glVertex2i( x+l/2, y);
	    glVertex2i( x, y-l/2);                glVertex2i( x, y+l/2);

    glEnd();        
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelZoom::displayGL()
{
	if ( !visible )			return;
	
    VarManager&     var = VarManager::getInstance();
    WindowsManager& wm  = WindowsManager::getInstance();

    float gris = 1.0;
    if ( bNuit )        setColor( 0xffff0000 );//glColor4f( gris,  0.0,  0.0, 1.0 );
    else                setColor( 0xffffffff );//glColor4f( gris,  0.0,  0.0, 1.0 );


    PanelSimple::displayGL();

	displayGLBordure();


	int scx, scy, scdx, scdy;
	scx  = getX();
	scy  = wm.getHeight() - getDY() - getY();
	scdx = getDX();
	scdy = getDY();
    if ( parent == NULL || bScissor )
    {
	    glScissor( scx, scy, scdx, scdy );
	    glEnable( GL_SCISSOR_TEST );
	}




    //if ( var.getb("bNuit") )        glColor4f( 0.5, 0.0, 0.0, 1.0 );    
    //else                            glColor4f( gris, gris, gris, 0.2 );

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

    glCroix( 0, 0, 100, vec4( 1.0, 0.2, 0.2, 1.0 ) );

    float x  = echelle * (xSuivi-vStar.x); 
    float y  = echelle * (ySuivi-vStar.y);
    glCroix( x, y, 100, vec4( 0.2, 0.2, 0.8, 1.0 ) );
    

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

    glDisable( GL_SCISSOR_TEST );

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelZoom::releaseLeft(int xm, int ym)
{
    logf( (char*)"PanelZoom::releaseLeft(xm=%d, ym=%d)", xm, ym );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelZoom::clickRight(int xm, int ym)
{
    logf( (char*)"PanelZoom::clickRight(xm=%d, ym=%d)", xm, ym );
    vClickRight.x = xm;
    vClickRight.y = ym;
    
    bFreePos = true;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelZoom::motionRight(int xm, int ym)
{
    logf( (char*)"PanelZoom::motionRight(xm=%d, ym=%d)", xm, ym );
    if ( abs(xm-vClickRight.x) < 20 )
    {
        if ( !bFreePos )        bFreePos = true;
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelZoom::releaseRight(int xm, int ym)
{
    logf( (char*)"PanelZoom::releaseRight(xm=%d, ym=%d)", xm, ym );
    logf( (char*)"  vStar(%0.2f, %0.2f)   mouse(%d, %d)", vStar.x, vStar.y, xm, ym );
    logf( (char*)"  vCamView(%0.2f, %0.2f) ech=%0.2f", vCamView.x, vCamView.y, ech );
    logf( (char*)"  paneZoom(%d, %d)", getPosX(), getPosY() );
    
    vec2 v = vCamView + vStar - vec2( getPosX(), getPosY() )/ech;
    
    if ( v.length() < 50 )
    {
        bFreePos = false;
        logf( (char*)"  Attache" );
    }
    else
    {
        bFreePos = true;
        logf( (char*)"  Free" );
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelZoom::setTextureWidth(int ww )
{
    w = ww;
    //logf( (char*)"PanelZoom::setTextWidth(%d)", w );

    echelle = 10.0;
    xFond  = -echelle * vStar.x + dxWin/2; 
    yFond  = -echelle * vStar.y + dyWin/2;
    dxFond = echelle * w;
    dyFond = echelle * h;

    pFond->setPosAndSize( xFond, yFond, dxFond, dyFond );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelZoom::setTextureHeight(int hh )
{
    h = hh;
    //logf( (char*)"PanelZoom::setTextHeight(%d)", h );

    echelle = 10.0;
    xFond  = -echelle * vStar.x + getDX()/2; 
    yFond  = -echelle * vStar.y + getDY()/2;
    dxFond = echelle * w;
    dyFond = echelle * h;

    pFond->setPosAndSize( xFond, yFond, dxFond, dyFond );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelZoom::setTextureSize(int ww, int hh )
{
//#define DEBUG
    #ifdef DEBUG
    logf( (char*)"PanelZoom::setTextureSize(%d, %d)", w, h );
    logf( (char*)"  echelle %d", echelle );
    logf( (char*)"  vStar(%0.2f, %0.2f)", vStar.x, vStar.y );
    logf( (char*)"  size(%d, %d)", getPosDX(), getPosDY() );
    #endif
    h = hh;
    w = ww;

    echelle = 10.0;
    xFond  = -echelle * vStar.x + getPosDX()/2; 
    yFond  = -echelle * vStar.y + getPosDY()/2;
    dxFond = echelle * w;
    dyFond = echelle * h;

    pFond->setPosAndSize( xFond, yFond, dxFond, dyFond );
//#undef DEBUG
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

