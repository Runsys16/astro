#include "panel_capture.h"
#include "captures.h"

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelCapture::PanelCapture( struct readBackground*  pReadBgr, Capture* pc )
{
    ech_geo     = 1.0;
    ech_user    = 1.0;
    dx          = 0.0;
    dy          = 0.0;
    xm_old      = -1;
    ym_old      = -1;
    
    pReadBgr    = pReadBgr;
    pCapture    = pc;
    bIcone      = false;
    stars.setView( this );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::deleteAllStars()
{
    stars.setRB( pReadBgr );

    stars.deleteAllStars();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::findAllStars()
{
    stars.setView( this );
    stars.setRB( pReadBgr );
    stars.findAllStars();
    return;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::setCent()
{
    float fDX = (float)pReadBgr->w / ech_geo;
    float fDY = (float)pReadBgr->h / ech_geo;
    
    float fw = (float)pReadBgr->w;
    float fh = (float)pReadBgr->h;

    int dxp = getParent()->getDX();
    int dyp = getParent()->getDY();
    
    int deltax = ((fw/2.0 ) - ech_user * (fw/2.0 - dx )) / ech_geo;
    int deltay = ((fh/2.0 ) - ech_user * (fh/2.0 - dy )) / ech_geo;
    
    if ( deltax > 0 )           deltax = 0;
    if ( deltay > 0 )           deltay = 0;
    
    int maxX  = -(fDX*ech_user - dxp);
    int maxY  = -(fDY*ech_user - dyp);
    
    if ( deltax < maxX )        deltax = maxX;
    if ( deltay < maxY )        deltay = maxY;

    setPos( deltax, deltay );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::setCentX(float f)
{
    dx = f;
    setCent();
    return;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::setCentY(float f)
{
    dy = f;
    setCent();
    return;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::setEchelle(float f)
{
    if ( f == 0.0 )     
    {
        logf( (char*)"setEchelle(%0.2f)", f );
        return;
    }
    if ( pReadBgr == NULL )         return;
    
    float fDX = (float)pReadBgr->w / ech_geo;
    float fDY = (float)pReadBgr->h / ech_geo;
    
    int dxp = getParent()->getDX();
    int dyp = getParent()->getDY();

    if ( fDX*f < dxp || fDY*f < dyp )
    {
        logf( (char*)"Stop setEchelle(%0.2f)", ech_user );
        return;
    }

    ech_user = f;
    
    
    setSize( fDX*ech_user, fDY*ech_user );
    setCent();
    logf( (char*)"setSize(%0.2f, %0.2f)", fDX, fDY );
    logf( (char*)"  PanelCapture::setEchelle() ech_geo=%0.2f ech_user=%0.2f dx=%0.2f dy=%0.2f", ech_geo, ech_user, dx, dy );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::update()
{
    if  ( pReadBgr==NULL )      logf( (char*)"PanelCamera::update()   pointeur RB NULL" );

    //Panel* pParent = getParent();
    stars.update( getX(), getY(), this, pReadBgr );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::updatePos()
{
    //PanelSimple::updatePos();

    float coef0 = (float)pReadBgr->w / getParent()->getDX();// * ech_user;
    float coef1 = (float)pReadBgr->h / getParent()->getDY();// * ech_user;
    float coef;
    if ( coef0 > coef1 )        coef = coef1;
    else                        coef = coef0;
    
    if ( coef != ech_geo )
    {
        logf( (char*)"Changement d'echelle ech_geo=%0.2f ech_user=%0.2f", coef, ech_user );
        ech_geo = coef;
    }
    
    //setEchelle(coef);
    float fDX = (float)pReadBgr->w / ech_geo;
    float fDY = (float)pReadBgr->h / ech_geo;

    setSize( fDX*ech_user, fDY*ech_user );
    setCent();
    
    PanelSimple::updatePos();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::displayGL()
{
    float gris = 0.3;
    VarManager& var = VarManager::getInstance();

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

    /*
    float x = getX();
    float y = getY();

    float X = -dx;
    float Y = -dy;

    float dx = getDX()/2;
    float dy = getDY()/2;
    */

    if ( bNuit )        glColor4f( gris,  0.0,  0.0, 1.0 );
    else                glColor4f( gris, gris, gris, 0.2 );    
    
    PanelSimple::displayGL();

    stars.displayGL();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::addStar(int xm, int ym)
{
    logf( (char*)"PanelCapture::addStar(%d,%d) ...", xm, ym );
    stars.setView( this );
    stars.setRB( pReadBgr );
    //stars.addStar( xm, ym );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::releaseLeft(int xm, int ym)
{
    logf( (char*)"PanelCapture::releaseLeft(%d,%d) ...", xm, ym );

    float e = (float)getDX() / (float)pReadBgr->w; 
    
    int xx = ((float)xm-(float)getX()) / e;
    int yy = ((float)ym-(float)getY()) / e;


    // Si en plei ecran on ajoute une etoile
        
    if ( !bIcone     )
    {
        stars.setView( this->getParent() );
        stars.setRB( pReadBgr );
        if ( stars.addStar( xm, ym, getX(), getY(), e ) == NULL )
            stars.selectLeft(xx, yy);
    }
    else
    {            
        captureOnTop(pCapture);
    }
    //else

        
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::releaseRight(int xm, int ym)
{
    logf( (char*)"PanelCamera::releaseRight(%d,%d) ...", xm, ym );
    if ( pReadBgr == NULL )     { logf( (char*)"Pointeur NULL" ); return; }
    logf( (char*)"   getDX=%d RB->w=%0.2f", getDX(), pReadBgr->w );
    
    //float e = (float)getDX() / (float)pReadBgr->w; 
    float e = (float)getDX() / (float)1920.0; 
    
    int xx = ((float)xm-(float)getX()) / e;
    int yy = ((float)ym-(float)getY()) / e;
    
    stars.setView( this->getParent() );
    stars.setRB( pReadBgr );
    if ( stars.addStar( xm, ym, getX(), getY(), e ) == NULL )
        stars.selectRight(xx, yy);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::wheelUp(int xm, int ym)
{
    logf( (char*)"PanelCamera::wheelUp(%d,%d) ...", xm, ym );
    Captures::getInstance().glutSpecialFunc(104, xm, ym);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::wheelDown(int xm, int ym)
{
    logf( (char*)"PanelCamera::wheelDown(%d,%d) ...", xm, ym );
    Captures::getInstance().glutSpecialFunc(105, xm, ym);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::clickMiddle(int xm, int ym)
{
    xm_old = xm;
    ym_old = ym;
    logf( (char*)"PanelCamera::clickMiddle(%d,%d) ...", xm, ym );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::motionMiddle(int xm, int ym)
{
    //logf( (char*)"PanelCamera::motionMiddle(%d,%d) ...", xm, ym );
    //logf( (char*)"    delta (%d,%d) ...", xm-xm_old, ym-ym_old );
    float deltaX, deltaY;
    
    deltaX = (float) (xm-xm_old) * ech_geo / ech_user;
    deltaY = (float) (ym-ym_old) * ech_geo / ech_user;

    setCentX( dx + deltaX );
    setCentY( dy + deltaY );
    
    xm_old = xm;
    ym_old = ym;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::releaseMiddle(int xm, int ym)
{
    xm_old = -1;
    ym_old = -1;
    logf( (char*)"PanelCamera::releaseMiddle(%d,%d) ...", xm, ym );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::screen2tex(int& x, int& y)
{
    x = (float)x * ech_geo / ech_user + dx;
    y = (float)y * ech_geo / ech_user + dy;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::tex2screen(int& x, int& y)
{
    x = (float)( x-dx) * ech_user / ech_geo;
    y = (float)( y-dy) * ech_user / ech_geo;
}






