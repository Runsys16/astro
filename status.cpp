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
PanelCheckBox *     pButtonRet;
PanelCheckBox *     pButtonMode;

PanelButton *       pFlecheHaut;
PanelButton *       pFlecheBas;

PanelButton *       pUrgentUp;
PanelButton *       pUrgentDown;

PanelCheckBox *     pButtonAsserv;

//--------------------------------------------------------------------------------------------------------------------
double  oldPas = -1.0;
double  old;
float   err_old = -1.0;
float   urg_old = -1.0;
bool    bFirst = true;
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void inverse_texture(PanelButton *, bool, string);
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void                logf( char *, ...);
void set_courbe(void)
{
    inverse_texture( pButtonCourbe,  bPanelCourbe,       "courbe" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void set_asservissement(void)
{
    var.set("bCorrection", bCorrection);
    fTimeCpt = 0.0; 

    var.set("vOrigine.x", panelCourbe->get_vOrigine().x);
    var.set("vOrigine.y", panelCourbe->get_vOrigine().y);

    if (bCorrection)            pAsservi->changeText((char*)"Asservissemnent");
    else                        pAsservi->changeText((char*)" ");
   
    pButtonAsserv->setVal(bCorrection);
    
    logf( (char*)"asservissement = %s", bCorrection?(char*)"true":(char*)"false" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void set_mode(void)
{
    var.set("bModeManuel", bModeManuel);

    if (!bModeManuel)               pMode->changeText((char*)"Mode suivi");
    else                            pMode->changeText((char*)"Mode souris");
   
    pButtonMode->setVal(bModeManuel);
    
    logf( (char*)"status::set_mode() bModeManuel = %s", bModeManuel?(char*)"true":(char*)"false" );
}
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
        pButton->setDown( (char*)down.c_str() );
        pButton->setOver( (char*)down.c_str() );
    }
    else
    {
        pButton->setUp(   (char*)down.c_str() );
        pButton->setDown( (char*)over.c_str() );
        pButton->setOver( (char*)over.c_str() );
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void cb_fleche(PanelButton* pPanel)
{
    char s[20];
    float err = panelCourbe->get_err();
    
    if ( pPanel == pFlecheHaut )
    {
        err /= 0.9;
        panelCourbe->set_err( err );
        var.set( "err", err );

        sprintf( s, "+%0.2f", err );
        panelCourbe->get_pXMax()->changeText( (char*)s );
        panelCourbe->get_pYMax()->changeText( (char*)s );

        sprintf( s, "-%0.2f", err );
        panelCourbe->get_pXMin()->changeText( (char*)s );
        panelCourbe->get_pYMin()->changeText( (char*)s );
    }
    else
    if ( pPanel == pFlecheBas )
    {
        err *= 0.9;
        panelCourbe->set_err( err );
        var.set( "err", err );

        sprintf( s, "+%0.2f", err );
        panelCourbe->get_pXMax()->changeText( (char*)s );
        panelCourbe->get_pYMax()->changeText( (char*)s );

        sprintf( s, "-%0.2f", err );
        panelCourbe->get_pXMin()->changeText( (char*)s );
        panelCourbe->get_pYMin()->changeText( (char*)s );
    }
    else
    if ( pPanel == pUrgentUp )
    {
		fLimitCorrection *= 1.1f;
        var.set("fLimitCorrection", (float)fLimitCorrection);
    }
    else
    if ( pPanel == pUrgentDown )
    {
		fLimitCorrection *= 0.9f;
        var.set("fLimitCorrection", (float)fLimitCorrection);
    }

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
        panelStdOut->setVisible(bPanelStdOut);
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
        sprintf( cmd, "sa;g" );
        Serial::getInstance().write_string(cmd);
    }
    else if (  p == pButtonDec )
    {
        char cmd[255];
        sprintf( cmd, "sd;g" );
        Serial::getInstance().write_string(cmd);
    }
    else if (  p == pButtonJoy )
    {
        char cmd[255];
        sprintf( cmd, "j;g" );
        Serial::getInstance().write_string(cmd);
    }
    else if (  p == pButtonSui )
    {
        char cmd[255];
        sprintf( cmd, "S;g" );
        Serial::getInstance().write_string(cmd);
    }
    else if (  p == pButtonRet )
    {
        char cmd[255];
        sprintf( cmd, "p;g" );
        Serial::getInstance().write_string(cmd);
    }
	else
	if ( p == pButtonAsserv )
	{
        bCorrection = p->getVal(); 
        logf( (char*)"Appui sur cadena !! %s %s", BOOL2STR(bCorrection), BOOL2STR(pButtonAsserv->getVal())  );
        set_asservissement();
    }
	else
	if ( p == pButtonMode )
	{
    	bModeManuel = !bModeManuel;
        var.set("bModeManuel", bModeManuel);
        set_mode();
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

    pCheckBox->setBackground( (char*)NULL );
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

    pButton->setUp(   (char*)down.c_str() );
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
void create_fleches( int x, char* up, char* down, PanelButton* &pUp, PanelButton* &pDown )
{
    pUp = new PanelButton();
    panelStatus->add(pUp);
    
    pUp->setPosAndSize( x, 2, 10, 8 );

    pUp->setUp(   up );
    pUp->setDown( up );
    pUp->setOver( up );

	pUp->setCallBackUp(   cb_fleche );


    pDown = new PanelButton();
    panelStatus->add(pDown);

    pDown->setPosAndSize( x, 2+8, 10, 8 );

    pDown->setUp(   down );
    pDown->setDown( down );
    pDown->setOver( down );

	pDown->setCallBackUp(   cb_fleche );
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

    pButtonMode     = create_window_check_box( 7, "cible" );
    pButtonAsserv   = create_window_check_box( 8, "cadena" );
    pButtonAsc      = create_window_check_box( 10, "asc" );
    pButtonDec      = create_window_check_box( 11, "dec" );
    pButtonJoy      = create_window_check_box( 12, "joy" );
    pButtonSui      = create_window_check_box( 13, "terre" );
    pButtonRet      = create_window_check_box( 14, "retour" );
    
    create_fleches( 730, (char*)"images/fleche_haut.tga", (char*)"images/fleche_bas.tga", pFlecheHaut, pFlecheBas);
    create_fleches( 780, (char*)"images/fleche_haut.tga", (char*)"images/fleche_bas.tga", pUrgentUp, pUrgentDown);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void idleStatus()
{
    //-----------------------------------------------------------------------
    // Gestion des connexions logiciel /dev/:
    // arduino et stellarium
    //-----------------------------------------------------------------------
    if ( Serveur_mgr::getInstance().isConnect() )
    {
       bStellarium = true;
       pStellarium->changeText( (char*)"Stellarium" );
    }
    else
    {
       bStellarium = false;
       pStellarium->changeText( (char*)"----" );
    }
    
    if ( pas_sideral != oldPas )
    {
        char str[255];
        sprintf( str, "pas:%0.4f", pas_sideral );
        pPas->changeText( (char*)str );
        oldPas = pas_sideral;
    }
    //-----------------------------------------------------------------------
    // Gestion de apause
    //-----------------------------------------------------------------------
    //if (!bPause || bOneFrame )    {
        Camera_mgr::getInstance().change_background_camera();
        if (bOneFrame)      { updatePanelPause(true); }
        bOneFrame = false;
    //}
    /*
    //change_joy( xSuivi, ySuivi );
    //-----------------------------------------------------------------------
    // Gestion de apause
    //-----------------------------------------------------------------------
    if (bModeManuel || bOneFrame )    {
        Camera_mgr::getInstance().change_background_camera();
        if (bOneFrame)      { updatePanelPause(true); }
        bOneFrame = false;
    }
    */

    if (err_old != panelCourbe->get_err() )
    {
        err_old = panelCourbe->get_err();
        char str[255];
        sprintf( str, "%0.2f", err_old );
        pErr->changeText( (char*)str );
    }

    if (urg_old != fLimitCorrection )
    {
        urg_old = fLimitCorrection;
        char str[255];
        sprintf( str, "%0.2f", urg_old );
        pUrg->changeText( (char*)str );
    }

    
    if ( bCorrection != pButtonAsserv->getVal() )   set_asservissement();
    if ( bModeManuel != pButtonMode->getVal() )     set_mode();

    if ( bFirst )
    {
        logf( (char*)"[WARNING]Change mode " );
        bFirst = false,
        set_mode();
    }

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

