#include "stars.h"
#include <malloc.h>
#include "panel_camera.h"

//#define DEBUG_UPDATES   0
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Stars::Stars()
{
    logf( (char*)"Constructeur Stars() ------------------" );
    RB       = NULL;
    pView    = NULL;
    pNbStars = NULL;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Stars::~Stars()
{
    logf( (char*)"Destructeur Stars() ------------------" );
    log_tab(true);
    deleteAllStars();
    log_tab(false);
    logf( (char*)"Destructeur Stars() ------------------" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Stars::add(Star * p)
{
    logf( (char*)"Stars::add(Star *)" );
    log_tab(true);
    int nb = v_tStars.size();

    for( int n=0; n<nb; n++ )
    {
        Star* ps = v_tStars[n];
        if ( ps == p )              {
            log_tab(false);
            logf( (char*)"Stars::add(Star *)" );
            return;
        }
    }
    v_tStars.push_back( p );


    if ( pNbStars != NULL )
    {
        char t[] = "00000000000";  
        sprintf( t, "%d", (int)v_tStars.size() );
        pNbStars->changeText( t );
    }

    log_tab(false);
    logf( (char*)"Stars::add(Star *)" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Stars::sup(Star * p)
{
    logf( (char*)"Stars::Sup Star()" );
    log_tab(true);

    int nb = v_tStars.size();

    for( int n=0; n<nb; n++ )
    {
        Star* ps = v_tStars[n];
        if ( ps == p )
        {
            
            //Panel* pPanel = p->getInfo()->getParent();
            //pPanel->sup( p->getInfo() );
            v_tStars.erase( v_tStars.begin() + n );
            delete p;

            log_tab(false);
            logf( (char*)"Stars::Sup Star()" );
            return;
        }
    }

    char t[] = "00000000000";  
    sprintf( t, "%d", (int)v_tStars.size() );
    pNbStars->changeText( t );
    log_tab(false);
    logf( (char*)"Stars::Sup Star()" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Star* Stars::addStar(int xm, int ym, int dx_screen, int dy_screen, float e )
{
    logf( (char*)"Stars::addStar() souris(%d, %d)   dx_screen=%d, dy_screen%d, echelle =%0.2f) : %d",
                  xm, ym, dx_screen, dy_screen, e, __LINE__);
    log_tab(true);

    float X = (float) ((float)xm - (float)dx_screen)/e; 
    float Y = (float) ((float)ym - (float)dy_screen)/e; 
    logf( (char*)"conversion  (%d, %d) => (%0.4f, %0.4f)", xm, ym, X, Y);
    


    Star * pp = new Star((int) X, (int) Y);
    pp->setPtr( RB->ptr );
    pp->setWidth( RB->w );
    pp->setHeight( RB->h );

    pp->chercheLum((int)X, (int)Y, 50);
    
    int x_find = pp->getX();
    int y_find = pp->getY();

    if ( starExist(x_find, y_find) )        { 
        delete pp;
        logf( (char*)"Etoile existe deja ..." );

        log_tab(false);
        logf( (char*)"Stars::addStar(%d, %d, %d, %d, %0.2f)", xm, ym, dx_screen, dy_screen, e);
        return NULL;
    }

    pp->setXY( x_find, y_find );
    pp->find();
    //pp->setView(pView);
    if ( x_find == -1 || y_find == -1 ) {
        delete pp;
        logf( (char*)"Pas d'Etoile ..." );

        log_tab(false);
        logf( (char*)"Stars::addStar(%d, %d, %d, %d, %0.2f)", xm, ym, dx_screen, dy_screen, e);
        return NULL;
    }
    
    logf( (char*)"Appelle star::updatePos()" );
    pp->updatePos( dx_screen, dy_screen, e, e ); 

    logf( (char*)"find(%d, %d)", pp->getXScreen(), pp->getYScreen()  );
    if ( starExist(pp->getXScreen(), pp->getYScreen()) )        { 
        delete pp;
        logf( (char*)"Etoile existe deja ..." );

        log_tab(false);
        logf( (char*)"Stars::addStar(%d, %d, %d, %d, %0.2f)", xm, ym, dx_screen, dy_screen, e);
        return NULL;
    }


    pp->computeMag();
    pp->getMagnitude();
    pView->add( pp->getInfo() );
    pp->setView( pView );
    pp->setRB( RB ); 




    v_tStars.push_back( pp );

    logf( (char*)"Nouvelle etoile no %d (%d,%d) mag=%0.2f find xy(%d,%d)", 
                v_tStars.size(), xm, ym, pp->getMagnitude() , x_find, y_find );


    char t[] = "00000000000";  
    sprintf( t, "%d", (int)v_tStars.size() );
    logf( (char*)"Nb etoiles %d", (int)v_tStars.size() );
    pNbStars->changeText( t );
    
    log_tab(false);
    logf( (char*)"Stars::addStar(%d, %d, %d, %d, %0.2f)", xm, ym, dx_screen, dy_screen, e);
    return pp;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool Stars::starExist(int xp, int yp )
{
    return starExist( xp, yp, -1 );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool Stars::starExist(int xp, int yp, int no )
{
    int nb = v_tStars.size();
    for( int n=0; n<nb; n++ )
    {
        int x_star = v_tStars[n]->getXScreen();
        int y_star = v_tStars[n]->getYScreen();
        
        int dx = abs(xp-x_star);
        int dy = abs(yp-y_star);
        
        //logf( (char*)"Etoile(%d,%d) v_tStars[%d](%d,%d) Test(%d,%d) ??", xp, yp, n, x_star, y_star, dx, dy );
        if ( dx < 8 && dy < 8 && n!=no )          return true;
    }
    
    return false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Stars::findAllStars()
{
    logf( (char*)"Stars::findAllStars()" );
    
    if (RB==NULL || RB->ptr==NULL)  return;
    if (pView==NULL )               return;

    log_tab(true);
        
    int dx_view = pView->getDX();
    int dy_view = pView->getDY();

    int width = RB->w;
    int height = RB->h;
    
    ech = (float) dx_view / (float)width; 

    logf( (char*)"Find all star(%d,%d)", width, height );

    Star *      p = new Star();
    
    p->setPtr( RB->ptr );
    p->setWidth( RB->w );
    p->setHeight( RB->h );

    for( int y_find=20; y_find<height; y_find+=(40) )
    {
        for( int x_find=20; x_find<width; x_find+=(40) )
        {
            //logf( (char*)"Cherche etoile(%d,%d)", x_find, y_find );
            if ( p->chercheLum(x_find, y_find, 50) )
            {
                //logf( (char*)"Examin etoile no %d (%d,%d)", v_tStars.size(), x_find, y_find );
                
                p->setXY(x_find,y_find);            
                p->find();            
                int x_find = p->getX();
                int y_find = p->getY();
                p->computeMag();
                
                if ( p->getMagnitude() < 11.0 )     
                {
                    p->updatePos(dx_view, dy_view, ech, ech);
                    if ( starExist(p->getXScreen(), p->getYScreen()) )            
                    {
                        //logf( (char*)"Etoile(%d,%d) mag=%0.2f   existe ...", x_find, y_find, p->getMagnitude() );
                        continue;
                    }
                    
                    Star * pp = new Star();
                    /*
                    pp->setPtr( RB->ptr );
                    pp->setWidth( RB->w );
                    pp->setHeight( RB->h );
                    */
                    pp->setRB( RB );
                    pp->setXY( x_find, y_find );
                    pp->find();

                    pp->updatePos(dx_view, dy_view, ech, ech);
                    pp->computeMag();
                    //pp->getMagnitude();
                    pView->add( pp->getInfo() );
                    pp->setView( pView );
                    
                    v_tStars.push_back( pp );

                    logf( (char*)" Add etoile no %d (%d,%d) mag=%0.2f", v_tStars.size(), x_find, y_find, pp->getMagnitude() );
                    logf( (char*)"      (%d,%d) e=%0.2f", dx_view, dy_view, ech );

                    
                }
            }
        }
    }
    delete p;

    //update_stars();
    suivi( RB );

    if ( pNbStars != NULL )
    {
        char t[] = "00000000000";  
        sprintf( t, "%d", (int)v_tStars.size() );
        logf( (char*)"Nb etoiles %d", (int)v_tStars.size() );
        pNbStars->changeText( t );
    }
    
    log_tab(false);
    logf( (char*)"Stars::findAllStars()" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Stars::deleteAllStars()
{
    logf( (char*)"Stars::deleteAllStars()" );
    log_tab(true);
    
    int nb = v_tStars.size();
    for( int n = nb-1; n>=0; n-- )
    {
        Star* p = v_tStars[n];
        //Panel*  panelPreview = p->getInfo()->getParent();
        //panelPreview->sup(p->getInfo());
        delete p;
        v_tStars.pop_back();
        p=0;
        
    }
    v_tStars.clear();

    char t[] = "00000000000";  
    sprintf( t, "%d", size() );
    pNbStars->changeText( t );

    logf( (char*)"Delete  %d Star()", nb );

    log_tab(false);
    logf( (char*)"Stars::deleteAllStars()" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Stars::setView(PanelSimple* p)
{
    pView = p;
    
    int nb = v_tStars.size();
    for( int n=0; n<nb; n++ )
    {
        v_tStars[n]->setView( p );
    }

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Stars::setPanelNbStars(PanelText* p)
{
    pNbStars = p;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Stars::setRB(rb_t* p)
{
    RB = p;
    
    int nb = v_tStars.size();
    for( int n=0; n<nb; n++ )
    {
        v_tStars[n]->setRB( p );
    }
    //logf( (char*)"setRB  %d Star()", nb );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Stars::updateRB(rb_t* p)
{
    setRB(p);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Stars::suivi(rb_t* p)
{
    //logf((char*)"Stars::suivi() w=%d h=%d  delta window (%d, %d)", p->w, p->h, pView->getDX(), pView->getDY() );
    
    int nb = v_tStars.size();
    setRB(p);
    int j = 0;
    vector<int> t;
    
    for (int i=0; i<nb; i++ )
    {
        float e;
        float ew = (float)pView->getDX() / (float)RB->w; 
        float eh = (float)pView->getDY() / (float)RB->h;
        if ( ew>eh )    e = ew; 
        else            e = eh;
        //logf((char*)"Stars::suivi() w=%d h=%d  delta window (%d, %d)", p->w, p->h, pView->getDX(), pView->getDY() );

        if ( !v_tStars[i]->find()   )
        {
            if ( v_tStars[i]->getNotFound() > 10   )
                t.push_back(i);
        }
        else v_tStars[i]->resetNotFound();
        
        v_tStars[i]->updatePos( pView->getX(), pView->getY(), ew, eh );
        v_tStars[i]->suivi();
    }
 
    nb = t.size();   
    logf( (char*)" Erase NB %d ", nb );
    
    for (int i=0; i<nb; i++ )
    {
        logf( (char*)" Erase %d ", t[i] );
    }

    if ( t.size() != 0 )
    {
        char t[] = "00000000000";  
        sprintf( t, "%d", size() );
        pNbStars->changeText( t );
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Stars::selectLeft( int xp, int yp)
{
    logf( (char*)"Stars::selectLeft(%d, %d) : %d ", xp, yp, __LINE__ );
    log_tab(true);
    int nb = v_tStars.size();
    for( int n=0; n<nb; n++ )
    {
        int x_star = v_tStars[n]->getX();
        int y_star = v_tStars[n]->getY();
        
        int dx = abs(xp-x_star);
        int dy = abs(yp-y_star);
        
        //logf( (char*)"Etoile(%d,%d) v_tStars[%d](%d,%d) Test(%d,%d) ??", xp, yp, n, x_star, y_star, dx, dy );
        if ( dx <20 && dy < 20 )
        {
            for( int k=0; k<nb; k++ )   { if (k!=n) v_tStars[k]->setZoom(false); }
            v_tStars[n]->setZoom(true);
        }
    }
    log_tab(false);
    //logf( (char*)"Stars::selectLeft(%d, %d) : %d ", xp, yp, __LINE__ );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Stars::selectMiddle( int xp, int yp)
{
    int nb = v_tStars.size();
    for( int n=0; n<nb; n++ )
    {
        int x_star = v_tStars[n]->getX();
        int y_star = v_tStars[n]->getY();
        
        int dx = abs(xp-x_star);
        int dy = abs(yp-y_star);
        
        //logf( (char*)"Etoile(%d,%d) v_tStars[%d](%d,%d) Test(%d,%d) ??", xp, yp, n, x_star, y_star, dx, dy );
        if ( dx <20 && dy < 20 )
            v_tStars[n]->toggleSuivi();
        else
            v_tStars[n]->setSuivi(false);
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Stars::selectStar( int xp, int yp)
{
    int nb = v_tStars.size();
    for( int n=0; n<nb; n++ )
    {
        int x_star = v_tStars[n]->getX();
        int y_star = v_tStars[n]->getY();
        
        int dx = abs(xp-x_star);
        int dy = abs(yp-y_star);
        
        //logf( (char*)"Etoile(%d,%d) v_tStars[%d](%d,%d) Test(%d,%d) ??", xp, yp, n, x_star, y_star, dx, dy );
        if ( dx <20 && dy < 20 )
            v_tStars[n]->toggleSelect();
        else
            v_tStars[n]->setSelect(false);
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Stars::update_stars( int DX, int DY, PanelSimple* pview, rb_t* rb)
{
    update_stars( DX, DY, pView, rb, 1.0 );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Stars::update_stars( int DX, int DY, PanelSimple* pview, rb_t* rb, float ech_usr)
{
    #ifdef DEBUG_UPDATES  
        logf( (char*)"Stars::update()  --- DEB ---" );
    #endif
    
    int nb = v_tStars.size();
    if (nb == 0)        return;

    if ( rb != NULL )       RB = rb;
    if ( pview != NULL )    pView = pview;

    //PanelCamera * panelCamera = dynamic_cast<PanelCamera*>(pview);
    //if ( panelCamera->getRB() != rb )
    //    pView->setBackground( rb->ptr, rb->w, rb->h, rb->d );

    
    dx = DX;
    dy = DY;
    
    float ew = (float)pView->getDX() / (float)RB->w; 
    float eh = (float)pView->getDY() / (float)RB->h;
    if ( ew<eh )    ech = ew; 
    else            ech = eh;
    ech *= ech_usr;
    //ech = (float)pView->getDX() / (float)RB->w; 
    
    #ifdef DEBUG_UPDATES  
        logf( (char*)"Stars::update() dx=%d, dy=%d, ech=%0.2f", dx, dy, ech );
        logf( (char*)"     w=%d, h=%d", RB->w.load(), RB->h.load() );
    #endif

    vector<int> t;
    t.clear();

    for( int i=0; i<nb; i++ )
    {
        Star* pStar = v_tStars[i];
        pStar->setRB( RB );

        if ( !pStar->find() && pStar->getNotFound() > 10   )
        {
            t.push_back(i);
            #ifdef DEBUG_UPDATES  
                logf( (char*)" Stars::update() push_back  !Star->find() = %d", i );
            #endif
        }
        // Star existe
        else
        {
            pStar->resetNotFound();
            
            int x_find = pStar->getX();
            int y_find = pStar->getY();

            #ifdef DEBUG_UPDATES  
                logf( (char*)" x_find=%d, y_find=%d", x_find, y_find );
            #endif
            if ( starExist(x_find, y_find, i) )        { 
                t.push_back(i);
                #ifdef DEBUG_UPDATES  
                    logf( (char*)" Stars::update() push_back existe = %d", i );
                #endif
            }
        }
        //*/       
        //v_tStars[i]->computeMag();
        //pStar->updatePos( dx, dy, ech_usr, ech_usr );
        pStar->updatePos( dx, dy, ew, eh );
    }
    //* 
    nb = t.size();
    int tot = v_tStars.size();    
    if ( pNbStars != NULL )
    {
        char t[] = "00000000000";  
        sprintf( t, "%d", (int)v_tStars.size() );
        
        #ifdef DEBUG_UPDATES  
            logf( (char*)"Nb etoiles %d", (int)v_tStars.size() );
        #endif
        
        pNbStars->changeText( t );
    }
    if ( nb!= 0 )
    {   
        for (int i=nb; i!=0; i-- )
        {
            int j = t[i-1];
            
            //logf( (char*)" Stars::update() erase no %d  (%d) ", j, tot );
            delete v_tStars[j];
            v_tStars.erase( v_tStars.begin()+j );

            tot = v_tStars.size();
        
            #ifdef DEBUG_UPDATES  
                logf( (char*)" reste %d", tot );
            #endif
        }
    }

    #ifdef DEBUG_UPDATES  
        logf( (char*)"Stars::update()  --- FIN ---" );
    #endif
    if ( nb != 0 )
    {
        char t[] = "00000000000";  
        sprintf( t, "%d", (int)v_tStars.size() );
        pNbStars->changeText( t );
    }
    //*/
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
vec2* Stars::getSuivi()
{
    int nb = v_tStars.size();
    for( int i=0; i<nb; i++ )
    {
        if ( v_tStars[i]->getSuivi() )
        {
            vec2* pV = new vec2();
            v_tStars[i]->getPos(pV);

    //#ifndef PANELCOURBE_H
    extern PanelCourbe* panelCourbe;
    //#endif
            if ( bSimu )
            { 
                vec3& vOrigine = panelCourbe->get_vOrigine();
                pV->x = vOrigine.x + 4.0 * (float)rand()/(float)RAND_MAX-2.0;
                pV->y = vOrigine.y + 4.0 * (float)rand()/(float)RAND_MAX-2.0;
            }
            //pV->y = v_tStars[i]->getSuiviY();
            //logf( (char*)"Stars::getSuivi() OK (%0.2f,%0.2f)", pV->x, pV->y );
            return pV;
        }
    }
    return NULL;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Stars::updateScreenPos( int DX, int DY, double ech )
{
    int nb = v_tStars.size();
    for( int i=0; i<nb; i++ )
    {
		v_tStars[i]->updatePos( DX, DY, ech, ech );
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Stars::idle()
{
	if ( !bVisible )			return;
    int color = 0;
    if ( bNuit )        
    {
        color = 0xFFFF0000;
    }
    else                //glColor4f( 1.0, 0.7, 0.0, 0.8 );
    {
        color = 0xFF00FF00;
    }

    int nb = v_tStars.size();
    for( int i=0; i<nb; i++ )
    {
        v_tStars[i]->setVisible( bVisible );
        v_tStars[i]->getInfo()->setColor(color);
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Stars::displayGL()
{
    //logf( (char*)"Stars::displayGL()" );
    //mat4 m = scale( 2.0, 2.0, 1.0 );
    float gris = 0.3;
    //if ( var.getb("bNuit") )        glColor4f( 0.5, 0.0, 0.0, 1.0 );    
    //else                            glColor4f( gris, gris, gris, 0.2 );

    if ( RB == NULL )        return;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

    //float x = getParent()->getX();
    //float y = getParent()->getY();


    if ( bNuit )        glColor4f( gris,  0.0,  0.0, 1.0 );
    else                glColor4f( 0.0,   1.0,  0.0, 0.4 );    

    float e = (float)pView->getX() / (float)RB->w.load(); 
    
    int nb = v_tStars.size();
    for( int i=0; i<nb; i++ )
    {
        //v_tStars[i]->updatePos( pView->getX(), pView->getY(), e );
        v_tStars[i]->displayGL();
    }


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Stars::position(double ra, double dc)
{
    int nb = v_tStars.size();
    for( int i=0; i<nb; i++ )
    {
        if ( v_tStars[i]->getSelect() )      v_tStars[i]->position(ra, dc);
        if ( v_tStars[i]->getSuivi() )       v_tStars[i]->position(ra, dc);
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Stars::set_delta(float xx, float yy)
{
    int nb = v_tStars.size();
    for( int i=0; i<nb; i++ )
    {
        v_tStars[i]->set_delta(xx, yy);
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int Stars::getSuiviScreenX()
{
    int nb = v_tStars.size();
    for( int i=0; i<nb; i++ )
    {
        Star* pStar = v_tStars[i];
        if ( pStar->getSuivi() )
        {
            int x = pStar->getX();
            return  x;
        }
    }
    return -1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int Stars::getSuiviScreenY()
{
    int nb = v_tStars.size();
    for( int i=0; i<nb; i++ )
    {
        Star* pStar = v_tStars[i];
        if ( pStar->getSuivi() )
        {
            int x = pStar->getY();
            return  x;
        }
    }
    return -1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Stars::setVisible( bool b )
{
	bVisible = b;
    int nb = v_tStars.size();
    for( int i=0; i<nb; i++ )
    {
        v_tStars[i]->setVisible(b);
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

