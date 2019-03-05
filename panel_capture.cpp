#include "panel_capture.h"

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelCapture::PanelCapture()
{
    echelle = 1.0;
    dx      = 0.0;
    dy      = 0.0;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::displayGL()
{
    //mat4 m = scale( 2.0, 2.0, 1.0 );
    float gris = 0.3;
    VarManager& var = VarManager::getInstance();
    if ( var.getb("bNuit") )        glColor4f( 0.5, 0.0, 0.0, 1.0 );    
    else                            glColor4f( gris, gris, gris, 0.2 );

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

    float x = getParent()->getX();
    float y = getParent()->getY();

    x = x - x*echelle + dx;
    y = y - y*echelle + dy;

    glTranslatef( x, y, 0.0 );
    glScalef( echelle, echelle, 1.0 );

    glColor4f( gris, gris, gris, 0.2 );    
    PanelSimple::displayGL();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}





