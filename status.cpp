PanelButton *       pButtonSerial;
PanelButton *       pButtonStdOut;
PanelButton *       pButtonControl;
PanelButton *       pButtonHelp;
PanelButton *       pButtonResultat;
PanelButton *       pButtonCourbe;

PanelButton *       pButtonMode;
PanelButton *       pButtonAsserv;


void call_back_over(PanelButton* pPanel)	{
	//cout << "Button CallBack Over()" << endl;
}


void inverse_texture(PanelButton * pButton, bool b )
{
    logf( (char*)"inverse_texture()" );
    if ( !b )
    {
        pButton->setUp( (char*) "window_over.tga" );
        pButton->setDown( (char*) "window_over.tga" );
        pButton->setOver( (char*) "window_down.tga" );
    }
    else
    {
	    pButton->setUp( (char*) "window_down.tga" );
	    pButton->setDown( (char*) "window_down.tga" );
	    pButton->setOver( (char*) "window_over.tga" );
    }
}



void call_back_up(PanelButton* pPanel)	{
	cout << "Button CallBack up()" << endl;
	
	if ( pPanel == pButtonSerial )
	{
        bPanelSerial = !bPanelSerial;
        PanelConsoleSerial::getInstance().setVisible( bPanelSerial );
        logf( (char*)"Toggle serial !!!" );

        inverse_texture( pPanel, bPanelSerial );
	}
	else
	if ( pPanel == pButtonStdOut )
	{
        bPanelStdOut = !bPanelStdOut;
        panelStdOutW->setVisible(bPanelStdOut);
        logf( (char*)"Toggle panelStdOut !!!" );

        inverse_texture( pPanel, bPanelStdOut );
	}
	else
	if ( pPanel == pButtonControl )
	{
        Camera_mgr::getInstance().togglePanel();
        log( (char*)"Toggle panelCamera !!!" );

        Camera * p = Camera_mgr::getInstance().getCurrent();
        if ( p )
        {
            bool b = p->getControlVisible();
            inverse_texture( pButtonControl, b );
        }
	}
	else
	if ( pPanel == pButtonHelp )
	{
        bPanelHelp = !bPanelHelp;
        panelHelp->setVisible(bPanelHelp);
        log( (char*)"Toggle panelHelp !!!" );

        inverse_texture( pPanel, bPanelHelp );
	}
	else
	if ( pPanel == pButtonResultat )
	{
        bPanelResultat = !bPanelResultat;
        panelResultat->setVisible(bPanelResultat);
        log( (char*)"Toggle panelResultat !!!" );

        inverse_texture( pPanel, bPanelResultat );
	}
	else
	if ( pPanel == pButtonCourbe )
	{
        bPanelCourbe = !bPanelCourbe;
        panelCourbe->setVisible(bPanelCourbe);
        log( (char*)"Toggle panelCourbe !!!" );

        inverse_texture( pPanel, bPanelCourbe );
	}
	else
	if ( pPanel == pButtonMode )
	{
    	bAutorisationSuivi = !bAutorisationSuivi;

        if (bAutorisationSuivi)         pMode->changeText((char*)"Mode suivi");
        else                            pMode->changeText((char*)"Mode souris");

        inverse_texture( pPanel, bAutorisationSuivi );

	}
	else
	if ( pPanel == pButtonAsserv )
	{
        bCorrection = !bCorrection; 
        fTimeCorrection = 0.0; 
        vOrigine.x = xSuivi;
        vOrigine.y = ySuivi;
        vOrigine.z = 0.0;
        if (bCorrection)            pAsservi->changeText((char*)"Asservissemnent");
        else                        pAsservi->changeText((char*)" ");
        
        inverse_texture( pPanel, bCorrection );
	}
}

void call_back_down(PanelButton* pPanel)	{
	//cout << "Button CallBack down()" << endl;
	//pCheck->setVal( !pCheck->getVal() );
}


PanelButton* create_window_button( PanelButton* pButton, int i)
{
    pButton = new PanelButton();
    panelStatus->add(pButton);
    
    pButton->setPosAndSize( 800+ i*18, 2, 16, 16 );
	pButton ->setUp( (char*) "window_over.tga" );
	pButton ->setDown( (char*) "window_over.tga" );
	pButton ->setOver( (char*) "window_down.tga" );

	pButton->setCallBackOver( call_back_over );
	pButton->setCallBackUp( call_back_up );
	pButton->setCallBackDown( call_back_down );
	
	return pButton;
}


void create_windows_button()
{
    pButtonControl  = create_window_button( pButtonControl,  0 );
    pButtonHelp     = create_window_button( pButtonHelp,     1 );
    pButtonResultat = create_window_button( pButtonResultat, 2 );
    pButtonCourbe   = create_window_button( pButtonCourbe,   3 );
    pButtonStdOut   = create_window_button( pButtonStdOut,   4 );
    pButtonSerial   = create_window_button( pButtonSerial,   5 );

    pButtonMode     = create_window_button( pButtonMode,     7 );
    pButtonAsserv   = create_window_button( pButtonAsserv,   8 );
}



