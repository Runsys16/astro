#include "stars.h"
#include <malloc.h>

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Star::~Star()
{
    logf( (char*)"Destructeur Star()" );
    if (panelZoom != NULL)
    {
        pView->sup( panelZoom );

        delete panelZoom;
        panelZoom = NULL;
    }

    if (pInfo != NULL)
    {
        Panel*  pp = pInfo->getParent();
        if ( pp != NULL )     pp->sup(pInfo);

        delete pInfo;
        pInfo = NULL;
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Star::Star()
{
    logf( (char*)"Constructeur Star()" );
    init( -1, -1 );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Star::Star(int X, int Y)
{
    logf( (char*)"Constructeur Star(%d,%d)", X, Y );
    init( X, Y );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Star::init(int xx, int yy)
{
    logf( (char*)"Star::init(%d,%d)", xx, yy );
    x           = xx;
    y           = yy;
    ech         = 1.0;
    limitLum    = 20.0;
    ptr         = NULL;
    bSelect     = false;
    bSuivi      = false;
    bZoom       = false;
    
    pInfo       = new PanelText( (char*)"mag=",		PanelText::NORMAL_FONT, x, y );
    panelZoom   = NULL;
    RB          = NULL;
    pView       = NULL;
    
    ra_rad      = 9999.0;
    dc_rad      = 9999.0;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Star::setPtr(GLubyte* p)
{
    ptr = p;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Star::setWidth(int w)
{
    width = w;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Star::setHeight(int h)
{
    height = h;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Star::setRB(rb_t* p)
{
    ptr =           p->ptr;
    width =         p->w;
    height =        p->h;
    RB =            p;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Star::setView(Panel* p)
{
    pView = p;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int Star::getOffset( int X, int Y )
{
    int d = 3;
    if (RB!= NULL)          d = RB->d;

    return d*(X) + d*(Y)* width;
}
/*
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Star::screen2tex( int& x, int& y )
{
    x = (float)(x-xCam) * rw;
    y = (float)(y-yCam) * rh;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Star::tex2screen( int& x, int& y )
{
    x = (float)x / rw + xCam;
    y = (float)y / rh + yCam;
}
*/
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Star::computeMag()
{
    magnitude = -(log( ponderation ) / log(2.0)) + 17.0;
    
    if ( bZoom )
        sprintf( p_sInfo, "mag=%0.2f (%0.2f, %0.2f)", magnitude, pos.x, pos.y );
    else
        sprintf( p_sInfo, "mag=%0.2f", magnitude );
    
    if ( haveCoord() )
    {
        char sDEC[255];
        char sRA[255];
        
        struct dms DMS;
        rad2dms( DMS, dc_rad );
        sprintf( sDEC, "DEC=%02d:%02d:%0.2f", (int)DMS.d, (int)DMS.m, DMS.s );

        struct hms HMS;
        rad2hms( HMS, ra_rad );
        sprintf( sRA,  "RA=%02d:%02d:%0.2f", (int)HMS.h, (int)HMS.m, HMS.s );

        sprintf( p_sInfo, "mag=%0.2f %s %s", magnitude, (char*)sRA, (char*)sDEC );
    }

    pInfo->changeText( p_sInfo );
    }
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
float Star::computeRayon()
{
    float r = (15.0 - magnitude)*1.5 - 8.0;
    if ( r<0.0 )    r = 0.0;
    return r;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
float Star::getLumLimit(int offset, float limit )
{
    float r;
    float g;
    float b;

    try
    {
        r = ptr[offset+0]; 
        g = ptr[offset+1]; 
        b = ptr[offset+2]; 
    }
    catch ( const std::exception& e )
    {
        std::cout << e.what() << std::endl;
        return -1;
    }

    float l = 0.33 * r + 0.5 * g  + 0.16 * b;
    if ( l<limit )       l = 0.0;

    return l;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
float Star::getLum(int offset )
{
    return getLumLimit( offset, limitLum );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
float Star::getLum(int X, int Y, int limit )
{
    int offset = getOffset( X, Y );
    return getLumLimit( offset, limit );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
float Star::getColorR(int offset)
{
    float r;

    try
    {
        r = ptr[offset+0]; 
    }
    catch ( const std::exception& e )
    {
        std::cout << e.what() << std::endl;
        return -1;
    }
    return r;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
float Star::getColorG(int offset)
{
    float r;

    try
    {
        r = ptr[offset+1]; 
    }
    catch ( const std::exception& e )
    {
        std::cout << e.what() << std::endl;
        return -1;
    }
    return r;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
float Star::getColorB(int offset)
{
    float r;

    try
    {
        r = ptr[offset+2]; 
    }
    catch ( const std::exception& e )
    {
        std::cout << e.what() << std::endl;
        return -1;
    }
    return r;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool Star::cherche(int X, int Y, int size)
{

    yFound = X - size;
        
    //size_t size_ptr = malloc_usable_size ( (void*) ptr);
    //-------------------------------------------------------------------------    
    yFound = Y - size;
    for( int i =-size; i<size; i++ ) 
    {
        xFound = X+i;

        if ( xFound < 0)                        continue;
        if ( xFound >= width )                  continue;
        if ( yFound < 0)                        continue;
        if ( yFound >= height )                 continue;

        int offset = getOffset( xFound, yFound );

        //if ( offset+4 > size_ptr )          continue;
        float l = getLum( offset) ;
        if (l!=0.0)                         { x = xFound; y=yFound; return true; }
    }  
    //-------------------------------------------------------------------------    
    yFound = Y + size;
    for( int i =-size; i<size; i++ ) 
    {
        xFound = X+i;

        if ( xFound < 0)                        continue;
        if ( xFound >= width )                  continue;
        if ( yFound < 0)                        continue;
        if ( yFound >= height )                 continue;

        int offset = getOffset( xFound, yFound );

        //if ( offset+4 > size_ptr )          continue;
        float l = getLum( offset) ;
        if (l!=0.0)                         { x = xFound; y=yFound; return true; }
    }  
    //-------------------------------------------------------------------------    
    xFound = X - size;
    for( int i =-size; i<size; i++ ) 
    {
        yFound = Y + i;

        if ( xFound < 0)                        continue;
        if ( xFound >= width )                  continue;
        if ( yFound < 0)                        continue;
        if ( yFound >= height )                 continue;

        int offset = getOffset( xFound, yFound );

        //if ( offset+4 > size_ptr )          continue;
        float l = getLum( offset) ;
        if (l!=0.0)                         { x = xFound; y=yFound; return true; }
    }  
    //-------------------------------------------------------------------------    
    xFound = X + size;
    for( int i =-size; i<size; i++ ) 
    {
        yFound = Y + i;

        if ( xFound < 0)                        continue;
        if ( xFound >= width )                  continue;
        if ( yFound < 0)                        continue;
        if ( yFound >= height )                 continue;

        int offset = getOffset( xFound, yFound );

        //if ( offset+4 > size_ptr )          continue;
        float l = getLum( offset) ;
        if (l!=0.0)                         { x = xFound; y=yFound; return true; }
    } 
    
    return false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool Star::cherche(int X, int Y, int size, float lum)
{
    limitLum = lum;
    
    return cherche(X, Y, size);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool Star::chercheLum(int X, int Y, int size)
{

    for ( int n=0; n<size; n++ )
    {
        if( cherche( X, Y, n ) )                return true;
    }    
    return false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool Star::chercheLum(int X, int Y, int size, int limit)
{

    for ( int n=0; n<size; n++ )
    {
        if( cherche( X, Y, n, limit ) )                return true;
    }    
    return false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool Star::find( int size)
{
    struct sky_point point;
    
    int i;
    for( i=0; i<size; i++ )
    {
        limitLum = 20.0;
        bool bTrouve = cherche(x, y, i);
        if ( bTrouve )
        {
            x = xFound;
            y = yFound;
#ifdef DEBUG
            logf( (char*)"Iteration : %d  (%d,%d)", i, x, y );
#endif

            getFWHM( xFound, yFound, 20 );


#ifdef DEBUG
            logf( (char*)"Iteration : %d  (%d,%d)", i, x, y );
#endif
            break;
        }
    }

    if ( i == size )    {
        logf( (char*)"Iteration Max : %d", size );
        return false;
    }
    return true;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool Star::find()
{
    return find(500);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Star::getFWHMLine( int xx, int yy, int size)
{
    int min = xx - size;
    int max = xx + size;

    if ( min < 0)                   return;
    if ( max >= width )             return;
    if ( yy < 0)                    return;
    if ( yy >= height )             return;
    /*
    size_t size_ptr;
    try
    {
        size_ptr = malloc_usable_size ( (void*) ptr);
    }
    catch(std::exception const& e)
    {
        cerr << "ERREUR : " << e.what() << endl;
    }
    */
     
    //--------------------------------------------------    
    for( int X=min; X<max; X++) 
    {
        int offset = getOffset(X,yy);
        //if ( offset+4 > size_ptr )              continue;

        //float l = getLumLimit( offset, 20.0 );
        float l = getLum( offset );

        //if ( l<limitLum )       l = 0.0;
        if ( l>maxLum )         maxLum = l;
        
        vFWHM.x     += l * (float)X;
        vFWHM.y     += l * (float)yy;
        ponderation += l;
    }  
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Star::getFWHM( int xx, int yy, int size)
{
    int min;
    int max;

    vFWHM.x         = 0.0;
    vFWHM.y         = 0.0;
    ponderation     = 0.0;
    maxLum          = 0.0;
    limitLum        = 20.0;
    
    min = yy - size;
    max = yy + size;
    

    for(int Y=min; Y<max; Y ++)        getFWHMLine(xx, Y, size);

    vFWHM.x     /= ponderation;
    vFWHM.y     /= ponderation;
    
    xx = round(vFWHM.x);
    yy = round(vFWHM.y);

    min = yy - size;
    max = yy + size;

    vFWHM.x         = 0.0;
    vFWHM.y         = 0.0;
    ponderation     = 0.0;
    limitLum = maxLum / 2.0;
    //limitLum = 20.0;
    size = computeRayon();
    if ( size < 10 )            size = 10;
    
    for(int Y=min; Y<max; Y ++)        getFWHMLine(xx, Y, size);
    limitLum = 20.0;
    
    computeMag();
    
#ifdef DEBUG
    logf( (char*)"Pond : %0.2f mag=%0.2f max=%0.2f limit=%0.2f", ponderation, magnitude, maxLum, limitLum );
    logf( (char*)" pt (%0.2f,%0.2f)", vFWHM.x/ponderation, vFWHM.y/ponderation );
#endif

    vFWHM.x     /= ponderation;
    vFWHM.y     /= ponderation;
    
    x = round(vFWHM.x);
    y = round(vFWHM.y);
    
    pos.x = vFWHM.x +0.5;
    pos.y = vFWHM.y +0.5;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Star::glCercle(int rayon)
{
    int delta_x = 0;
    int delta_y = 0;
    
    if (pView!=NULL) {
        delta_x = pView->getX();
        delta_y = pView->getY();
        //logf( (char*)"Star::glCercle() : delta(%d, %d)", delta_x, delta_y );
    }
    delta_x = 0;
    delta_y = 0;

    int xx = x_screen - delta_x;
    int yy = y_screen - delta_y;

    /*
    xx = x;
    yy = y;
    screen2tex(x, y);
    */
    //logf( (char*)"Star::glCercle() : (%d, %d)  (%d, %d)", xx, yy, x, y );
	glBegin(GL_LINE_LOOP);

        for( float i=0; i<=360.0; i+=1.0 )
        {
            float fx = (float)xx+ (float)rayon*cos(DEG2RAD(i));
            float fy = (float)yy+ (float)rayon*sin(DEG2RAD(i));
            glVertex2i(fx,fy);
        }
        
    glEnd();        
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Star::glCarre( int dx,  int dy )
{
    int x = x_screen;
    int y = y_screen;
    
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
void Star::glCarre( int d )
{
    glCarre(d, d);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Star::glCroix( int dx,  int dy )
{
    int x = x_screen;
    int y = y_screen;
    
	glBegin(GL_LINES);

	    glVertex2i(x, y-dy);         glVertex2i(x, y+dy);
	    glVertex2i(x-dx, y);         glVertex2i(x+dx, y);

    glEnd();        
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Star::glMark( int dx,  int dy )
{
    int x = x_screen;
    int y = y_screen;
    
	glBegin(GL_LINES);

	    glVertex2i(x, y+5);         glVertex2i(x, y+dy);
	    glVertex2i(x+5, y);         glVertex2i(x+dx, y);

    glEnd();        
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Star::screen2tex(int& X, int& Y)
{
    float xx = ((float)x_screen-(float)dx_screen) / ech;
    float yy = ((float)y_screen-(float)dy_screen) / ech;

    X = xx;
    Y = yy;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Star::updatePos(int X, int Y, float e)
{
    //logf( (char*)"Star::updatePos(int %d, int %d, float %f)", X, Y, e );
    dx_screen = X;
    dy_screen = Y;
    
    ech = e;
    x_screen = e*(pos.x+0.5) + (float)X;
    y_screen = e*(pos.y+0.5) + (float)Y;
    
    pInfo->setPos( e*x + 8, e*y + 8 );
    pInfo->updatePos();
    if (panelZoom!=NULL)
    {
        //logf( (char*)"-------------" );
        panelZoom->setRB( RB );
        float xx = 1.0 * x;
        float yy = 1.0 * y;
        panelZoom->setPosStar(pos.x, pos.y);
        //logf( (char*)"Appel de setPosAndSize() x,y(%d, %d)", x, y );
        panelZoom->setPosAndSize( (x+40)*ech, (y+40)*ech, 200, 200 );
        //logf( (char*)"-------------" );
        panelZoom->updatePos();
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Star::updatePos(int X, int Y)
{   
    updatePos( X, Y, 1.0 );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Star::suivi()
{
    if ( panelZoom == NULL )                return;
    
    panelZoom->setRB( RB );
    panelZoom->setPosStar(pos.x, pos.y);
    panelZoom->setPosAndSize( (x+40)*ech, (y+40)*ech, 300, 300 );
    panelZoom->updatePos();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Star::setZoom(bool b)
{
    if ( b )
    {
        if ( bZoom )
        {
            bZoom = bSelect = false;
            if (panelZoom!=NULL)
            {
                pView->sup( panelZoom );
                delete panelZoom;
                panelZoom = NULL;
            }
            return;
        }
        else
        {
            bZoom = bSelect = true;
            panelZoom = new PanelZoom();
            pView->add( panelZoom );
            //panelZoom->setPosStar(xFound, yFound);
            panelZoom->setPosAndSize( (x+40)*ech, (y+40)*ech, 200, 200 );
            
            logf( (char*)"Star::displayGL() setBackGround()" );
            if ( RB != NULL )
            {
                logf( (char*)"Star::displayGL() setRB()" );
                panelZoom->setRB( RB );
                //panelZoom->setPosAndSize( (x+40)*ech, (y+40)*ech, 200, 200 );
            }
            panelZoom->updatePos();
            return;
        }
        
    }
    else
    {
        bZoom = false;
        if (panelZoom!=NULL)
        {
            pView->sup( panelZoom );
            delete panelZoom;
            panelZoom = NULL;
        }
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Star::displayGL()
{
    //logf( (char*)"Star::displayGL()" );
    //glCroix(20,20);
    if ( bNuit )        glColor4f( 0.3,  0.0,  0.0, 1.0 );
    else                glColor4f( 0.0,   1.0,  0.0, 0.4 );    

    glMark(ech*40, ech*40);
    glCercle( ech*computeRayon() + 4 );
    
    if ( bZoom )      
    {
        glColor4f( 1.0,  1.0,  0.0, 1.0 );
        glCarre( ech*computeRayon() + 4 +10 );
    }
    else if ( bSelect )      
    {
        glColor4f( 1.0,  0.0,  0.0, 1.0 );
        glCarre( ech*computeRayon() + 4 +10 );
    }

    if ( bSuivi )      
    {
        glColor4f( 1.0,  0.0,  0.0, 1.0 );
        glCercle( 1.5*ech*computeRayon() + 4 +10 );
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Star::position(double ra, double dc)
{
    ra_rad = ra;
    dc_rad = dc;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

