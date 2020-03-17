#include "panel_courbe.h"

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelCourbe::PanelCourbe()
{
	WindowsManager&     wm  = WindowsManager::getInstance();
    VarManager&         var = VarManager::getInstance();

    //setDisplayGL(displayGLnuit_cb);
    //resizeCourbe( width, height );

    wm.add(this);
    
    //setVisible(bPanelCourbe);
 	setBackground((char*)"images/background.tga");
    //setDisplayGL( PanelCourbe::displayResultat_cb );

    pXMax = new PanelText( (char*)"+err",		PanelText::NORMAL_FONT, 5, 50 );
	add( pXMax );

    pXMin = new PanelText( (char*)"-err",		PanelText::NORMAL_FONT, 5, 60 );
	add( pXMin );

    pYMax = new PanelText( (char*)"+err",		PanelText::NORMAL_FONT, 5, 70 );
	add( pYMax );

    pYMin = new PanelText( (char*)"-err",		PanelText::NORMAL_FONT, 5, 80 );
	add( pYMin );

    err = var.getf("err");

    courbe1         = var.getf("courbe1");
    delta_courbe1   = var.getf("delta_courbe1");
    courbe2         = var.getf("courbe2");
    delta_courbe2   = var.getf("delta_courbe2");

    vOrigine.x      = var.getf("vOrigine.x");
    vOrigine.y      = var.getf("vOrigine.y");
    vOrigine.z      = 0.0;

    update_err();

    if ( var.existe("FileResultat") )
    {
        string* pFile = var.gets( "FileResultat" );
        charge_fichier( *pFile );
    }
    
    setVisible( var.getb( "bPanelCourbe" ) );
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
            x += getPosDX()-xStartAxe;
            glVertex2i( x, y );

            x = xStartAxe;
            y = AXE - i;
            x2Screen(x);
            y2Screen(y);
            glVertex2i( x, y );
            x += getPosDX()-xStartAxe;
            glVertex2i( x, y );
        }
        //
        // graduation verticale
        //
        fPas = courbe1;
        while( fPas < 10.0 )     fPas += courbe1;
        float M = getPosDX();
        for( float i=xStartAxe; i<M; i+=fPas )
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
        x += getPosDX()-xStartAxe;
        glVertex2i( x, y );
        //
        // Limite horizontale
        // Min et Max
        //
        glColor4fv( (GLfloat*)&colorLimit );

        y = (float)(delta_courbe1*(min) + AXE);
        if ( pMax != NULL )     pMax->setPos( 5, y-7 );
        x = xStartAxe;
        x2Screen(x);
        y2Screen(y);
        glVertex2i( x, y );
        x += getPosDX()-xStartAxe;
        glVertex2i( x, y );

        y = (float)(delta_courbe1*(max) + AXE);
        if ( pMin != NULL )     pMin->setPos( 5, y-7 );
        x = xStartAxe;
        x2Screen(x);
        y2Screen(y);
        glVertex2i( x, y );
        x += getPosDX()-xStartAxe;
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
int PanelCourbe::sc2winX( int n )
{
    return n - getX();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int PanelCourbe::sc2winY( int n )
{
    return n - getY();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::glCourbe()
{
	WindowsManager&     wm  = WindowsManager::getInstance();
    VarManager& var = VarManager::getInstance();

	if ( !visible )			return;
	
	

    int DY = getY();
    int n = t_vResultat.size();
    
    int  AXE_X = (float)getDY()/4.0;
    int  AXE_Y   (3.0*(float)getDY()/4.0);

    offset_x = vOrigine.x;
    offset_y = vOrigine.y;
    
    if ( decal_resultat >= t_vResultat.size() )     return;
    
    
    float height = (float)wm.getHeight();
	int   scx  = getX();
	int   scy  = height - getDY() - getY();
	int   scdx = getDX();
	int   scdy = getDY();
    
    glScissor( scx, scy, scdx, scdy );
    glEnable( GL_SCISSOR_TEST );

    
    if ( n != 0  )
    {
        glBegin(GL_LINE_STRIP);
        int deb = decal_resultat;
        int fin = deb + 2000.0/courbe1;
        
        if ( fin >= t_vResultat.size() )         fin = t_vResultat.size();
        if ( deb <= 0 )                          deb = 0;

        //
        // Courbe des X
        //
        if ( var.getb("bNuit") )        glColor4f( 1.0, 0.0, 0.0, 1.0 );
        else                            glColor4f( 0.0, 0.0, 1.0, 1.0 );
        for( int i=deb; i<fin; i++ )
        {
            float y = t_vResultat[n-i-1].x - offset_x;
            y = (float)(delta_courbe1*(y) + AXE_X);
            
            int x = (i-deb)*courbe1 + xStartAxe;
            if ( x > getPosDX() )                           break;

            int Y = y;

            x2Screen(x);
            y2Screen(Y);

            glVertex2i( x, Y );
            //logf( "%d %0.2f %0.2f y=%0.2f", n-i-1, t_vResultat[i].x, offset_x, y );
        }
        glEnd();        

        
        //
        // Courbe des Y
        //
        if ( var.getb("bNuit") )        glColor4f( 1.0, 0.0, 0.0, 1.0 );
        else                            glColor4f( 1.0, 1.0, 0.0, 1.0 );
        glBegin(GL_LINE_STRIP);
        //for( int i=decal_resultat; i<(t_vResultat.size()-decal_resultat); i++ )
        for( int i=deb; i<fin; i++ )
        {
            float y = t_vResultat[n-i-1].y - offset_y;
            y = (float)(delta_courbe2*(y) + AXE_Y);
            int x = (i-deb)*courbe2 + xStartAxe;
            if ( x > getPosDX() )                           break;

            int Y = y;
            x2Screen(x);
            y2Screen(Y);

            glVertex2i( x, Y );
        }
        glEnd();   
        
    }     
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
//void PanelCourbe::displayResultat_cb(void)
void PanelCourbe::displayGL(void)
{
    VarManager& var = VarManager::getInstance();
	if ( !visible )			return;

    if ( var.getb("bNuit") )        glColor4f( 1.0, 0.0, 0.0, 1.0 );
    else                            glColor4f( 1.0, 1.0, 0.0, 1.0 );
    PanelWindow::displayGL();

    glEchelle();
    glCourbe();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::clickMiddle( int xm, int ym )
{
    logf( (char*)"PanelCourbe::clickMiddle( %d, %d )", xm, ym );

    xm_svg = sc2winX(xm);
    ym_svg = sc2winY(ym);

    courbe1_svg = courbe1;
    courbe2_svg = courbe2;
    delta_courbe1_svg = delta_courbe1;
    delta_courbe2_svg = delta_courbe2;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::motionMiddle( int xm, int ym )
{
    //logf( (char*)"PanelCourbe::motionMiddle( %d, %d )", sc2winX(xm), sc2winY(ym) );

    float deltaX = sc2winX(xm) - xm_svg;
    float deltaY = sc2winY(ym) - ym_svg;
    
    deltaX = 1.0 + 2.0 * deltaX / ((float)getDX()/2.0);
    deltaY = 1.0 - 2.0 * deltaY / ((float)getDY()/2.0);

    if ( deltaX < 0.1 )         return;
    if ( deltaY < 0.1 )         return;
    
    courbe1 = deltaX * courbe1_svg;
    courbe2 = deltaX * courbe2_svg;

    delta_courbe1 = deltaY * delta_courbe1_svg;
    delta_courbe2 = deltaY * delta_courbe2_svg;

    //logf( (char*)"  %0.2f, %0.2f )", courbe1, courbe2 );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::releaseMiddle( int xm, int ym )
{
    VarManager& var = VarManager::getInstance();
    logf( (char*)"PanelCourbe::releaseMiddle( %d, %d )", xm, ym );

    motionMiddle( xm, ym );

    logf( (char*)"  courbe        %0.2f, %0.2f )", courbe1, courbe2 );
    logf( (char*)"  delta_courbe  %0.2f, %0.2f )", delta_courbe1, delta_courbe2 );

    var.set("courbe1", courbe1);
    var.set("delta_courbe1", delta_courbe1);
    var.set("courbe2", courbe2);
    var.set("delta_courbe2", delta_courbe2);
    //courbe1 = delta_courbe1_svg;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::clickLeft( int xm, int ym )
{
    logf( (char*)"PanelCourbe::clickLeft( %d, %d )", xm, ym );

    xm_svg = sc2winX(xm);
    ym_svg = sc2winY(ym);

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::motionLeft( int xm, int ym )
{
    //logf( (char*)"PanelCourbe::motionLeft( %d, %d )", xm, ym );
    
    decal_resultat += 1.0/courbe1 * (float)(xm_svg - sc2winX(xm));

    xm_svg = sc2winX(xm);
    ym_svg = sc2winY(ym);

    
    //logf( (char*)"PanelCourbe::motionLeft( %d, %0.2f )", decal_resultat, courbe1 );


    if ( decal_resultat <0 )        decal_resultat = 0;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::releaseLeft( int xm, int ym )
{
    VarManager& var = VarManager::getInstance();
    logf( (char*)"PanelCourbe::releaseLeft( %d, %d )", xm, ym );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

