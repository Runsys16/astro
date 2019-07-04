#include "panel_capture.h"

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelCapture::PanelCapture( struct readBackground*  pReadBgr )
{
    echelle = 1.0;
    dx      = 0.0;
    dy      = 0.0;
    pReadBgr = pReadBgr;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::findAllStar()
{
    if ( pReadBgr == NULL )         return;
    if ( !bFindStar )               return;

    int width = pReadBgr->w;
    int height = pReadBgr->h;

    logf( (char*)"Find all star(%d,%d)", width, height );

    Star *      p = new Star();
    
    p->setPtr( pReadBgr->ptr );
    p->setWidth( pReadBgr->w );

    for( int y_find=20; y_find<height; y_find+=(40) )
    {
        for( int x_find=20; x_find<width; x_find+=(40) )
        {
            //logf( (char*)"Cherche etoile(%d,%d)", x_find, y_find );
            if ( p->chercheLum(x_find, y_find, 50) )
            {
                //logf( (char*)"  (%d,%d)", p->getX(), p->getY() );
                
                p->setXY(x_find,y_find);            
                p->find();            
                int x_find = p->getX();
                int y_find = p->getY();
                p->computeMag();
                
                if ( p->getMagnitude() < 9.0 )     
                {
                    if ( starExist(x_find, y_find) )            
                    {
                        logf( (char*)"Etoile(%d,%d) mag=%0.2f   existe ...", x_find, x_find, p->getMagnitude() );
                        continue;
                    }
                    
                    Star * pp = new Star();
                    pp->setPtr( pReadBgr->ptr );
                    pp->setWidth( pReadBgr->w );

                    pp->setXY( x_find, y_find );
                    pp->find();
                    add( pp->getInfo() );
                    
                    v_tStars.push_back( pp );

                    logf( (char*)"Nouvelle etoile(%d,%d) mag=%0.2f", x_find, x_find, pp->getMagnitude() );

                    
                }
            }
        }
    }
    delete p;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool PanelCapture::starExist(int x, int y)
{
    int nb = v_tStars.size();
    for( int n=0; n<nb; n++ )
    {
        int x_star = v_tStars[n]->getX();
        int y_star = v_tStars[n]->getY();
        
        if ( abs(x-x_star) < 8 && abs(y-y_star) < 8 )       return true;
    }
    
    return false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::addStar(int x, int y)
{
    if ( pReadBgr == NULL )         return;


    float e = (float)pReadBgr->w / (float)getDX(); 
    
    float fx = e*(x-getX());
    float fy = e*(y-getY());

    Star *      p = new Star(fx,fy);
    
    p->setPtr( pReadBgr->ptr );
    p->setWidth( pReadBgr->w );

    p->setXY( fx, fy);
    p->find();
    //p->computeMag();
    add( p->getInfo() );

    v_tStars.push_back( p );

    logf( (char*)"Nouvelle etoile(%d,%d) mag=%0.2f", (int)p->getX(), (int)p->getY(), p->getMagnitude() );
    logf( (char*)"setXY( %0.4f, %0.4f )", fx, fy );
    logf( (char*)"Echelle %0.4f", e );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::setCentX(float f)
{
    dx = f;

    float fDX = (float)pReadBgr->w / echelle;
    float fDY = (float)pReadBgr->h / echelle;

    int dxp = getParent()->getDX();
    int dyp = getParent()->getDY();
    
    int deltax = (dxp-fDX)/2 + dx*echelle;
    int deltay = (dyp-fDY)/2 + dy*echelle;
    
    setPos( deltax, deltay );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::setCentY(float f)
{
    dy = f;

    float fDX = (float)pReadBgr->w / echelle;
    float fDY = (float)pReadBgr->h / echelle;

    int dxp = getParent()->getDX();
    int dyp = getParent()->getDY();
    
    int deltax = (dxp-fDX)/2 + dx*echelle;
    int deltay = (dyp-fDY)/2 + dy*echelle;
    
    setPos( deltax, deltay );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::setEchelle(float f)
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
void PanelCapture::displayGL()
{
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
    
    PanelSimple::displayGL();



    if ( bNuit )        glColor4f( gris,  0.0,  0.0, 1.0 );
    else                glColor4f( 0.0,   1.0,  0.0, 0.4 );    

    int nb = v_tStars.size();
    for( int i=0; i<nb; i++ )
    {
        float e = (float)getDX() / (float)pReadBgr->w; 
        //logf( (char*)"Echelle %0.2f", v_tStars[i]->getMagnitude() );

        v_tStars[i]->updatePos( getX(), getY(), e );
        v_tStars[i]->displayGL();
    }
    


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::releaseLeft(int xm, int ym)
{
    logf( (char*)"PanelCapture::releaseLeft(%d,%d) ...", xm, ym );
    

    addStar( xm, ym );
}




