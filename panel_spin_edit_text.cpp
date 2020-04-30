#include "panel_spin_edit_text.h"
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelSpinEditText::PanelSpinEditText()
{
    PanelEditText();
    pCadran = new PanelSimple();
    pCadran->setBackground((char*)"images/cadran.tga");

 	WindowsManager&     wm  = WindowsManager::getInstance();
    wm.add( pCadran );
    pCadran->setVisible( false );
    pCadran->setPosAndSize( 300, 300, 200, 200 );

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelSpinEditText::clickLeft( int xm, int ym )
{
    logf( (char*)"PanelSpinEditText::clickLeft(%d, %d)", xm, ym );
    pCadran->setVisible( true );
    pCadran->setPos( x_raw - 100, y_raw - 100 );

 	WindowsManager&     wm  = WindowsManager::getInstance();
    wm.onTop(pCadran);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelSpinEditText::motionLeft( int xm, int ym )
{
    logf( (char*)"PanelSpinEditText::motionLeft(%d, %d)", xm, ym );

    vec2 vm = vec2( xm, ym );
    vec2 v = vm - vCentre;
    v.y *= -1.0;
    
    float angle = RAD2DEG( acos( v.y / v.length() ) );
    if ( v.x <0.0 )         angle = 360.0 - angle;
    
    logf( (char*)"  (%0.2f, %0.2f)", v.x, v.y );
    logf( (char*)"  angle = %0.2f", angle );
    val = (max - min) / 360.0 * angle + min;
    logf( (char*)"  val = %0.2f", val );
    
    char s[50];
    sprintf( s, "%0.2f", val );
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
    vCentre.x = (float)x_raw - (float)dx / 2.0;
    vCentre.y = (float)y_raw - (float)dy / 2.0;
    vCentre.x = (float)x_raw;// - (float)dx / 2.0;
    vCentre.y = (float)y_raw;// - (float)dy / 2.0;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

