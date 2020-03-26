#include "panel_camera.h"

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelCamera::PanelCamera()
{
    echelle = 1.0;
    dx      = 0.0;
    dy      = 0.0;
    pReadBgr = NULL;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::findAllStar()
{
    stars.setView( this );
    stars.setRB( pReadBgr );
    stars.findAllStars();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool PanelCamera::starExist(int x, int y)
{
    return stars.starExist(x,y);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::addStar(int x, int y)
{
    stars.setView( this );
    stars.setRB( pReadBgr );
    //stars.addStar(x, y);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::setCentX(float f)
{
    dx = f;

    float fDX = (float)pReadBgr->w / echelle;
    float fDY = (float)pReadBgr->h / echelle;

    int dxp = getParent()->getDX();
    int dyp = getParent()->getDY();
    
    int deltax = (dxp-fDX)/2 + dx*echelle;
    int deltay = (dyp-fDY)/2 + dy*echelle;
    
    setPos( deltax, deltay );
    updatePos();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::setCentY(float f)
{
    dy = f;

    float fDX = (float)pReadBgr->w / echelle;
    float fDY = (float)pReadBgr->h / echelle;

    int dxp = getParent()->getDX();
    int dyp = getParent()->getDY();
    
    int deltax = (dxp-fDX)/2 + dx*echelle;
    int deltay = (dyp-fDY)/2 + dy*echelle;
    
    setPos( deltax, deltay );
    updatePos();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::setEchelle(float f)
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
    updatePos();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::setRB(rb_t* p)
{
    pReadBgr = p;
    stars.setRB(p);
    //logf((char*)"PanelCamera::setRB() w=%d", pReadBgr->w);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
vec2* PanelCamera::getSuivi()
{
    return stars.getSuivi();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::update()
{
    if  ( pReadBgr==NULL )      logf( (char*)"PanelCamera::update()   pointeur RB NULL" );
    //else            logf((char*)"PanelCamera::update() w=%d", pReadBgr->w);
    
    stars.update( getX(), getY(), this, pReadBgr );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::tex2screen(float& xx, float& yy)
{
    if  ( panelCourbe==NULL )   return;
    if  ( pReadBgr==NULL )      return;

	WindowsManager& wm = WindowsManager::getInstance();
    
    //      Dimension ecran
    float wSc = (float)wm.getWidth();
    float hSc = (float)wm.getHeight();

    //      Dimension texture
    float wTex = (float)pReadBgr->w;
    float hTex = (float)pReadBgr->h;

    //      Facteur d'echelle
    float ew = wSc / wTex;
    float eh = hSc / hTex;
    float e = ew<eh ? ew : eh;

    xx = e * xx + (float)getX();    
    yy = e * yy + (float)getY();    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::tex2screen(int& xx, int& yy)
{
    if  ( panelCourbe==NULL )   return;
    if  ( pReadBgr==NULL )      return;

    float XX = xx;
    float YY = yy;
    
    tex2screen(XX, YY);

    xx = (int)XX;
    yy = (int)YY;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::screen2tex(float& xx, float& yy)
{
    if  ( panelCourbe==NULL )   return;
    if  ( pReadBgr==NULL )      return;

	WindowsManager& wm = WindowsManager::getInstance();
    
    //      Dimension ecran
    float wSc = (float)wm.getWidth();
    float hSc = (float)wm.getHeight();

    //      Dimension texture
    float wTex = (float)pReadBgr->w;
    float hTex = (float)pReadBgr->h;

    //      Facteur d'echelle
    float ew = wSc / wTex;
    float eh = hSc / hTex;
    float e = ew<eh ? ew : eh;

    xx = (xx - (float)getX()) /e;    
    yy = (yy - (float)getY()) / e;    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::screen2tex(int& xx, int& yy)
{
    if  ( panelCourbe==NULL )   return;
    if  ( pReadBgr==NULL )      return;

    float XX = xx;
    float YY = yy;
    
    screen2tex(XX, YY);

    xx = (int)XX;
    yy = (int)YY;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::glCercle(int x, int y, int rayon)
{
	glBegin(GL_LINE_LOOP);

        for( float i=0; i<=360.0; i+=1.0 )
        {
            float fx = (float)x+ (float)rayon*cos(DEG2RAD(i));
            float fy = (float)y+ (float)rayon*sin(DEG2RAD(i));
            glVertex2i(fx,fy);
        }
        
    glEnd();        
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::glCarre( int x,  int y,  int dx,  int dy )
{
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
void PanelCamera::glCroix( int x,  int y,  int dx,  int dy )
{
	glBegin(GL_LINES);

	    glVertex2i(x, y-dy);         glVertex2i(x, y+dy);
	    glVertex2i(x-dx, y);         glVertex2i(x+dx, y);

    glEnd();        
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::displaySuivi()
{
    if  ( panelCourbe==NULL )   return;
    if  ( pReadBgr==NULL )      return;

	WindowsManager& wm = WindowsManager::getInstance();
    /*
    //      Dimension ecran
    float wSc = (float)wm.getWidth();
    float hSc = (float)wm.getHeight();

    //      Dimension texture
    float wTex = (float)pReadBgr->w;
    float hTex = (float)pReadBgr->h;

    //      Facteur d'echelle
    float ew = wSc / wTex;
    float eh = hSc / hTex;
    float e = ew<eh ? ew : eh;
    
    //      Coordonnées du point de suivi
    */
    float x = xSuivi;
    float y = ySuivi;
    
    //      Convertion des coordonnées
    /*
    x = e * x + getX();
    y = e * y + getY();
    */
    
    tex2screen( x, y );
    float gris = 0.8;

    //      DEBUG
    //logf( (char*)"Screen (%0.2f,%0.2f)  texture(%0.2f,%0.2f)", wSc, hSc, wTex, hTex );
    //logf( (char*)"     suivi (%0.2f,%0.2f) -> (%0.2f,%0.2f)", xSuivi, ySuivi, x, y );

    //      Affichage en mode nuit ?
    if ( bNuit )        glColor4f( 1.0,   0.0,  0.0, gris );
    else                glColor4f( 0.5,   0.4,  0.5, gris );    
    
    //      Affichage de la croix
	glBegin(GL_LINES);
        
        glVertex2i(x-50,y);             glVertex2i(x+50, y );
        glVertex2i(x,y-50);             glVertex2i(x, y+50 );

    glEnd();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::displayCentre()
{
    if  ( pReadBgr==NULL )  return;

	WindowsManager& wm = WindowsManager::getInstance();

    int x = wm.getWidth();
    int y = wm.getHeight();

    float gris = 0.3;
    if ( bNuit )        glColor4f( 1.0,   0.0,  0.0, gris );
    else                glColor4f( 0.0,   1.0,  0.0, gris );    
    
	glBegin(GL_LINES);
        
        glVertex2i(x/2,0);              glVertex2i(x/2, y );
        glVertex2i(0,y/2);              glVertex2i(x, y/2 );

    glEnd();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::displayGL()
{
    //logf( (char*)"*** PanelCamera::displayGL() ***" );
    //mat4 m = scale( 2.0, 2.0, 1.0 );
    float gris = 0.3;
    VarManager& var = VarManager::getInstance();
    //if ( var.getb("bNuit") )        glColor4f( 0.5, 0.0, 0.0, 1.0 );    
    //else                            glColor4f( gris, gris, gris, 0.2 );

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

    //float x = getParent()->getX();
    //float y = getParent()->getY();

    float x = getX();
    float y = getY();

    float X = -dx;
    float Y = -dy;

    float dx = getDX()/2;
    float dy = getDY()/2;


    if ( bNuit )        glColor4f( gris,  0.0,  0.0, 1.0 );
    else                glColor4f( gris, gris, gris, 0.2 );    
    
    PanelWindow::displayGL();



    if ( bNuit )        glColor4f( gris,  0.0,  0.0, 1.0 );
    else                glColor4f( 0.0,   1.0,  0.0, 0.4 );    


    stars.displayGL();
    
    if ( bAffCentre )           displayCentre();
    if ( bAffSuivi )            displaySuivi();

    //*
    if ( bModeManuel )
    {
        if ( var.getb("bNuit") )        glColor4f( 1.0, 0.0, 0.0, 0.2 );
	    else                            glColor4f( 0.0, 1.0, 0.0, 0.2 );
	    
	    int x = xClick;
	    int y = yClick;

	    //logf( (char*)"--(%d,%d)", x, y );
	    tex2screen(x,y);
	    //logf( (char*)"  (%d,%d)", x, y );

	    glCroix(x, y, 50, 50);
        glCercle(x, y, 25);

    }

    if ( bAfficheVec)
    {
        glVecAD();
        glVecDC();
    }

    displayGLTrace();
    //*/
    
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::releaseLeft(int xm, int ym)
{
    logf( (char*)"PanelCamera::releaseLeft(%d,%d) ...", xm, ym );
    if ( pReadBgr == NULL )     { logf( (char*)" return Pointeur NULL" ); return; }
    
    log_tab(true);
    logf( (char*)"getDX=%d RB->w=%0.2f", getDX(), pReadBgr->w );
    
    //float e = (float)getDX() / (float)pReadBgr->w; 
    float e = (float)getDX() / (float)1920.0; 
    
    int xx = ((float)xm-(float)getX()) / e;
    int yy = ((float)ym-(float)getY()) / e;
    
    stars.setView( this );
    stars.setRB( pReadBgr );
    
    if ( !bModeManuel && stars.addStar( xm, ym, getX(), getY(), e ) == NULL )
        stars.selectLeft(xx, yy);

    log_tab(false);
    logf( (char*)"PanelCamera::releaseLeft(%d,%d) ...", xm, ym );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::releaseMiddle(int xm, int ym)
{
    if ( bMouseDeplace ) 
    {
        logf( (char*)"PanelCamera::releaseMiddle(%d,%d) ... deplacement MOUSE", xm, ym );
        return;
    }

    logf( (char*)"PanelCamera::releaseMiddle(%d,%d) ...", xm, ym );
    if ( pReadBgr == NULL )     { logf( (char*)"Pointeur NULL" ); return; }
    log_tab(true);
        
    logf( (char*)"panelCamera->getDX()=%d pReadBgr->w=%0.2f", getDX(), pReadBgr->w );
    
    //float e = (float)getDX() / (float)pReadBgr->w; 
    float e = (float)getDX() / (float)1920.0; 
    
    int xx = ((float)xm-(float)getX()) / e;
    int yy = ((float)ym-(float)getY()) / e;
    
    stars.setView( this );
    stars.setRB( pReadBgr );
    if ( stars.addStar( xm, ym, getX(), getY(), e ) == NULL )
    {
        stars.selectMiddle(xx, yy);
        logf( (char*)"releaseMiddle(%d,%d) selects star...", xm, ym );
    }
    else
        stars.selectMiddle(xx, yy);
    
    log_tab(false);
    logf( (char*)"PanelCamera::releaseMiddle(%d,%d) ...", xm, ym );
}




