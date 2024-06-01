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
    pCadran->setPosAndSize( -100+20, -100+8, 200, 200 );

    pBoule = new PanelSimple();
    pBoule->setBackground((char*)"images/boule.tga");
    pBoule->setPosAndSize( 55, 55, 20, 20 );

	pEditCopy = new PanelEditText();
	pEditCopy->setPos( 100-20, 100-8 );
	
 	WindowsManager&     wm  = WindowsManager::getInstance();
    pCadran->add( pBoule );
    pCadran->add( pEditCopy );
    wm.add( pCadran );
    

	pCadran->setVisible( false );
	pCadran->setScissor( false );

    delta_x = delta_y = nDecimal = 0;
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
    vec3 ptm = vec3( xm, ym, 0.0 );
    vec3 ptc = vec3( vCentre.x, vCentre.y, 0.0 );
    vec3 v  = ptm - ptc; 

    //logf( (char*)"compute_pos_relatif()  l=%0.2f", v.length() ); 



    if ( v.length() <= ZONE_MORTE )		{
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
void PanelSpinEditText::ajusteDelta( int ws, int hs )
{
    int dx = pCadran->getDX();
    int dy = pCadran->getDY();
    int dx2 = dx/2;
    int dy2 = dy/2;

	delta_x = 0;
	delta_y = 0;
	
	int X=20, Y=8;
	
	//delta_x = delta_y = 0;
	
	if ( (x_raw+dx2) > ws )	{
		X += -dx2;
	}
	if ( (x_raw-dx2) < 0 )	{
		X += +dx2;
	}
	if ( (y_raw+dy2) > hs )	{
		Y += -dy2;
	}
	if ( (y_raw-dy2) < 0 )	{
		Y += +dy2;
	}
	
	delta_x += X;
	delta_y += Y;
	
	pCadran->setPos( getX() + delta_x - dx2, getY() + delta_y - dy2 );
	pCadran->updatePos();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelSpinEditText::clampVal()
{
    if ( val > max )        val = max;
    if ( val < min )        val = min;
    if ( pVal != NULL )     *pVal = val;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelSpinEditText::clickLeft( int xm, int ym )
{
    logf( (char*)"PanelSpinEditText::clickLeft(%d, %d)", xm, ym );

    log_tab(true);
    logf( (char*)"delta old (%d, %d)", delta_x, delta_y );

	setVisible( false );
    pCadran->setVisible( true );
    pCadran->setCanMove(false);


 	WindowsManager&     wm  = WindowsManager::getInstance();
	int ws = wm.getWidth();
	int hs = wm.getHeight();
    int dx2 = pCadran->getDX() / 2;
    int dy2 = pCadran->getDY() / 2;
    int xScreen = getX();
    int yScreen = getY();
    x_click = getPosX();
    y_click = getPosY();
    
    char s[20];
    strncpy( s, getText().c_str(), sizeof(s)-1 );
    pEditCopy->changeText( (char*)s );

    
    logf( (char*)"position (%d, %d)", xScreen, yScreen );
    pCadran->setPos(xScreen, yScreen);
    pCadran->updatePos();
    
	ajusteDelta( ws, hs );
    logf( (char*)"delta new (%d, %d)", delta_x, delta_y );
	
    logf( (char*)"pos_raw old (%d, %d)", pEditCopy->getX(), pEditCopy->getY() );
    
    logf( (char*)"cadran (%d, %d)  boule (%d,%d)", pCadran->getX(), pCadran->getY(), pBoule->getX(), pBoule->getY() );
    
    compute_pos_relatif( xm, ym );
    clampVal();

    wm.onTop(pCadran);
    /*
    if ( pVal!= NULL )
    {          
        val_angle = val = *pVal;
    }
    */
    
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
    clampVal();
    //logf( (char*)"  (%0.2f, %0.2f)", v.x, v.y );
    //logf( (char*)"  angle = %0.2f", angle );
    
    //logf( (char*)"  val = %0.2f", val );
    
    char s[50];
    switch( nDecimal )
    {
        case 0:
            sprintf( s, "%0.0f", val );
            break;
        case 1 :
            sprintf( s, "%0.1f", val );
            break;
        case 2:
            sprintf( s, "%0.2f", val );
            break;
        default:
            sprintf( s, "%0.0f", val );
            break;
    }
    //logf( (char*)"val=%0.2f max=%0.2f", val, max );
    changeText( (char*)s );
    pEditCopy->changeText( (char*)s );

 	WindowsManager&     wm  = WindowsManager::getInstance();
    wm.onTop(pEditCopy);

	if ( cb_motion != NULL )			(*cb_motion)(xm, ym);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelSpinEditText::releaseLeft( int xm, int ym )
{
    logf( (char*)"PanelSpinEditText::releaseLeft(%d, %d)", xm, ym );
    setVisible( true );
    pCadran->setVisible( false );
    pCadran->setCanMove(true);

    clampVal();
    
    if ( pVal!= NULL )          		*pVal = val;
    if ( release_left_cb != NULL )		(*release_left_cb)( xm, ym);
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
    
    logf( (char*)"	delta (%d, %d)", delta_x, delta_y );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelSpinEditText::updatePos()
{
    //logf( (char*)"PanelSpinEditText::updatePos()" );
    PanelEditText::updatePos();

    int dx2 = pCadran->getDX() / 2;
    int dy2 = pCadran->getDY() / 2;

    vCentre = vec2( (float)(pCadran->getX() + dx2), (float)(pCadran->getY() + dy2) );
    //vCentre = vec2( (float)(x_raw)+20, (float)(y_raw)+8 );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelSpinEditText::idle(float f)
{
    //log( (char*)"PanelSpinEditText::idle()" ); 
    VarManager&         var = VarManager::getInstance();
	long color;

    if ( var.getb("bNuit") )        color = 0xffff0000;
    else                            color = 0xffffffff;

    setColor( color );
    pCadran->setColor( color );
    pBoule->setColor( color );
    pEditCopy->setColor( color );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
//#define DBUG_CENTRE
void PanelSpinEditText::displayGL()
{
    PanelEditText::displayGL();

    if ( pEditCopy && pEditCopy->getVisible()==false )		return;

#ifdef DBUG_CENTRE
    //logf( (char*)"cadran (%d, %d)  boule (%d,%d)", pCadran->getX(), pCadran->getY(), pBoule->getX(), pBoule->getY() );


    glBegin(GL_LINES);
		//--------------------------------------------------------
		// graduation horizontale
		//--------------------------------------------------------
        vec4 color       = vec4( 0.5, 0.5, 0.9, 1.0 );    
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
#endif
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

