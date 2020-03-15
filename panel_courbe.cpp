#include "panel_courbe.h"

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelCourbe::PanelCourbe()
{
	WindowsManager& wm = WindowsManager::getInstance();

    //setDisplayGL(displayGLnuit_cb);
    resizeCourbe( width, height );

    wm.add(this);
    
    //setVisible(bPanelCourbe);
 	setBackground((char*)"images/background.tga");
    setDisplayGL( PanelCourbe::displayResultat_cb );

    pXMax = new PanelText( (char*)"+err",		PanelText::NORMAL_FONT, 5, 50 );
	add( pXMax );

    pXMin = new PanelText( (char*)"-err",		PanelText::NORMAL_FONT, 5, 60 );
	add( pXMin );

    pYMax = new PanelText( (char*)"+err",		PanelText::NORMAL_FONT, 5, 70 );
	add( pYMax );

    pYMin = new PanelText( (char*)"-err",		PanelText::NORMAL_FONT, 5, 80 );
	add( pYMin );

    update_err();

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::update_err()
{
    char s[20];
    sprintf( s, "+%0.2f", err );
    pXMax->changeText( (char*)s );
    pYMax->changeText( (char*)s );

    sprintf( s, "-%0.2f", err );
    pXMin->changeText( (char*)s );
    pYMin->changeText( (char*)s );
}
/*
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::screen2tex( int& x, int& y )
{
    x = (float)(x-xCam) * rw;
    y = (float)(y-yCam) * rh;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::tex2screen( int& x, int& y )
{
    x = (float)x / rw + xCam;
    y = (float)y / rh + yCam;
}
*/
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::glEchelleAxe( int AXE, int SIZE, float max, float min, PanelText* pMax, PanelText* pMin )
{
    VarManager& var = VarManager::getInstance();

    float gris = 0.3;
    vec4 color, colorLimit, colorAxe;
    
    if ( var.getb("bNuit") )
    {
        color       = vec4( 0.15, 0.0, 0.0, 1.0 );    
        colorLimit  = vec4( 0.8, 0.0, 0.0, 1.0 );    
        colorAxe    = vec4( 1.0, 0.0, 0.0, 1.0 );
    }
    else                            
    {
        gris = 0.15;
        color       = vec4( gris, gris, gris, 1.0 );
        colorLimit  = vec4( 1.0, 0.0, 1.0, 1.0 );
        colorAxe    = vec4( 1.0, 1.0, 1.0, 1.0 );
    }
    
    glBegin(GL_LINES);

        //
        // graduation horizontale
        //
        glColor4fv( (GLfloat*)&color );
        int pas = delta_courbe1;
        float fPas = delta_courbe1;
        while( fPas < 10.0 )     fPas += courbe1;

        for( float i=0; i<SIZE/2; i+=fPas )
        {
            int x = xStartAxe;
            int y = AXE + i;
            x2Screen(x);
            y2Screen(y);
            glVertex2i( x, y );
            x += getPosDX();
            glVertex2i( x, y );

            x = xStartAxe;
            y = AXE - i;
            x2Screen(x);
            y2Screen(y);
            glVertex2i( x, y );
            x += getPosDX();
            glVertex2i( x, y );
        }
        //
        // graduation verticale
        //
        fPas = courbe1;
        while( fPas < 10.0 )     fPas += courbe1;
        for( float i=xStartAxe; i<2000; i+=fPas )
        {
            int x = i;
            int y = AXE;
            x2Screen(x);
            y2Screen(y);
            y -= SIZE/2;
            glVertex2i( x, y );
            
            y += SIZE;
            glVertex2i( x, y );
        }
        //
        // axe horizontale centrale
        //
        glColor4fv( (GLfloat*)&colorAxe );
        int y = AXE;
        int x = xStartAxe;

        x2Screen(x);
        y2Screen(y);

        glVertex2i( x, y );
        x += getPosDX();
        glVertex2i( x, y );
        //
        // Limite horizontale
        // Min et Max
        //
        glColor4fv( (GLfloat*)&colorLimit );

        y = (float)(delta_courbe1*(min) + AXE);
        if ( pMax != NULL )     pMax->setPos( 5, y-7 );
        y2Screen(y);
        glVertex2i( xStartAxe, y );
        glVertex2i( x, y );

        y = (float)(delta_courbe1*(max) + AXE);
        if ( pMin != NULL )     pMin->setPos( 5, y-7 );
        y2Screen(y);
        glVertex2i( xStartAxe, y );
        glVertex2i( x, y );

        glColor4fv( (GLfloat*)&colorAxe );
    glEnd();        
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::glEchelle()
{
    VarManager& var = VarManager::getInstance();

    int  AXE_X = (float)getDY()/4.0;
    int  AXE_Y = (3.0*(float)getDY()/4.0);
    
    int dy_axe = (float)getDY() / 2.0;
    
    glEchelleAxe( AXE_X, dy_axe, (float)+err, (float)-err, pXMax, pXMin );
    glEchelleAxe( AXE_Y, dy_axe, (float)+err, (float)-err, pYMax, pYMin );

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::displayResultat_cb(void)
{
    VarManager& var = VarManager::getInstance();

    
    int DY = getY();
    int n = t_vResultat.size();
    
    int  AXE_X = (float)getDY()/4.0;
    int  AXE_Y   (3.0*(float)getDY()/4.0);

    /*
    if ( bCorrection || bRestauration )
    {
        offset_x = vOrigine.x;
        offset_y = vOrigine.y;
    }
    */
    offset_x = vOrigine.x;
    offset_y = vOrigine.y;

    glEchelle();
    
    if ( decal_resultat >= t_vResultat.size() )     return;
    
    if ( n != 0  )
    {
        if ( var.getb("bNuit") )        glColor4f( 1.0, 0.0, 0.0, 1.0 );
        else                            glColor4f( 0.0, 0.0, 1.0, 1.0 );
        
        glBegin(GL_LINE_STRIP);
        int deb = decal_resultat;
        int fin = deb + 2000.0/courbe1;
        
        if ( fin >= t_vResultat.size() )        fin = t_vResultat.size();
        if ( deb <= 0 )                          deb = 0;
        //int fin = t_vResultat.size()-decal_resultat;
        //logf( 
        //for( int i=decal_resultat; i<(t_vResultat.size()-decal_resultat); i++ )
        for( int i=deb; i<fin; i++ )
        {
            float y = t_vResultat[n-i-1].x - offset_x;
            y = (float)(delta_courbe1*(y) + AXE_X);
            int x = (i-deb)*courbe1 + xStartAxe;

            int Y = y;
            x2Screen(x);
            y2Screen(Y);


            glVertex2i( x, Y );
            //logf( "%d %0.2f %0.2f y=%0.2f", n-i-1, t_vResultat[i].x, offset_x, y );
        }
        glEnd();        

        
        if ( var.getb("bNuit") )        glColor4f( 1.0, 0.0, 0.0, 1.0 );
        else                            glColor4f( 1.0, 1.0, 0.0, 1.0 );
        glBegin(GL_LINE_STRIP);
        //for( int i=decal_resultat; i<(t_vResultat.size()-decal_resultat); i++ )
        for( int i=deb; i<fin; i++ )
        {
            float y = t_vResultat[n-i-1].y - offset_y;
            y = (float)(delta_courbe2*(y) + AXE_Y);
            int x = (i-deb)*courbe2 + xStartAxe;

            int Y = y;
            x2Screen(x);
            y2Screen(Y);

            glVertex2i( x, Y );
        }
        glEnd();   
        
    }     
    
    if ( var.getb("bNuit") )        glColor4f( 1.0, 0.0, 0.0, 1.0 );
    else                            glColor4f( 1.0, 1.0, 1.0, 1.0 );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

