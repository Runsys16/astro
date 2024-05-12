class PanelButtonAsservissement : public PanelButton
{
public:
    virtual void		wheelUp( int, int);
    virtual void		wheelDown( int, int);
};

class PanelTextAsservissement : public PanelText
{
public:
    virtual void		wheelUp( int, int);
    virtual void		wheelDown( int, int);
};


//PanelCheckBox *     pButtonMode;
PanelCheckBox *     pButtonSerial;
PanelCheckBox *     pButtonStdOut;
PanelCheckBox *     pButtonControl;
PanelCheckBox *     pButtonResultat;
PanelCheckBox *     pButtonCourbe;
PanelCheckBox *     pButtonHelp;
PanelCheckBox *     pButtonAsc;
PanelCheckBox *     pButtonDec;
PanelCheckBox *     pButtonJoy;
PanelCheckBox *     pButtonSui;
PanelCheckBox *     pButtonRet;
PanelCheckBox *     pButtonMode;
PanelCheckBox *     pButtonSon;
PanelCheckBox *     pButtonPause;
PanelCheckBox *     pButtonDeplacement;
PanelSpinEditText*  pCercleAsserv;
PanelSpinEditText*  pCercleErr;

PanelButtonAsservissement *       pFlecheHaut;
PanelButtonAsservissement *       pFlecheBas;

PanelButtonAsservissement *       pUrgentUp;
PanelButtonAsservissement *       pUrgentDown;

PanelText*          pErr;
PanelText*          pUrg;
PanelTextAsservissement*          pErrA;
PanelTextAsservissement*          pUrgA;

PanelCheckBox *     pButtonAsserv;

//--------------------------------------------------------------------------------------------------------------------
double  oldPas = -1.0;
double  old;
float   err_old = -1.0;
float   urg_old = -1.0;
bool    bFirstStatus = true;
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
    //inverse_texture( pButtonCourbe,  bPanelCourbe,       "courbe" );
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

    //if (bCorrection)            pAsservi->changeText((char*)"Asservissemnent");
    //else                        pAsservi->changeText((char*)" ");

    //pAsservi->changeText((char*)"Asservissemnent");

    if (bCorrection)            pAsservi->setColor(0xFFFFFFFF);
    else                        pAsservi->setColor(0XFF404040);
   
    pButtonAsserv->setVal(bCorrection);
    
    logf( (char*)"set_asservissement() bCorrection = %s", bCorrection?(char*)"true":(char*)"false" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void set_mode(bool b)
{
    var.set("bModeManuel", b);
    bModeManuel = b;

    if (!bModeManuel)               bMouseDeplace = false;
   
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
    
    //logf( (char*)"inverse_texture() , %d \"%s\"", (int) b, tex.c_str() );
    if ( b )
    {
        pButton->setUp(   (char*)down.c_str() );
        pButton->setDown( (char*)down.c_str() );
        pButton->setOver( (char*)down.c_str() );
    }
    else
    {
        pButton->setUp(   (char*)over.c_str() );
        pButton->setDown( (char*)over.c_str() );
        pButton->setOver( (char*)over.c_str() );
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelTextAsservissement::wheelUp( int, int)
{
    char s[20];
    float err = panelCourbe->get_err();
    log((char*)"PanelTextAsservissement::wheelUp()" );
    
    if ( this == pUrgA  )
    {
	    fLimitCorrection /= 0.9f;
        var.set("fLimitCorrection", (float)fLimitCorrection);
        logf( (char*) "fLimitCorrection = %0.2f", (float)fLimitCorrection );
    }
    else if ( this == pErrA )
    {
        err /= 0.9;
        panelCourbe->set_err( err );
        var.set( "err", err );

        sprintf( s, "+%0.2f", err );
        panelCourbe->get_pXMax()->changeText( (char*)s );
        panelCourbe->get_pYMax()->changeText( (char*)s );
        sprintf( s, "%d", (int)err );
        pErrA->changeText( (char*)s );
        pCercleErr->set_val( err );


        sprintf( s, "-%0.2f", err );
        panelCourbe->get_pXMin()->changeText( (char*)s );
        panelCourbe->get_pYMin()->changeText( (char*)s );
        logf( (char*) "err = %0.2f", (float)err );

    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelTextAsservissement::wheelDown( int, int)
{
    char s[20];
    float err = panelCourbe->get_err();
    log((char*)"PanelTextAsservissement::wheelDown()" );
    
    if ( this == pUrgA )
    {
	    fLimitCorrection *= 0.9f;
        var.set("fLimitCorrection", (float)fLimitCorrection);
        logf( (char*) "fLimitCorrection = %0.2f", (float)fLimitCorrection );
    }
    else if ( this == pErrA )
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
        logf( (char*) "err = %0.2f", (float)err );
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelButtonAsservissement::wheelUp( int, int)
{
    char s[20];
    float err = panelCourbe->get_err();
    if ( this == pUrgentUp || this == pUrgentDown )
    {
	    fLimitCorrection /= 0.9f;
        var.set("fLimitCorrection", (float)fLimitCorrection);
        logf( (char*) "fLimitCorrection = %0.2f", (float)fLimitCorrection );
    }
    else if ( this == pFlecheHaut || this == pFlecheBas)
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
        logf( (char*) "err = %0.2f", (float)err );
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelButtonAsservissement::wheelDown( int, int)
{
    char s[20];
    float err = panelCourbe->get_err();
    if ( this == pUrgentUp || this == pUrgentDown )
    {
	    fLimitCorrection *= 0.9f;
        var.set("fLimitCorrection", (float)fLimitCorrection);
        logf( (char*) "fLimitCorrection = %0.2f", (float)fLimitCorrection );
    }
    else if ( this == pFlecheHaut || this == pFlecheBas)
    {
        err *= 0.9;
        panelCourbe->set_err( err );
        var.set( "err", err );

        sprintf( s, "+%0.2f", err );
        panelCourbe->get_pXMax()->changeText( (char*)s );
        panelCourbe->get_pYMax()->changeText( (char*)s );

        sprintf( s, "-%0.2f", err );    float err = panelCourbe->get_err();

        panelCourbe->get_pXMin()->changeText( (char*)s );
        panelCourbe->get_pYMin()->changeText( (char*)s );
        logf( (char*) "err = %0.2f", (float)err );
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
        logf( (char*) "fLimitCorrection = %0.2f", (float)fLimitCorrection );

    }
    else
    if ( pPanel == pUrgentDown )
    {
		fLimitCorrection *= 0.9f;
        var.set("fLimitCorrection", (float)fLimitCorrection);
        logf( (char*) "fLimitCorrection = %0.2f", (float)fLimitCorrection );
    }

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void call_back_up(PanelButton* pPanel)
{
	logf( (char*) "Button CallBack up()" );
	/*
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
	{    float err = panelCourbe->get_err();

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
	*/
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
        set_mode(bModeManuel);
	}
	else
	if ( p == pButtonHelp )
	{
        bPanelHelp = !bPanelHelp;
        var.set("bPanelHelp", bPanelHelp);
        panelHelp->setVisible(bPanelHelp);
        log( (char*)"Toggle panelHelp !!!" );
	}
	else
	if ( p == pButtonSerial )
	{
        bPanelSerial = !bPanelSerial;
        var.set("bPanelSerial", bPanelSerial);
        PanelConsoleSerial::getInstance().setVisible( bPanelSerial );
        logf( (char*)"Toggle serial !!!" );
	}
	else
	if ( p == pButtonStdOut )
	{
        bPanelStdOut = !bPanelStdOut;
        var.set("bPanelStdOut", bPanelStdOut);
        panelStdOut->setVisible(bPanelStdOut);
        logf( (char*)"Toggle panelStdOut !!!" );
	}
	else
	if ( p == pButtonControl )
	{
        Camera_mgr::getInstance().togglePanel();
        log( (char*)"Toggle panelCamera !!!" );

        Camera * p = Camera_mgr::getInstance().getCurrent();
        if ( p )
        {
            bool b = p->getControlVisible();
        }
	}
	else
	if ( p == pButtonResultat )
	{
        bPanelResultat = !bPanelResultat;
        var.set("bPanelResultat", bPanelResultat);
        panelResultat->setVisible(bPanelResultat);
        log( (char*)"Toggle panelResultat !!!" );

        //inverse_texture( pPanel, bPanelResultat, "cible" );
	}
	else
	if ( p == pButtonCourbe )
	{
        bPanelCourbe = !bPanelCourbe;
        var.set("bPanelCourbe", bPanelCourbe);
        panelCourbe->setVisible(bPanelCourbe);
        log( (char*)"Toggle panelCourbe !!!" );
	}
	else
	if ( p == pButtonSon )
	{
        bSound = !bSound;
        var.set("bSound", bSound);
        pButtonSon->setVal(bSound);
        logf((char*)"Bouton son Active/sDesactive le son : %s", BOOL2STR(bSound) );
	}
	else
	if ( p == pButtonPause )
	{
        bPause = !bPause;
        var.set("bPause", bPause);
        pButtonPause->setVal(bPause);
        logf((char*)"Bouton Pause : %s", BOOL2STR(bPause) );
	}
	else
	if ( p == pButtonDeplacement )
	{
        if ( !bModeManuel )				bModeManuel = true;
        bMouseDeplace = !bMouseDeplace;
        var.set("bMouseDeplace", bMouseDeplace);
        pButtonDeplacement->setVal(bMouseDeplace);
        logf((char*)"bMouseDeplace : %s", BOOL2STR(bMouseDeplace) );
	}
	/*
	*/
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelCheckBox* create_window_check_box( string tex, int dx)
{
    if ( tex.length() == 0 )    return NULL;
        
    string strue = "images/" + tex + "_down.tga";
    string sfalse = "images/" + tex + "_over.tga";

    PanelCheckBox* pCheckBox = new PanelCheckBox();
    pCheckBox->setTrue( (char*)strue.c_str() );
    pCheckBox->setFalse( (char*)sfalse.c_str() );

    pCheckBox->setCallBackMouse( cb_rotationCheck );

    pCheckBox->setBackground( (char*)NULL );
    pCheckBox->setPosAndSize( dx, 2, 16, 16 );

    panelStatus->add(pCheckBox);
    return pCheckBox;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelCheckBox* create_window_check_box( int i, string tex)
{
    return create_window_check_box(tex, 320+ i*18);
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
void create_fleches( int x, char* up, char* down, PanelButtonAsservissement* &pUp, PanelButtonAsservissement* &pDown )
{
    pUp = new PanelButtonAsservissement();
    panelStatus->add(pUp);
    
    pUp->setPosAndSize( x, 2, 10, 8 );

    pUp->setUp(   up );
    pUp->setDown( up );
    pUp->setOver( up );

	pUp->setCallBackUp(   cb_fleche );


    pDown = new PanelButtonAsservissement();
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
int xAsserv, yAsserv;
void click_left_cercle_asserv( int xm, int ym ) 
{
    logf((char*)"click_left_cercle_asserv( %d, %d)", xm, ym );
    //pCercleAsserv->setPosAndSize( 200, -200, 180, 20 );
    xAsserv =     pCercleAsserv->getPosX();
    yAsserv =     pCercleAsserv->getPosY();
    pCercleAsserv->setPos( xAsserv, yAsserv-100-8 );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void release_left_cercle_asserv( int xm, int ym ) 
{
    logf((char*)"release_left_cercle_asserv( %d, %d)", xm, ym );
    //pCercleAsserv->setPosAndSize( 200, 0, 180, 20 );
    //int x =     pCercleAsserv->getX();
    //int y =     pCercleAsserv->getY();
    pCercleAsserv->setPos( xAsserv, yAsserv );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void create_windows_button()
{
	int pos = 0;
    pButtonHelp     = create_window_check_box( pos++, "help" );
    pButtonControl  = create_window_check_box( pos++, "camera" );
    pButtonResultat = create_window_check_box( pos++, "cible" );
    pButtonCourbe   = create_window_check_box( pos++, "courbe" );
    pButtonStdOut   = create_window_check_box( pos++, "console" );
    pButtonSerial   = create_window_check_box( pos++, "arduino" );

    pButtonMode     = create_window_check_box( pos++, "mode" );
    pButtonDeplacement    = create_window_check_box( pos++, "pas" );
    pButtonAsserv   = create_window_check_box( pos++, "cadena" );
    pButtonSon      = create_window_check_box( pos++, "son" );

    pButtonAsc      = create_window_check_box( pos++, "asc" );
    pButtonDec      = create_window_check_box( pos++, "dec" );
    pButtonJoy      = create_window_check_box( pos++, "joy" );
    pButtonSui      = create_window_check_box( pos++, "terre" );
    pButtonRet      = create_window_check_box( pos++, "retour" );
    
    pButtonPause    = create_window_check_box( "pause", 10 );

	int X = pos*18 + 325;

    create_fleches( X, (char*)"images/fleche_haut.tga", (char*)"images/fleche_bas.tga", pFlecheHaut, pFlecheBas);
    create_fleches( X+50, (char*)"images/fleche_haut.tga", (char*)"images/fleche_bas.tga", pUrgentUp, pUrgentDown);

    pErr  = new PanelText( (char*)"000",		    PanelText::NORMAL_FONT, X+12, 2 );
    pErrA = new PanelTextAsservissement();// (char*)"000",		    PanelText::NORMAL_FONT, X+12, 2);
    pErrA->setPosAndSize(X+12, 2, 32, 20);
	//panelStatus->add( pErr );
	panelStatus->add( pErrA );
    /*
    */
    pUrg  = new PanelText( (char*)"000",		    PanelText::NORMAL_FONT, X+62, 2 );
    pUrgA = new PanelTextAsservissement();
    pUrgA->setPosAndSize(X+62, 2, 32, 20);
	//panelStatus->add( pUrg );
	panelStatus->add( pUrgA );
	
    char s[55];
	pCercleAsserv = new PanelSpinEditText();
    pCercleAsserv->setPosAndSize( X+62, 2, 180, 20 );
    pCercleAsserv->setClickLeft( click_left_cercle_asserv );
    pCercleAsserv->setReleaseLeft( release_left_cercle_asserv );

    sprintf( s,"%0.0f", fLimitCorrection );
    pCercleAsserv->changeText( s );

    //  Valeur de l'edition dimension de la
    pCercleAsserv->set( 1, 600, 1, 2 );
    pCercleAsserv->set_delta( 20, -100 );
    //pCercleAsserv->set_nb( 2 );
    pCercleAsserv->set_val( fLimitCorrection );
    pCercleAsserv->set_pVal( &fLimitCorrection );

    panelStatus->add( pCercleAsserv );


	pCercleErr = new PanelSpinEditText();
    pCercleErr->setPosAndSize( X+12, 2, 180, 20 );
    //pCercleErr->setClickLeft( click_left_cercle_asserv );
    //pCercleErr->setReleaseLeft( release_left_cercle_asserv );

    sprintf( s,"%0.0f", fLimitCorrection );
    pCercleErr->changeText( s );

    //  Valeur de l'edition dimension de la
    pCercleErr->set( 1, 10, 0.02, 2 );
    pCercleErr->set_delta( 20, -100 );
    //pCercleAsserv->set_nb( 2 );
    float f = 0.0;//panelCourbe->get_err();
    pCercleErr->set_val( f );
    pCercleErr->set_pVal( NULL );

    panelStatus->add( pCercleErr );
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
       //pStellarium->changeText( (char*)"Stellarium" );
       pStellarium->setColor(0xFFFFFFFF);
    }
    else
    {
       bStellarium = false;
       //pStellarium->changeText( (char*)"----" );
       pStellarium->setColor(0xFF404040);
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
    //}


    bool b;
    Camera * p = Camera_mgr::getInstance().getCurrent();
    if ( p )      b = p->getControlVisible();
    pButtonControl->setVal( b );

    pButtonSerial->setVal( bPanelSerial );
    pButtonStdOut->setVal( bPanelStdOut );
    pButtonResultat->setVal( bPanelResultat );
    pButtonCourbe->setVal( bPanelCourbe );
    pButtonHelp->setVal( bPanelHelp );
    pButtonSon->setVal( bSound );
    pButtonPause->setVal( bPause );
    pButtonDeplacement->setVal( bMouseDeplace );

    /*
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
    }    if ( bSuivi )           pSuivi->setColor(COLOR_WHITE);
    else                    pSuivi->setColor(COLOR_GREY);

    */
	if (bMouseDeplace)              pDeplacement->setColor(COLOR_WHITE);
    else                            pDeplacement->setColor(COLOR_GREY);

    if (bCorrection)                pAsservi->setColor(COLOR_WHITE);
    else                            pAsservi->setColor(COLOR_GREY);

    if ( Serial::getInstance().isConnect() )        pArduino->setColor(COLOR_WHITE);
    else                                            pArduino->setColor(COLOR_GREY);

    if ( bSuivi )                   pSuivi->setColor(COLOR_WHITE);
    else                            pSuivi->setColor(COLOR_GREY);
    
    if ( bCorrection != pButtonAsserv->getVal() )   set_asservissement();

    if ( bFirstStatus )
    {
        logf( (char*)"[WARNING]Change mode " );
        set_mode(bModeManuel);
        set_asservissement();
    }

    bFirstStatus = false;
    if ( bModeManuel != pButtonMode->getVal() )     set_mode(bModeManuel);

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

