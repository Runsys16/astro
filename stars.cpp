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
    logf( (char*)"Add Star()" );
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
    logf( (char*)"Sup Star()" );
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
bool Stars::starExist(int xp, int yp )
{
    int nb = v_tStars.size();
    for( int n=0; n<nb; n++ )
    {
        int x_star = v_tStars[n]->getX();
        int y_star = v_tStars[n]->getY();
        
        if ( abs(xp-x_star) < 8 && abs(yp-y_star) < 8 )       return true;
    }
    
    return false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Stars::findAllStars()
{
    logf( (char*)"Stars::findAllStars()" );

    if (RB==NULL || RB->ptr==NULL) return;

    int width = RB->w;
    int height = RB->h;

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
                        //logf( (char*)"Etoile(%d,%d) mag=%0.2f   existe ...", x_find, x_find, p->getMagnitude() );
                        continue;
                    }
                    
                    Star * pp = new Star();
                    pp->setPtr( RB->ptr );
                    pp->setWidth( RB->w );
                    pp->setHeight( RB->h );

                    pp->setXY( x_find, y_find );
                    pp->find();
                    pView->add( pp->getInfo() );
                    
                    v_tStars.push_back( pp );

                    logf( (char*)"Nouvelle etoile no %d (%d,%d) mag=%0.2f", v_tStars.size(), x_find, x_find, pp->getMagnitude() );

                    
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
    for( int n = nb-1; n>0; n-- )
    {
        Star* p = v_tStars[n];
        Panel*  panelPreview = p->getInfo()->getParent();
        panelPreview->sup(p->getInfo());
        delete p;
        v_tStars.pop_back();
        p=0;
        
    }
    logf( (char*)"Delete  %d Star()", nb );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Stars::setView(Panel* p)
{
    pView = p;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Stars::setRB(rb_t* p)
{
    RB = p;
    
    int nb = v_tStars.size();
    for( int n = nb-1; n>0; n-- )
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
        v_tStars[i]->updatePos( pView->getX(), pView->getY(), e );
        v_tStars[i]->find();
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

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

    //float x = getParent()->getX();
    //float y = getParent()->getY();


    if ( bNuit )        glColor4f( gris,  0.0,  0.0, 1.0 );
    else                glColor4f( 0.0,   1.0,  0.0, 0.4 );    

    
    int nb = v_tStars.size();
    for( int i=0; i<nb; i++ )
    {
        float e = (float)pView->getDX() / (float)RB->w; 
        //logf( (char*)"Echelle %0.2f", v_tStars[i]->getMagnitude() );

        v_tStars[i]->updatePos( pView->getX(), pView->getY(), e );
        v_tStars[i]->displayGL();
    }


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

