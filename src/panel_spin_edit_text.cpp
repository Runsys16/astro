#include "panel_spin_edit_text.h"
#define ZONE_MORTE		20.0
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelSpinEditText::PanelSpinEditText()
{
    PanelEditText();
    pCadran = new PanelSimple();
    pCadran->setBackground((char*)"images/cadran.tga");
    pBoule = new PanelSimple();
    pBoule->setBackground((char*)"images/boule.tga");

 	WindowsManager&     wm  = WindowsManager::getInstance();
    wm.add( pCadran );
    pCadran->add( pBoule );
    
    pCadran->setVisible( false );
    pCadran->setPosAndSize( 300, 300, 200, 200 );
    //pBoule->setVisible( false );
    pBoule->setPosAndSize( 55, 55, 20, 20 );

    delta_x = delta_y = 0;
    pVal = NULL;
    cb_motion = NULL;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelSpinEditText::set_enum( vector<float> t )
{
    t_val = t;
    nb = t.size();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelSpinEditText::boule_pos( int xm, int ym )
{
    VarManager&         var = VarManager::getInstance();
    if ( var.getb("bNuit") )    pCadran->setColor( 0xffff0000 );
    else                        pCadran->setColor( 0xffffffff );
    if ( var.getb("bNuit") )    pBoule->setColor( 0xffff0000 );
    else                        pBoule->setColor( 0xffffffff );


    vec2 vm = vec2( xm, ym );
    vec2 v = vm - vCentre;
    v.y *= -1.0;
    float norme = v.length();
    
    angle = RAD2DEG( acos( v.y / norme ) );
    if ( v.x <0.0 )         angle = 360.0 - angle;
    
    vec2 vBoule = v / norme * 45.0 + vec2( 100.0-10.0, -100.0+10.0 );
    vBoule.y *= -1;
    pBoule->setPos( vBoule.x, vBoule.y );

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelSpinEditText::compute_pos( int xm, int ym )
{
    VarManager&         var = VarManager::getInstance();
    if ( var.getb("bNuit") )    pCadran->setColor( 0xffff0000 );
    else                        pCadran->setColor( 0xffffffff );
    if ( var.getb("bNuit") )    pBoule->setColor( 0xffff0000 );
    else                        pBoule->setColor( 0xffffffff );


    vec2 ptm = vec2( xm, ym );
    vec2 v = ptm - vCentre;
    v.y *= -1.0;
    float norme = v.length();
    
    angle = RAD2DEG( acos( v.y / norme ) );
    if ( v.x <0.0 )         angle = 360.0 - angle;
    
    
    boule_pos(xm, ym);

    //--------------------------------------
    if ( t_val.size() == 0 )
    {
        float new_val = (max - min) / 360.0 * angle + min;
        val = new_val - fmod( new_val, step );
    }
    //--------------------------------------
    else
    {
        float new_val = nb / 360.0 * angle;
        int i = new_val;
        val = t_val[i];
    }    
    //--------------------------------------
    //logf( (char*)"PanelSpinEditText::compute_pos()  val = %0.2f", val ); 
    if ( pVal!= NULL )          *pVal = val;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelSpinEditText::compute_pos_relatif( int xm, int ym )
{
    VarManager&         var = VarManager::getInstance();
    //logf( (char*)"compute_pos_relatif()  val_anglel=%0.2f", val_angle ); 

    if ( var.getb("bNuit") )    pCadran->setColor( 0xffff0000 );
    else                        pCadran->setColor( 0xffffffff );
    if ( var.getb("bNuit") )    pBoule->setColor( 0xffff0000 );
    else                        pBoule->setColor( 0xffffffff );


    vec3 ptm = vec3( xm, ym, 0.0 );
    vec3 ptc = vec3( vCentre.x, vCentre.y, 0.0 );
    vec3 v  = ptm - ptc; 

    //logf( (char*)"compute_pos_relatif()  l=%0.2f", v.length() ); 

    if ( v.length() <= ZONE_MORTE )
    {
        vRef = v;
        vRef.normalize();
        return;
    }

    boule_pos(xm, ym);
    
    v.normalize();
    vec3 r;

    r.cross( vRef, v );
    
    float norm = r.length();
    float angle = RAD2DEG(asin( norm ));

    if ( r.z < 0.0 )    angle = -angle;
    
    //logf( (char*)"compute_pos_relatif()  (%0.2f, %0.2f, %0.2f)", r.x, r.y, r.z ); 
    //logf( (char*)"compute_pos_relatif()  angle=%0.2f", angle ); 

    vRef = v;

    //--------------------------------------
    if ( t_val.size() == 0 )
    {
        val_angle += (max - min) / (nb*360.0) * angle;
        //logf( (char*)"compute_pos_relatif()  val_angle=%0.2f", val_angle ); 

        if ( val_angle >= max )      val_angle = max;
        if ( val_angle <= min )      val_angle = min;

        val = val_angle - fmod( val_angle, step );     

        if ( val >= max )      val = max;

    }    
    //--------------------------------------
    else
    {
        val_angle += nb / 360.0 * angle;
        if ( val_angle <  0.0 )         val_angle = 0.0;
        if ( val_angle >= (nb-1 ))      val_angle = nb-1;
        int i = val_angle;
        val = t_val[i];
    }    
    //--------------------------------------
    //logf( (char*)"compute_pos_relatif()  angle=%0.2f norm=%0.2f val=%0.2f", val_angle, norm, val ); 
    if ( pVal!= NULL )          *pVal = val;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelSpinEditText::clickLeft( int xm, int ym )
{
    logf( (char*)"PanelSpinEditText::clickLeft(%d, %d)", xm, ym );
    log_tab(true);
    logf( (char*)"	delta (%d, %d)", delta_x, delta_y );
    x_click = xm; y_click = ym;
    pCadran->setPos( x_raw - 100 + delta_x, y_raw - 100 + delta_y );
    //pCadran->setPos( getPosX() - 100 + delta_x, getPosY() - 100 + delta_y );
    pCadran->updatePos();
    pCadran->setVisible( true );
    
    vRef = vec3( 0.0, 0.0, 0.0 );
    compute_pos_relatif( xm, ym );

 	WindowsManager&     wm  = WindowsManager::getInstance();
    wm.onTop(pCadran);

    if ( pVal!= NULL )
    {          
        logf( (char*)"  val=%0.2f *pVal=%0.2f", val, *pVal );
        val_angle = val = *pVal;
    }
    
    if ( click_left_cb != NULL )        (*click_left_cb)( xm, ym);

    log_tab(false);
}

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelSpinEditText::motionLeft( int xm, int ym )
{
    //logf( (char*)"PanelSpinEditText::motionLeft(%d, %d)", xm, ym );
    
    
    compute_pos_relatif( xm, ym );
    //logf( (char*)"  (%0.2f, %0.2f)", v.x, v.y );
    //logf( (char*)"  angle = %0.2f", angle );
    
    //logf( (char*)"  val = %0.2f", val );
    
    char s[50];
    sprintf( s, "%0.0f", val );
    changeText( (char*)s );

 	WindowsManager&     wm  = WindowsManager::getInstance();
    wm.onTop(pCadran);

	if ( cb_motion != NULL )			(*cb_motion)(xm, ym);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelSpinEditText::releaseLeft( int xm, int ym )
{
    logf( (char*)"PanelSpinEditText::releaseLeft(%d, %d)", xm, ym );
    pCadran->setVisible( false );

    if ( pVal!= NULL )          *pVal = val;
    
    if ( release_left_cb != NULL )        (*release_left_cb)( xm, ym);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelSpinEditText::clickRight( int xm, int ym )
{
    logf( (char*)"PanelSpinEditText::clickRight(%d, %d)", xm, ym );
    logf( (char*)"	delta (%d, %d)", delta_x, delta_y );
    x_click = xm; y_click = ym;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelSpinEditText::releaseRight( int xm, int ym )
{
    logf( (char*)"PanelSpinEditText::releaseRight(%d, %d)", xm, ym );
    
    set_delta( delta_x + (xm-x_click), delta_y + (ym-y_click) );
    logf( (char*)"	delta (%d, %d)", delta_x, delta_y );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelSpinEditText::updatePos()
{
    //logf( (char*)"PanelSpinEditText::updatePos()" );
    PanelEditText::updatePos();
    /*
    vCentre.x = (float)x_raw - (float)dx / 2.0 + (float)delta_x;
    vCentre.y = (float)y_raw - (float)dy / 2.0 + (float)delta_y;

    pCadran->setPos( x_raw - 100 + delta_x, y_raw - 100 + delta_y );
    
    vCentre.x = (float)x_raw;// - (float)dx / 2.0;
    vCentre.y = (float)y_raw;// - (float)dy / 2.0;

    */

    //vCentre = vec2( (float)(x_raw  + delta_x), (float)(y_raw  + delta_y) );
    vCentre = vec2( (float)(x_raw)+20, (float)(y_raw)+8 );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelSpinEditText::displayGL()
{
    PanelEditText::displayGL();

    if ( pCadran && pCadran->getVisible()==false )		return;

    //vCentre = vec2((float) delta_x, (float) delta_y);
	/*

	logf( (char*)"PanelSpinEditText::displayGL() centre (%0.2f,%0.2f)", vCentre.x, vCentre.y );

    glBegin(GL_LINES);
		//--------------------------------------------------------
		// graduation horizontale
		//--------------------------------------------------------
        vec4 color       = vec4( 0.2, 0.9, 0.9, 1.0 );    
		glColor4fv( (GLfloat*)&color );
		{
		    int x0 = vCentre.x-10;
		    int y0 = vCentre.y;
		    int x1 = vCentre.x+10;
		    int y1 = vCentre.y;

		    //xy2Screen(x0, y0);
		    //xy2Screen(x1, y1);
		    
		    glVertex2i( x0, y0 );
		    glVertex2i( x1, y1 );
		}
		//--------------------------------------------------------
		{
		    int x0 = vCentre.x;
		    int y0 = vCentre.y+10;
		    int x1 = vCentre.x;
		    int y1 = vCentre.y-10;

		    //xy2Screen(x0, y0);
		    //xy2Screen(x1, y1);
		    
		    glVertex2i( x0, y0 );
		    glVertex2i( x1, y1 );
		}
		//--------------------------------------------------------

	glEnd();
	*/
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

