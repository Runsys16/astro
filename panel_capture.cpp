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
    
    setExtraString( "panelPreview" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelCapture::~PanelCapture()
{
    stars.setRB( NULL );
    stars.deleteAllStars();
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
    float fDX = (float)pReadBgr->w * ech_geo;
    float fDY = (float)pReadBgr->h * ech_geo;
    
    float fw = (float)pReadBgr->w;
    float fh = (float)pReadBgr->h;

    int dxp = getParent()->getDX();
    int dyp = getParent()->getDY();
    
    int deltax = ((fw/2.0 ) - ech_user * (fw/2.0 - dx )) * ech_geo;
    int deltay = ((fh/2.0 ) - ech_user * (fh/2.0 - dy )) * ech_geo;
    
    if ( deltax > 0 )           deltax = 0;
    if ( deltay > 0 )           deltay = 0;
    
    int maxX  = -(fDX*ech_user - dxp);
    int maxY  = -(fDY*ech_user - dyp);
    
    if ( deltax < maxX )        deltax = maxX;
    if ( deltay < maxY )        deltay = maxY;

    //setPos( deltax, deltay );
    Panel* p = getParent();
    if ( p!= NULL )
    {
        //logf( (char*)"delta : %d", p->getPosY() );
        stars.set_delta( deltax + p->getPosX(), deltay + p->getPosY() );
    }
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
        logf( (char*)"PanelCapture::setEchelle(%0.2f)", f );
        return;
    }
    if ( pReadBgr == NULL )         return;
    
    float fDX = (float)pReadBgr->w * ech_geo;
    float fDY = (float)pReadBgr->h * ech_geo;
    
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
    logf( (char*)"PanelCapture::setEchelle() ech_geo=%0.2f ech_user=%0.2f dx=%0.2f dy=%0.2f", ech_geo, ech_user, dx, dy );
    logf( (char*)"  setSize(%0.2f, %0.2f)", fDX, fDY );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::update()
{
    if  ( pReadBgr==NULL )      logf( (char*)"PanelCapture::update()   pointeur RB NULL" );

    //Panel* pParent = getParent();
    stars.update( getX(), getY(), this, pReadBgr );

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::updatePos()
{
    //PanelSimple::updatePos();
    if ( pReadBgr == NULL )
    {
        PanelSimple::updatePos();
        return;
    }

    float coef0 = (float)pReadBgr->w / getParent()->getDX();// * ech_user;
    float coef1 = (float)pReadBgr->h / getParent()->getDY();// * ech_user;
    float coef;
    if ( coef0 > coef1 )        coef = 1.0/coef1;
    else                        coef = 1.0/coef0;
    
    if ( coef != ech_geo )
    {
        logf( (char*)"PanelCapture::updatePos() Changement d'echelle" );
        logf( (char*)"  %s", pCapture!=NULL? (char*)pCapture->getBasename().c_str() : (char*)"" );
        logf( (char*)"  ech_geo=%0.2f ech_user=%0.2f", coef, ech_user );
        ech_geo = coef;
    }
    
    //setEchelle(coef);
    float fDX = (float)pReadBgr->w * ech_geo;
    float fDY = (float)pReadBgr->h * ech_geo;

    setSize( fDX*ech_user, fDY*ech_user );
    setCent();

    stars.update(getParent()->getX(), getParent()->getY(), this, NULL );

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
void PanelCapture::clip(int& xm, int& ym)	{
    Panel* p = getParent();

    float ech = ech_user * ech_geo;

    float max_x = ech*pReadBgr->w - p->getDX();
    float max_y = ech*pReadBgr->h - p->getDY();
    
    float X1 = xm;
    float Y1 = ym;
    
    if ( X1<0 )             X1 = 0.0;
    else
    if ( X1>max_x )         X1 = max_x;
    
    if ( Y1<0 )             Y1 = 0.0;
    else
    if ( Y1>max_y )         Y1 = max_y;
    
    xm = X1;
    ym = Y1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::clickLeft(int xm, int ym)
{
    logf( (char*)"PanelCapture::clickLeft(%d,%d) ...", xm, ym );
    
    Captures::getInstance().setCurrent( pCapture );
    Panel::clickLeft(xm,ym);
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


    // Si en plein ecran on ajoute une etoile
        
    if ( !bIcone     )
    {
        stars.setView( (PanelSimple*)this->getParent() );
        stars.setRB( pReadBgr );
        if ( stars.addStar( xm, ym, getX(), getY(), e ) == NULL )
            stars.selectLeft(xx, yy);
    }
    else
    {            
        captureOnTop(pCapture);
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::clickRight(int xm, int ym)
{
    Captures::getInstance().setCurrent( pCapture );
    PanelSimple::clickRight(xm,ym);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::releaseRight(int xm, int ym)
{
    logf( (char*)"PanelCapture::releaseRight(%d,%d) ...", xm, ym );
    printObjet();
    if ( pReadBgr == NULL )     { logf( (char*)"Pointeur NULL" ); return; }
    
    float e = (float)getDX() / (float)pReadBgr->w; 
    //float e = (float)getDX() / (float)1920.0; 
    
    int xx = ((float)xm-(float)getX()) / e;
    int yy = ((float)ym-(float)getY()) / e;
    
    stars.setView( (PanelSimple*)this->getParent() );
    stars.setRB( pReadBgr );
    if ( pCapture != NULL )
    {
        if ( pCapture->isFits() )       pCapture->afficheFits();
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::wheelUp(int xm, int ym)
{
    //logf( (char*)"PanelCapture::wheelUp(%d,%d) ... 2tex(%d,%d)", xm, ym, screen2texX(xm), screen2texY(ym) );
    //Captures::getInstance().glutSpecialFunc(104, xm, ym);
    float k = 1.1;
    ech_user *= k;
    
    Panel* p = getParent();

    float XM =  ( xm- p->getX() );
    float X0 = -( getPosX() ); 
    float X1 = k*X0 + (k-1)*XM;
    
    float YM =  ( ym-p->getY() );
    float Y0 = -( getPosY() ); 
    float Y1 = k*Y0 + (k-1)*YM;
    
    float ech = ech_user * ech_geo;
    float max_x = (ech*(float)pReadBgr->w) - p->getDX();
    float max_y = (ech*(float)pReadBgr->h) - p->getDY();

    setPos( -X1, -Y1 );
    
    //logf( (char*)"  XY    (%0.2f, %0.2f)  (%0.2f, %0.2f)", X0, Y0, X1, Y1 );
    //logf( (char*)"  Mouse (%0.2f, %0.2f)  (%0.2f, %0.2f)", (float)xm, (float)ym, XM, YM );
    //logf( (char*)"  MAX   (%0.2f, %0.2f)", max_x, max_y );
    //logf( (char*)"  ech   (%0.2f, %0.2f) bgr(%d, %d)", ech_geo, ech_user, pReadBgr->w, pReadBgr->h );
    //logf( (char*)"  win   (%0.2f, %0.2f)", (float)p->getDX(), (float)p->getDY() );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::wheelDown(int xm, int ym)
{
    //logf( (char*)"PanelCapture::wheelDown(%d,%d) ... 2tex(%d,%d)", xm, ym, screen2texX(xm), screen2texY(ym) );
    //Captures::getInstance().glutSpecialFunc(105, xm, ym);
    float k = 0.9;
    ech_user *= k;
    if (ech_user<=1.0)           ech_user = 1.0;
    
    Panel* p = getParent();

    float XM =  ( xm- p->getX() );
    float X0 = -( getPosX() ); 
    float X1 = k*X0 + (k-1)*XM;
    
    float YM =  ( ym-p->getY() );
    float Y0 = -( getPosY() ); 
    float Y1 = k*Y0 + (k-1)*YM;
    /*    
    float ech = ech_user * ech_geo;
    float max_x = ech*pReadBgr->w - p->getDX();
    float max_y = ech*pReadBgr->h - p->getDY();
    
    if ( X1<0 )             X1 = 0.0;
    else
    if ( X1>max_x )         X1 = max_x;
    
    if ( Y1<0 )             Y1 = 0.0;
    else
    if ( Y1>max_y )         Y1 = max_y;
    */
    int x1 = X1;
    int y1 = Y1;
    
    clip( x1, y1 );
    setPos( -x1, -y1 );
    
    //logf( (char*)"  XY    (%0.2f, %0.2f)  (%0.2f, %0.2f)", X0, Y0, X1, Y1 );
    //logf( (char*)"  Mouse (%0.2f, %0.2f)  (%0.2f, %0.2f)", (float)xm, (float)ym, XM, YM );
    //logf( (char*)"  MAX   (%0.2f, %0.2f)", max_x, max_y );
    //logf( (char*)"  ech   (%0.2f, %0.2f) bgr(%d, %d)", ech_geo, ech_user, pReadBgr->w, pReadBgr->h );
    //logf( (char*)"  win   (%0.2f, %0.2f)", (float)p->getDX(), (float)p->getDY() );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::clickMiddle(int xm, int ym)
{
    xm_old = xm;
    ym_old = ym;
    logf( (char*)"PanelCapture::clickMiddle(%d,%d) ...", xm, ym );
    Captures::getInstance().setCurrent( pCapture );

    bHaveMove = false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::motionMiddle(int xm, int ym)
{
    //logf( (char*)"PanelCapture::motionMiddle(%d,%d) ...", xm, ym );
    //logf( (char*)"    delta (%d,%d) ...", xm-xm_old, ym-ym_old );
    float deltaX, deltaY;
    float ech = ech_geo * ech_user;
    Panel* p = getParent();
    
    deltaX = (float) (xm-xm_old) / ech;
    deltaY = (float) (ym-ym_old) / ech;

    float X0 = ( getPosX() ); 
    float X1 = X0 + (xm-xm_old);
    
    float Y0 = ( getPosY() ); 
    float Y1 = Y0 + (ym-ym_old);
    
    float max_x = ech*pReadBgr->w - p->getDX();
    float max_y = ech*pReadBgr->h - p->getDY();

    int x1 = -X1;
    int y1 = -Y1;

    clip(x1, y1);
    setPos( -x1, -y1 );
    
    //logf( (char*)"  XY    (%0.2f, %0.2f)  (%0.2f, %0.2f)", X0, Y0, X1, Y1 );
    //logf( (char*)"  Mouse (%0.2f, %0.2f) ", (float)xm, (float)ym );
    //logf( (char*)"  MAX   (%0.2f, %0.2f)", max_x, max_y );
    //logf( (char*)"  ech   (%0.2f, %0.2f) bgr(%d, %d)", ech_geo, ech_user, pReadBgr->w, pReadBgr->h );
    //logf( (char*)"  win   (%0.2f, %0.2f)", (float)p->getDX(), (float)p->getDY() );
    //stars.set_delta( deltaX, deltaY );
    
    xm_old = xm;
    ym_old = ym;
    
    bHaveMove = true;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::releaseMiddle(int xm, int ym)
{
    xm_old = -1;
    ym_old = -1;
    logf( (char*)"PanelCapture::releaseMiddle(%d,%d) ...", xm, ym );
    log_tab(true);
    logf( (char*)"ech_geo=%0.2f ech_user=%0.2f dx=%0.2f dy=%0.2f", ech_geo, ech_user, dx, dy );

    if ( pReadBgr == NULL )     { logf( (char*)"Pointeur NULL" ); log_tab(false); return; }
    
    
    //float e = (float)getDX() / (float)pReadBgr->w; 
    float e = (float)getDX() / (float)pReadBgr->w; 
    logf( (char*)"echelle%0.2f", e );
    
    int xx = ((float)xm-(float)getX()) / e;
    int yy = ((float)ym-(float)getY()) / e;
    
    if ( !bHaveMove )
    {
        stars.setView( this );
        stars.setRB( pReadBgr );
        if ( stars.addStar( xm, ym, getX(), getY(), e ) == NULL )
        {
            stars.selectStar(xx, yy);
            logf( (char*)"releaseMiddle(%d,%d) selects star...", xm, ym );
        }
        else
            stars.selectStar(xx, yy);
    }
    bHaveMove = false;
    log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int PanelCapture::screen2texX( int x )
{
    return (float)x / ( ech_geo*ech_user) + dx;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int PanelCapture::screen2texY( int y )
{
    return (float)y / (ech_geo*ech_user) + dy;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::screen2tex(int& x, int& y)
{
    x = screen2texX(x);
    y = screen2texY(y);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int PanelCapture::tex2screenX( int x )
{
    return (float)( x-dx) * ech_user * ech_geo;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int PanelCapture::tex2screenY( int y )
{
    return (float)( y-dy) * ech_user * ech_geo;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::tex2screen(int& x, int& y)
{
    x = tex2screenX(x);
    y = tex2screenY(y);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::printObjet()
{
    logf( (char*)"   this->dx=%d", dx );
    logf( (char*)"   this->dy=%d", dy );
    logf( (char*)"   this->ech_geo=%0.4f",  ech_geo );
    logf( (char*)"   this->ech_user=%0.4f", ech_user );
    if ( pReadBgr == NULL )    {
        logf( (char*)"   pReadBgr = NULL" );
    } else {
        logf( (char*)"  this->pReadBgr->w %d", pReadBgr->w );
        logf( (char*)"  this->pReadBgr->h %d", pReadBgr->h );
        logf( (char*)"  this->pReadBgr->d %d", pReadBgr->d );
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

