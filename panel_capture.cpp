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
    if ( coef0 > coef1 )        coef = coef1;
    else                        coef = coef0;
    
    if ( coef != ech_geo )
    {
        logf( (char*)"PanelCapture::updatePos() Changement d'echelle" );
        logf( (char*)"  %s", pCapture!=NULL? (char*)pCapture->getBasename().c_str() : (char*)"" );
        logf( (char*)"  ech_geo=%0.2f ech_user=%0.2f", coef, ech_user );
        ech_geo = coef;
    }
    
    //setEchelle(coef);
    float fDX = (float)pReadBgr->w / ech_geo;
    float fDY = (float)pReadBgr->h / ech_geo;

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
Panel* PanelCapture::isMouseOver(int xm, int ym)	{
	#ifdef DEBUG
	cout << "Panel::isMouseOver()" << x_raw <<", "<< y_raw <<", "<< dx_raw <<", "<< dy_raw << endl;
	#endif

	if ( !visible )			return NULL;
	if ( bFantome )			return NULL;

    int xx  = getParent()->getX();
    int yy  = getParent()->getY();
    int dxx = getParent()->getDX();
    int dyy = getParent()->getDY();


    /*
    int nb = childs.size();
	for( int i=0; i<nb; i++ )	{
		Panel * p = childs[i]->isMouseOver(xm, ym);
		if ( p )		return p;
	}
    */
	if ( xx <= xm && xm <= (xx+dxx) && yy <= ym && ym <= (yy+dyy) )		                    return this;
	else																					return NULL;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::clickLeft(int xm, int ym)
{
    Captures::getInstance().setCurrent( pCapture );
    PanelSimple::clickLeft(xm,ym);
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
    
    //float e = (float)getDX() / (float)pReadBgr->w; 
    float e = (float)getDX() / (float)1920.0; 
    
    int xx = ((float)xm-(float)getX()) / e;
    int yy = ((float)ym-(float)getY()) / e;
    
    stars.setView( this->getParent() );
    stars.setRB( pReadBgr );
    if ( pCapture != NULL )
    {
        if ( pCapture->isFits() )       pCapture->afficheFits();
    }
    /*
    if ( stars.addStar( xm, ym, getX(), getY(), e ) == NULL )
        stars.selectRight(xx, yy);
    */
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::wheelUp(int xm, int ym)
{
    logf( (char*)"PanelCapture::wheelUp(%d,%d) ... 2tex(%d,%d)", xm, ym, screen2texX(xm), screen2texY(ym) );
    Captures::getInstance().glutSpecialFunc(104, xm, ym);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::wheelDown(int xm, int ym)
{
    logf( (char*)"PanelCapture::wheelDown(%d,%d) ... 2tex(%d,%d)", xm, ym, screen2texX(xm), screen2texY(ym) );
    Captures::getInstance().glutSpecialFunc(105, xm, ym);
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
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::motionMiddle(int xm, int ym)
{
    //logf( (char*)"PanelCapture::motionMiddle(%d,%d) ...", xm, ym );
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
    logf( (char*)"PanelCapture::releaseMiddle(%d,%d) ...", xm, ym );
    logf( (char*)"  ech_geo=%0.2f ech_user=%0.2f dx=%0.2f dy=%0.2f", ech_geo, ech_user, dx, dy );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int PanelCapture::screen2texX( int x )
{
    return (float)x * ech_geo / ech_user + dx;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int PanelCapture::screen2texY( int y )
{
    return (float)y * ech_geo / ech_user + dy;
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
    return (float)( x-dx) * ech_user / ech_geo;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int PanelCapture::tex2screenY( int y )
{
    return (float)( y-dy) * ech_user / ech_geo;
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

