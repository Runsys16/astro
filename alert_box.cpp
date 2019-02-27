#include "alert_box.h"


//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
AlertBox::AlertBox(string m)
{
    sMessage = m;

	pT = new PanelText( (char*)sMessage.c_str(),		PanelText::LARGE_FONT, 0, 0 );
	pT->setAlign( PanelText::CENTER );

	pRetour = new PanelText( (char*)"Appuyer sur n'importe quelle touche ...",		PanelText::NORMAL_FONT, 0, 20 );
	pRetour->setAlign( PanelText::CENTER );

    pW = new PanelWindow();
    
    int x, y, dx, dy, l0, l1, l;
	
	l0 = pT->getTextLenght();
	l1 = pRetour->getTextLenght();
	
	if ( l0 > l1 )          l = l0;
	else                    l = l1;
    
    dy = 40;
    dx = l + 80;

    x = (getWidth() -  dx) / 2;
    y = (getHeight() - dy) / 2;
    
    pW->setPosAndSize( x, y, dx, dy );
 	pW->setBackground((char*)"background.tga");
	pW->add( pT );
	pW->add( pRetour );

	WindowsManager::getInstance().add( pW );
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void AlertBox::quit()
{
	WindowsManager::getInstance().sup( pW );
	pW->sup(pT);
	
	delete pT;
	delete pW;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------



