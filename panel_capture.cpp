#include "panel_capture.h"

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelCapture::PanelCapture( struct readBackground*  pReadBgr, Capture* pc )
{
    ech_geo     = 1.0;
    ech_user    = 1.0;
    dx          = 0.0;
    dy          = 0.0;
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

    int dxp = getParent()->getDX();
    int dyp = getParent()->getDY();
    
    int deltax = (dxp-fDX)/2 + dx*ech_geo*ech_user;
    int deltay = (dyp-fDY)/2 + dy*ech_geo*ech_user;
    
    setPos( deltax, deltay );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::setCentX(float f)
{
    dx = f;

    float fDX = (float)pReadBgr->w / ech_geo;
    float fDY = (float)pReadBgr->h / ech_geo;

    int dxp = getParent()->getDX();
    int dyp = getParent()->getDY();
    
    int deltax = (dxp-fDX)/2 + dx*ech_geo*ech_user;
    int deltay = (dyp-fDY)/2 + dy*ech_geo*ech_user;
    
    setPos( deltax, deltay );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::setCentY(float f)
{
    dy = f;

    float fDX = (float)pReadBgr->w / ech_geo;
    float fDY = (float)pReadBgr->h / ech_geo;

    int dxp = getParent()->getDX();
    int dyp = getParent()->getDY();
    
    int deltax = (dxp-fDX)/2 + dx*ech_geo;
    int deltay = (dyp-fDY)/2 + dy*ech_geo;
    
    setPos( deltax, deltay );
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
    
    //ech_geo  = f;
    ech_user = f;
    
    float ech;
    ech = ech_geo * ech_user;
    
    float fDX = (float)pReadBgr->w * ech;
    float fDY = (float)pReadBgr->h * ech;
    
    int dxp = getParent()->getDX();
    int dyp = getParent()->getDY();

    if ( fDX < dxp )
    {
        ech_geo = (float)pReadBgr->w/(float)dxp;
        ech = ech_geo * ech_user;
        fDX = (float)pReadBgr->w / ech;
        fDY = (float)pReadBgr->h / ech;
        logf( (char*)"Stop setEchelle(%0.2f)", ech );
    }
    
    //int deltax = (dxp-fDX)/2 + dx*ech_geo;
    //int deltay = (dyp-fDY)/2 + dy*ech_geo;
    
    int deltax = (dxp-fDX)/2 + dx*ech;
    int deltay = (dyp-fDY)/2 + dy*ech;
    
    setSize( fDX, fDY );
    //logf( (char*)"setSize(%0.2f, %0.2f)", fDX, fDY );
    setPos( deltax, deltay );
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




