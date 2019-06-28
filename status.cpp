PanelButton *       pButtonSerial;
PanelButton *       pButtonStdOut;
PanelButton *       pButtonControl;
PanelButton *       pButtonHelp;
PanelButton *       pButtonResultat;
PanelButton *       pButtonCourbe;
PanelCheckBox *     pButtonAsc;
PanelCheckBox *     pButtonDec;
PanelCheckBox *     pButtonJoy;
PanelCheckBox *     pButtonSui;

PanelButton *       pFlecheHaut;
PanelButton *       pFlecheBas;

PanelButton *       pButtonMode;
PanelButton *       pButtonAsserv;


//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void call_back_over(PanelButton* pPanel)	{
	//cout << "Button CallBack Over()" << endl;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void inverse_texture(PanelButton * pButton, bool b, string tex )
{
    
    if ( tex.length() == 0 )    tex = "window";
        
    string down = "images/" + tex + "_down.tga";
    string over = "images/" + tex + "_over.tga";
    
    logf( (char*)"inverse_texture() , %d \"%s\"", (int) b, tex.c_str() );
    if ( !b )
    {
        pButton->setUp(   (char*)over.c_str() );
        pButton->setDown( (char*)over.c_str() );
        pButton->setOver( (char*)over.c_str() );
    }
    else
    {
        pButton->setUp(   (char*)down.c_str() );
        pButton->setDown( (char*)down.c_str() );
        pButton->setOver( (char*)down.c_str() );
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void cb_up_fleche_haut(PanelButton* pPanel)
{
    char s[20];
    if ( pPanel == pFlecheHaut )
    {
        err /= 0.9;
    }
    else
    {
        err *= 0.9;
    }

    var.set( "err", err );

    sprintf( s, "+%0.2f", err );
    pXMax->changeText( (char*)s );
    pYMax->changeText( (char*)s );

    sprintf( s, "-%0.2f", err );
    pXMin->changeText( (char*)s );
    pYMin->changeText( (char*)s );

	logf( (char*)"Button fleche up() : err=%0.2f", err );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void call_back_up(PanelButton* pPanel)
{
	logf( (char*) "Button CallBack up()" );
	
	if ( pPanel == pButtonSerial )
	{
        bPanelSerial = !bPanelSerial;
        var.set("bPanelSerial", bPanelSerial);
        PanelConsoleSerial::getInstance().setVisible( bPanelSerial );
        logf( (char*)"Toggle serial !!!" );

        inverse_texture( pPanel, bPanelSerial, "arduino" );
	}
	else
	if ( pPanel == pButtonStdOut )
	{
        bPanelStdOut = !bPanelStdOut;
        var.set("bPanelStdOut", bPanelStdOut);
        panelStdOutW->setVisible(bPanelStdOut);
        logf( (char*)"Toggle panelStdOut !!!" );

        inverse_texture( pPanel, bPanelStdOut, "" );
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
            inverse_texture( pButtonControl, b, "camera" );
        }
	}
	else
	if ( pPanel == pButtonHelp )
	{
        bPanelHelp = !bPanelHelp;
        var.set("bPanelHelp", bPanelHelp);
        panelHelp->setVisible(bPanelHelp);
        log( (char*)"Toggle panelHelp !!!" );

        inverse_texture( pPanel, bPanelHelp, "help" );
	}
	else
	if ( pPanel == pButtonResultat )
	{
        bPanelResultat = !bPanelResultat;
        var.set("bPanelResultat", bPanelResultat);
        panelResultat->setVisible(bPanelResultat);
        log( (char*)"Toggle panelResultat !!!" );

        inverse_texture( pPanel, bPanelResultat, "cible" );
	}
	else
	if ( pPanel == pButtonCourbe )
	{
        bPanelCourbe = !bPanelCourbe;
        var.set("bPanelCourbe", bPanelCourbe);
        panelCourbe->setVisible(bPanelCourbe);
        log( (char*)"Toggle panelCourbe !!!" );

        inverse_texture( pPanel, bPanelCourbe, "courbe" );
	}
	else
	if ( pPanel == pButtonMode )
	{
    	bAutorisationSuivi = !bAutorisationSuivi;
        var.set("bAutorisationSuivi", bAutorisationSuivi);

        if (bAutorisationSuivi)         pMode->changeText((char*)"Mode suivi");
        else                            pMode->changeText((char*)"Mode souris");

        inverse_texture( pPanel, bAutorisationSuivi, "cible" );

	}
	else
	if ( pPanel == pButtonAsserv )
	{
        bCorrection = !bCorrection; 
        var.set("bCorrection", bCorrection);
        fTimeCpt = 0.0; 
        /*
        vOrigine.x = xSuivi;
        vOrigine.y = ySuivi;
        vOrigine.z = 0.0;
        */

        var.set("vOrigine.x", vOrigine.x);
        var.set("vOrigine.y", vOrigine.y);

        if (bCorrection)            pAsservi->changeText((char*)"Asservissemnent");
        else                        pAsservi->changeText((char*)" ");
        
        inverse_texture( pPanel, bCorrection, "cadena" );
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void call_back_down(PanelButton* pPanel)	{
	//cout << "Button CallBack down()" << endl;
	//pCheck->setVal( !pCheck->getVal() );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void cb_rotationCheck(PanelCheckBox* p)	{
    if (  p == pButtonAsc )
    {
        char cmd[255];
        sprintf( cmd, "sa" );
        Serial::getInstance().write_string(cmd);
    }
    else if (  p == pButtonDec )
    {
        char cmd[255];
        sprintf( cmd, "sd" );
        Serial::getInstance().write_string(cmd);
    }
    else if (  p == pButtonJoy )
    {
        char cmd[255];
        sprintf( cmd, "j" );
        Serial::getInstance().write_string(cmd);
    }
    else if (  p == pButtonSui )
    {
        char cmd[255];
        sprintf( cmd, "S" );
        Serial::getInstance().write_string(cmd);
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelCheckBox* create_window_check_box( int i, string tex)
{
    if ( tex.length() == 0 )    return NULL;
        
    string strue = "images/" + tex + "_down.tga";
    string sfalse = "images/" + tex + "_over.tga";

    PanelCheckBox* pCheckBox = new PanelCheckBox();
    pCheckBox->setTrue( (char*)strue.c_str() );
    pCheckBox->setFalse( (char*)sfalse.c_str() );

    pCheckBox->setCallBackMouse( cb_rotationCheck );

    pCheckBox->setBackground( NULL );
    pCheckBox->setPosAndSize( 450+ i*18, 2, 16, 16 );

    panelStatus->add(pCheckBox);
    return pCheckBox;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelButton* create_window_button( int i, string tex)
{
    PanelButton * pButton = new PanelButton();
    panelStatus->add(pButton);
    
    pButton->setPosAndSize( 450+ i*18, 2, 16, 16 );
    
    
    if ( tex.length() == 0 )    tex = "window";
        
    string down = "images/" + tex + "_down.tga";
    string over = "images/" + tex + "_over.tga";

    pButton->setUp(   (char*)over.c_str() );
    pButton->setDown( (char*)over.c_str() );
    pButton->setOver( (char*)over.c_str() );

	pButton->setCallBackOver( call_back_over );
	pButton->setCallBackUp( call_back_up );
	pButton->setCallBackDown( call_back_down );
	
	return pButton;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void create_fleches()
{
    pFlecheHaut = new PanelButton();
    panelStatus->add(pFlecheHaut);
    
    pFlecheHaut->setPosAndSize( 800, 2, 16, 8 );

    pFlecheHaut->setUp(   (char*)"images/fleche_haut.tga" );
    pFlecheHaut->setDown( (char*)"images/fleche_haut.tga" );
    pFlecheHaut->setOver( (char*)"images/fleche_haut.tga" );

	pFlecheHaut->setCallBackUp(   cb_up_fleche_haut );


    pFlecheBas = new PanelButton();
    panelStatus->add(pFlecheBas);

    pFlecheBas->setPosAndSize( 800, 2+8, 16, 8 );

    pFlecheBas->setUp(   (char*)"images/fleche_bas.tga" );
    pFlecheBas->setDown( (char*)"images/fleche_bas.tga" );
    pFlecheBas->setOver( (char*)"images/fleche_bas.tga" );

	pFlecheBas->setCallBackUp(   cb_up_fleche_haut );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void create_windows_button()
{
    pButtonControl  = create_window_button( 0, "camera" );
    pButtonHelp     = create_window_button( 1, "help" );
    pButtonResultat = create_window_button( 2, "cible" );
    pButtonCourbe   = create_window_button( 3, "courbe" );
    pButtonStdOut   = create_window_button( 4, "" );
    pButtonSerial   = create_window_button( 5, "arduino" );

    pButtonMode     = create_window_button( 7, "cible" );
    pButtonAsserv   = create_window_button( 8, "cadena" );
    
    pButtonAsc      = create_window_check_box( 10, "asc" );
    pButtonDec      = create_window_check_box( 11, "dec" );
    pButtonJoy      = create_window_check_box( 12, "joy" );
    pButtonSui      = create_window_check_box( 13, "terre" );
    
    create_fleches();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

