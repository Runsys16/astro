#include "panel_stdout.h"
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelStdOut::PanelStdOut()	{
	WindowsManager&     wm  = WindowsManager::getInstance();
    VarManager&         var = VarManager::getInstance();

	int x=10;
	int y=50;
	int dx=350;
	int dy=400;

    x = var.geti("xPanelStdOut");
    y = var.geti("yPanelStdOut");
    dx = var.geti("dxPanelStdOut");
    dy = var.geti("dyPanelStdOut");

    logf( (char*)"dy=%d", dy );
    if ( x<= 0 )        x = 10;
    if ( dx<= 10 )      dx = 350;
    if ( y<= 0 )        y = 10;
    if ( dy<= 10 )      dy = 400;

    setDisplayGL(displayGLnuit_cb);

	pScroll = new PanelScrollText(1400/13+1,100);
	//panelStdOut->setPrompt(string(">"));

	setPosAndSize( x, y, dx, dy );
 	setVisible(bPanelStdOut);

	pScroll->setPosAndSize( 0, 0, dx, dy );


 	wm.add( this );
 	
 	add( pScroll );

 	pScroll->setBackground((char*)"images/background.tga");
 	pScroll->setTabSize(20);

    string st = string("Bonjour\n");
    
    setExtraString( "panelStdOut" );
       
    logf((char*)"** CreateStdOut()  panelStdOut  %d,%d %dx%d", x, y, dx, dy);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelStdOut::displayGL(void)
{
    VarManager&         var = VarManager::getInstance();

    if ( var.getb("bNuit") )        glColor4f( 1.0, 0.0, 0.0, 1.0 );
    else                            glColor4f( 1.0, 1.0, 1.0, 1.0 );
    
    PanelWindow::displayGL();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelStdOut::updatePos(void)
{
    //cout << "PanelStdOut::updatePos DY="<< getPosDY() << endl;
    int h = pScroll->getHeightLine() + 3;
    if ( getPosDY() < h )              setSize(getPosDX(), h);
    PanelWindow::updatePos();
	pScroll->setSize( getDX(), getDY() );
	pScroll->updatePos();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelStdOut::setColor(unsigned long c)
{
    PanelWindow::setColor(c);
    pScroll->setColor(c);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelStdOut::affiche(char*pText)
{
    pScroll->affiche(pText);
}

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
