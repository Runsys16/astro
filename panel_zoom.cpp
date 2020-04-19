#include "panel_zoom.h"

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelZoom::~PanelZoom()
{
    WindowsManager& wm = WindowsManager::getInstance();

    logf( (char*)"Destructeur PanelZoom()");
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
    setBackground( (_Texture2D*)NULL );
    
    this->setScissor(true);
    wm.add(this);
    setPosAndSize( 10, 10, 200, 200 );
    bFreePos = false;
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
        logf( (char*)"Stop setEchelle(%0.2f)", echelle );
    }
    
    int deltax = (dxp-fDX)/2 + dx*echelle;
    int deltay = (dyp-fDY)/2 + dy*echelle;
    
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
    
    //logf( (char*)"PanelZoom::setPosAndSize(%d, %d, %d, %d)", xx, y, ddx, ddy);
    if ( !bFreePos )
    {
        PanelWindow::setPosAndSize(xWin, yWin, dxWin, dyWin);
        
    }
    else
    {
        //PanelWindow::setPosAndSize(xWin, yWin, dxWin, dyWin);
        if ( abs(xx-xPanel) > 5 || abs(yy-yPanel) > 5 )
        {
            
        }
    }
    /*
    if ( pReadBgr != NULL)
    {
        echelle = 10.0;
        xFond  = -echelle * vStar.x + dxWin/2; 
        yFond  = -echelle * vStar.y + dyWin/2;
        dxFond = echelle * pReadBgr->w;
        dyFond = echelle * pReadBgr->h;

        pFond->setPosAndSize( xFond, yFond, dxFond, dyFond );
        //logf( (char*)"pFond->SetPosAndSizep(%0.2f, %0.2f, %0.2f, %0.2f)", xFond, yFond, dxFond, dyFond );
        pFond->deleteBackground();
        pFond->setBackground( pReadBgr->ptr, pReadBgr->w, pReadBgr->h, pReadBgr->d);
        ajuste();
    }
    else
    {
        logf( (char*)"PanelZoom::setPosAndSize(%d, %d, %d, %d) Pointeur NULL", xx, y, ddx, ddy);
    }
    */    
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
    //logf( (char*)"PanelZoom::setBackground()" );
    pFond->setBackground(p);
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
void PanelZoom::glCroix( float xx, float yy, float l )
{
    float x = getX();
    float y = getY();

    float X = -dx;
    float Y = -dy;

    float dx = xx;
    float dy = yy;


    float gris = 1.0;
    if ( bNuit )        glColor4f( gris,  0.0,  0.0, 1.0 );
    else                glColor4f( gris, gris, gris, 1.0 );    
    

    if ( bNuit )        glColor4f( gris,  0.0,  0.0, 1.0 );
    else                glColor4f( 0.0,   1.0,  0.0, 0.4 );    

    glColor4f( 0.0, 1.0, 1.0, 1.0);

    Panel* p = getParent();
    int xp = 0;//p->getX();
    int yp = 0;//p->getY();

	glBegin(GL_LINES);

	    glVertex2i(xWin+0+xp, yWin+dyWin/2+yp);         glVertex2i(xWin+dxWin+xp, yWin+dyWin/2+yp);
	    glVertex2i(xWin+dxWin/2+xp, yWin+0+yp);         glVertex2i(xWin+dxWin/2+xp, yWin+dyWin+yp);

    glEnd();        
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelZoom::displayGL()
{
	if ( !visible )			return;
	
    float gris = 1.0;
    if ( bNuit )        glColor4f( gris,  0.0,  0.0, 1.0 );
    else                glColor4f( 0.0,   1.0,  0.0, 0.4 );    

    PanelSimple::displayGL();
	displayGLBordure();

    VarManager& var = VarManager::getInstance();
    //if ( var.getb("bNuit") )        glColor4f( 0.5, 0.0, 0.0, 1.0 );    
    //else                            glColor4f( gris, gris, gris, 0.2 );

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

    glCroix( (float)getDX()/2, (float)getDY()/2, 100 );
    glCroix( (float)xSuivi-(float)getDX()/2, (float)ySuivi - (float)getDY()/2, 100 );
    

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();


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
    logf( (char*)"  paneZoom(%d, %d)", getX(), getY() );
    
    vec2 v = vCamView + vStar - vec2( getX(), getY() );
    
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
void PanelZoom::setTextWidth(int ww )
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
void PanelZoom::setTextHeight(int hh )
{
    h = hh;
    //logf( (char*)"PanelZoom::setTextHeight(%d)", h );

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

