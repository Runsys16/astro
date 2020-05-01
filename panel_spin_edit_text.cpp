#include "panel_spin_edit_text.h"
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
void PanelSpinEditText::clickLeft( int xm, int ym )
{
    logf( (char*)"PanelSpinEditText::clickLeft(%d, %d)", xm, ym );
    pCadran->setPos( x_raw - 100 + delta_x, y_raw - 100 + delta_y );
    pCadran->updatePos();
    pCadran->setVisible( true );
    compute_pos( xm, ym );

 	WindowsManager&     wm  = WindowsManager::getInstance();
    wm.onTop(pCadran);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelSpinEditText::motionLeft( int xm, int ym )
{
    //logf( (char*)"PanelSpinEditText::motionLeft(%d, %d)", xm, ym );

    compute_pos( xm, ym );
    //logf( (char*)"  (%0.2f, %0.2f)", v.x, v.y );
    //logf( (char*)"  angle = %0.2f", angle );
    float new_val = (max - min) / 360.0 * angle + min;

    val = new_val - fmod( new_val, step );
    
    //logf( (char*)"  val = %0.2f", val );
    
    char s[50];
    sprintf( s, "%0.0f", val );
    changeText( (char*)s );

 	WindowsManager&     wm  = WindowsManager::getInstance();
    wm.onTop(pCadran);

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelSpinEditText::releaseLeft( int xm, int ym )
{
    logf( (char*)"PanelSpinEditText::releaseLeft(%d, %d)", xm, ym );
    pCadran->setVisible( false );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelSpinEditText::updatePos()
{
    //logf( (char*)"PanelSpinEditText::updatePos()" );
    PanelEditText::updatePos();
    vCentre.x = (float)x_raw - (float)dx / 2.0 + (float)delta_x;
    vCentre.y = (float)y_raw - (float)dy / 2.0 + (float)delta_y;
    vCentre.x = (float)x_raw;// - (float)dx / 2.0;
    vCentre.y = (float)y_raw;// - (float)dy / 2.0;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

