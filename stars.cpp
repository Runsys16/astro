#include "stars.h"
#include <malloc.h>

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Stars::Stars()
{
    logf( (char*)"Constructeur Stars()" );
    RB      = NULL;
    pView   = NULL;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Stars::add(Star * p)
{
    logf( (char*)"Stars::add(Star *)" );
    int nb = v_tStars.size();

    for( int n=0; n<nb; n++ )
    {
        Star* ps = v_tStars[n];
        if ( ps == p )              return;
    }
    v_tStars.push_back( p );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Stars::sup(Star * p)
{
    logf( (char*)"Stars::Sup Star()" );
    int nb = v_tStars.size();

    for( int n=0; n<nb; n++ )
    {
        Star* ps = v_tStars[n];
        if ( ps == p )
        {
            Panel* pPanel = p->getInfo()->getParent();
            pPanel->sup( p->getInfo() );
            v_tStars.erase( v_tStars.begin() + n );
            p = 0;
            return;
        }
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Star* Stars::addStar(int xm, int ym, int dx_screen, int dy_screen, float e )
{
    logf( (char*)"Stars::addStar(%d, %d, %d, %d, %0.2f)", xm, ym, dx_screen, dy_screen, e);
    xm = (float) ((float)xm - (float)dx_screen)/e; 
    ym = (float) ((float)ym - (float)dy_screen)/e; 
    


    Star * pp = new Star();
    pp->setPtr( RB->ptr );
    pp->setWidth( RB->w );
    pp->setHeight( RB->h );

    pp->chercheLum(xm, ym, 50);
    
    int x_find = pp->getX();
    int y_find = pp->getY();

    if ( starExist(x_find, y_find) )        { delete pp; logf( (char*)"Etoile existe deja ..." ); return NULL; }

    pp->setXY( x_find, y_find );
    pp->find();
    //pp->setView(pView);
    pp->updatePos( dx_screen, dy_screen, e ); 
    pp->computeMag();
    pp->getMagnitude();
    pView->add( pp->getInfo() );

    v_tStars.push_back( pp );

    logf( (char*)"Nouvelle etoile no %d (%d,%d) mag=%0.2f", v_tStars.size(), xm, ym, pp->getMagnitude() );
    
    return pp;
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool Stars::starExist(int xp, int yp )
{
    int nb = v_tStars.size();
    for( int n=0; n<nb; n++ )
    {
        int x_star = v_tStars[n]->getX();
        int y_star = v_tStars[n]->getY();
        
        int dx = abs(xp-x_star);
        int dy = abs(yp-y_star);
        
        //logf( (char*)"Etoile(%d,%d) v_tStars[%d](%d,%d) Test(%d,%d) ??", xp, yp, n, x_star, y_star, dx, dy );
        if ( dx < 8 && dy < 8 )       return true;
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
                
                if ( p->getMagnitude() < 9.0 )     
                {
                    if ( starExist(x_find, y_find) )            
                    {
                        //logf( (char*)"Etoile(%d,%d) mag=%0.2f   existe ...", x_find, y_find, p->getMagnitude() );
                        continue;
                    }
                    
                    Star * pp = new Star();
                    pp->setPtr( RB->ptr );
                    pp->setWidth( RB->w );
                    pp->setHeight( RB->h );

                    pp->setXY( x_find, y_find );
                    pp->find();
                    pp->updatePos(dx_view, dy_view, ech);
                    pp->computeMag();
                    //pp->getMagnitude();
                    pView->add( pp->getInfo() );
                    
                    v_tStars.push_back( pp );

                    logf( (char*)" Add etoile no %d (%d,%d) mag=%0.2f", v_tStars.size(), x_find, y_find, pp->getMagnitude() );
                    logf( (char*)"      (%d,%d) e=%0.2f", dx_view, dy_view, ech );

                    
                }
            }
        }
    }
    delete p;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Stars::deleteAllStars()
{
    int nb = v_tStars.size();
    for( int n = nb-1; n>=0; n-- )
    {
        Star* p = v_tStars[n];
        Panel*  panelPreview = p->getInfo()->getParent();
        panelPreview->sup(p->getInfo());
        delete p;
        v_tStars.pop_back();
        p=0;
        
    }
    v_tStars.clear();
    logf( (char*)"Delete  %d Star()", nb );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Stars::setView(Panel* p)
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
    int nb = v_tStars.size();
    setRB(p);
    for (int i=0; i<nb; i++ )
    {
        float e = (float)pView->getDX() / (float)RB->w; 
        //logf((char*)"Stars::suivi() w=%d h=%d  delta window (%d, %d)", p->w, p->h, pView->getDX(), pView->getDY() );

        v_tStars[i]->find();
        v_tStars[i]->updatePos( pView->getX(), pView->getY(), e );
        v_tStars[i]->suivi();
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Stars::selectLeft( int xp, int yp)
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
        {
            for( int k=0; k<nb; k++ )   { if (k!=n) v_tStars[k]->setZoom(false); }
            v_tStars[n]->setZoom(true);
        }
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Stars::selectRight( int xp, int yp)
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
        {
            v_tStars[n]->toggleSuivi();
            //logf( (char*)"Etoile trouve " );
        }
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Stars::update( int DX, int DY, Panel* pview, rb_t* rb)
{
    if ( rb != NULL )       RB = rb;
    if ( pview != NULL )    pView = pview;
    
    dx = DX;
    dy = DY;
    
    ech = (float)pView->getDX() / (float)RB->w; 
    
    //logf( (char*)"Stars::update() dx=%d, dy=%d, ech=%0.2f", dx, dy, ech );
    //logf( (char*)"     w=%d, h=%d", RB->w, RB->h );

    int nb = v_tStars.size();
    for( int i=0; i<nb; i++ )
    {
        v_tStars[i]->setRB( RB );
        v_tStars[i]->find();
        //v_tStars[i]->computeMag();
        v_tStars[i]->updatePos( dx, dy, ech );
    }


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
            v_tStars[i]->getPos(*pV);
            return pV;
        }
    }
    return NULL;
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

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

    //float x = getParent()->getX();
    //float y = getParent()->getY();


    if ( bNuit )        glColor4f( gris,  0.0,  0.0, 1.0 );
    else                glColor4f( 0.0,   1.0,  0.0, 0.4 );    

    float e = (float)pView->getX() / (float)RB->w; 
    
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

