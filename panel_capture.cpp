#include "panel_capture.h"

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelCapture::PanelCapture()
{
    echelle = 1.0;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::displayGL()
{
    //mat4 m = scale( 2.0, 2.0, 1.0 );
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

    float x = getParent()->getX();
    float y = getParent()->getY();

    
    glScalef( echelle, echelle, 1.0 );
    
    x = x - x*echelle;
    y = y - y*echelle;

    glTranslatef( x, y, 0.0 );
    
    PanelSimple::displayGL();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}





