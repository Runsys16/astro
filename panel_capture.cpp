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
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

    float x = getParent()->getX();
    float y = getParent()->getY();

    x = x - x*echelle - 100;
    y = y - y*echelle - 100;

    glTranslatef( x, y, 0.0 );
    glScalef( echelle, echelle, 1.0 );
    
    PanelSimple::displayGL();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}





