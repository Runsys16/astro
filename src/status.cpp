class PanelButtonAsservissement : public PanelButton
{
	public:
		virtual void		wheelUp(int, int);
		virtual void		wheelDown(int, int);
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
PanelCheckBox *     pButtonDeplac;
PanelSpinEditText*  pCercleAsserv;
PanelSpinEditText*  pCercleErr;
PanelSpinEditText*  pCercleSuivi1;

PanelButtonAsservissement *       pFlecheHaut;
PanelButtonAsservissement *       pFlecheBas;

PanelButtonAsservissement *       pUrgentUp;
PanelButtonAsservissement *       pUrgentDown;


PanelCheckBox *     pButtonAsserv;

PanelText *			pAlpha;
PanelText *			pDelta;


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
	logf( (char*)"set_asservissement()" );
	
    var.set("bCorrection", bCorrection);
    fTimeCpt = 0.0; 

    var.set("vOrigine.x", panelCourbe->get_vOrigine().x);
    var.set("vOrigine.y", panelCourbe->get_vOrigine().y);



    if (bCorrection)            pAsservi->setColor(0xFFFFFFFF);
    else                        pAsservi->setColor(0X404040FF);
   
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
void setPanelCourbeErr( float e )
{
    //logf( (char*) "setPanelCourbeErr( %0.2f )", (float)e );

    char s[20];

    panelCourbe->set_err( e );

    sprintf( s, "+%0.2f", e );
    panelCourbe->get_pXMax()->changeText( (char*)s );
    panelCourbe->get_pYMax()->changeText( (char*)s );

    sprintf( s, "-%0.2f", e );
    panelCourbe->get_pXMin()->changeText( (char*)s );
    panelCourbe->get_pYMin()->changeText( (char*)s );

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
	    fLimitCorrection0 /= 0.9f;
        var.set("fLimitCorrection0", (float)fLimitCorrection0);
        logf( (char*) "PanelButtonAsservissement::wheelUp() fLimitCorrection0 = %0.2f", (float)fLimitCorrection0 );
        sprintf( s, "%0.0f", fLimitCorrection0 );
    }
    else if ( this == pFlecheHaut || this == pFlecheBas)
    {
        err /= 0.9;
        logf( (char*) "PanelButtonAsservissement::wheelUp() err = %0.2f", (float)err );
        var.set( "err", err );

		setPanelCourbeErr(err);

        sprintf( s, "%0.2f", err );
        pCercleErr->set_val( err );
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
	    fLimitCorrection0 *= 0.9f;
        var.set("fLimitCorrection0", (float)fLimitCorrection0);
        logf( (char*) "PanelButtonAsservissement::wheelDown() fLimitCorrection0 = %0.2f", (float)fLimitCorrection0 );
        sprintf( s, "%0.0f", fLimitCorrection0 );
    }
    else if ( this == pFlecheHaut || this == pFlecheBas)
    {
        err *= 0.9;
        logf( (char*) "PanelButtonAsservissement::wheelDown() err = %0.2f", (float)err );
        var.set( "err", err );

		setPanelCourbeErr(err);

        sprintf( s, "%0.2f", err );
        pCercleErr->set_val( err );
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void cb_fleche(PanelButton* pPanel)
{
    log( (char*) "cb_fleche()" );

    char s[20];
    float err = panelCourbe->get_err();
    
    if ( pPanel == pFlecheHaut )
    {
        err /= 0.9;
        var.set( "err", err );
		setPanelCourbeErr(err);
    }
    else
    if ( pPanel == pFlecheBas )
    {
        err *= 0.9;
        var.set( "err", err );
		setPanelCourbeErr(err);
    }
    else
    if ( pPanel == pUrgentUp )
    {
		fLimitCorrection0 *= 1.1f;
        var.set("fLimitCorrection0", (float)fLimitCorrection0);
        logf( (char*) "fLimitCorrection0 = %0.2f", (float)fLimitCorrection0 );

    }
    else
    if ( pPanel == pUrgentDown )
    {
		fLimitCorrection0 *= 0.9f;
        var.set("fLimitCorrection0", (float)fLimitCorrection0);
        logf( (char*) "fLimitCorrection0 = %0.2f", (float)fLimitCorrection0 );
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
	if ( p == pButtonDeplac )
	{
        if ( !bModeManuel )				bModeManuel = true;
        bMouseDeplace = !bMouseDeplace;
        var.set("bMouseDeplace", bMouseDeplace);
        pButtonDeplac->setVal(bMouseDeplace);
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
    //pCercleAsserv->setPos( xAsserv, yAsserv-100-8 );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void release_left_cercle_asserv( int xm, int ym ) 
{
    logf((char*)"release_left_cercle_asserv( %d, %d)", xm, ym );
    //logf((char*)"val = %lf",  fLimitCorrection0 );
    var.set( "fLimitCorrection0", fLimitCorrection0 );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void click_left_cercle_err( int xm, int ym ) 
{
    logf((char*)"click_left_cercle_err( %d, %d)", xm, ym );
    //pCercleAsserv->setPosAndSize( 200, -200, 180, 20 );
    xAsserv =     pCercleErr->getPosX();
    yAsserv =     pCercleErr->getPosY();
    //pCercleErr->setPos( xAsserv, yAsserv-100-8 );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void release_left_cercle_err( int xm, int ym ) 
{
    logf((char*)"release_left_cercle_err( %d, %d)", xm, ym );
    //pCercleErr->setPos( xAsserv, yAsserv );
	double* pErr = panelCourbe->get_perr();
	logf( (char*)"err=%0.2f", *pErr );
    var.set( "err", *pErr );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void click_left_cercle_suivi1( int xm, int ym ) 
{
    logf((char*)"click_left_cercle_err( %d, %d)", xm, ym );
    //pCercleAsserv->setPosAndSize( 200, -200, 180, 20 );
    xAsserv =     pCercleErr->getPosX();
    yAsserv =     pCercleErr->getPosY();
    //pCercleErr->setPos( xAsserv, yAsserv-100-8 );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void release_left_cercle_suivi1( int xm, int ym ) 
{
    logf((char*)"release_left_cercle_suivi1( %d, %d)", xm, ym );
    var.set( "fDiamSuivi1", (float)fDiamSuivi1 );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void cb_motion( int xm, int ym ) 
{
    //logf((char*)"cb_motion( %d, %d)", xm, ym );
	char s[20];
    float err;
    
    
    if ( panelCourbe )		err = panelCourbe->get_err();
    snprintf( s, sizeof(s), "%0.2f", err );
    pCercleErr->changeText( s );
	setPanelCourbeErr(err);
	var.set( "err", (float)err );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void create_windows_button()
{
    char s[55];
	int pos = 0;
	
    pButtonHelp     = create_window_check_box( pos++, "help" );		pButtonHelp->setExtraString( string("Button Help") );
    pButtonControl  = create_window_check_box( pos++, "camera" );	pButtonControl->setExtraString( string("Button Control") );
    pButtonResultat = create_window_check_box( pos++, "cible" );	pButtonResultat->setExtraString( string("Button Resultat") );
    pButtonCourbe   = create_window_check_box( pos++, "courbe" );	pButtonCourbe->setExtraString( string("Button Courbe") );
    pButtonStdOut   = create_window_check_box( pos++, "console" );	pButtonStdOut->setExtraString( string("Button StdOut") );
    pButtonSerial   = create_window_check_box( pos++, "arduino" );	pButtonSerial->setExtraString( string("Button Serial") );

    pButtonMode     = create_window_check_box( pos++, "mode" );		pButtonMode->setExtraString( string("Button Mode") );
    pButtonDeplac   = create_window_check_box( pos++, "pas" );		pButtonDeplac->setExtraString( string("Button Deplacement") );
    pButtonAsserv   = create_window_check_box( pos++, "cadena" );	pButtonAsserv->setExtraString( string("Button Asservissement") );
    pButtonSon      = create_window_check_box( pos++, "son" );		pButtonSon->setExtraString( string("Button Son") );

    pButtonAsc      = create_window_check_box( pos++, "asc" );		pButtonAsc->setExtraString( string("Button Asc Droite") );
    pButtonDec      = create_window_check_box( pos++, "dec" );		pButtonDec->setExtraString( string("Button Declinaison") );
    pButtonJoy      = create_window_check_box( pos++, "joy" );		pButtonJoy->setExtraString( string("Button Joystick") );
    pButtonSui      = create_window_check_box( pos++, "terre" );	pButtonSui->setExtraString( string("Button Suivi rotation terre") );
    pButtonRet      = create_window_check_box( pos++, "retour" );	pButtonRet->setExtraString( string("Button Retour arduino") );
    
    pButtonPause    = create_window_check_box( "pause", 10 );		pButtonPause->setExtraString( string("Button Pause") );

	int X = pos*18 + 325;

	//--------------------------------------------------------
    create_fleches( X, (char*)"images/fleche_haut.tga", (char*)"images/fleche_bas.tga", pFlecheHaut, pFlecheBas);
    create_fleches( X+50, (char*)"images/fleche_haut.tga", (char*)"images/fleche_bas.tga", pUrgentUp, pUrgentDown);
	//--------------------------------------------------------
	pCercleErr = new PanelSpinEditText();
    pCercleErr->setPosAndSize( X+12, 2, 40, 20 );
    pCercleErr->setClickLeft( click_left_cercle_err );
    pCercleErr->setReleaseLeft( release_left_cercle_err );

    sprintf( s,"%0.2f", var.getf("err")  );
    pCercleErr->changeText( s );

    pCercleErr->set( 0.0, 5.0, 2, 4 );
    pCercleErr->set_ndecimal( 2 );
    //pCercleErr->set_delta( 0, 0 );
    pCercleErr->set_val( 0.0 );					// initialisation à zero l'objet panelCourbe n'est pas encore créé
    //pCercleErr->set_pVal( NULL );
    
    pCercleErr->setMotion( cb_motion );			// callback sur deplacement de la souris

    panelStatus->add( pCercleErr );
    pCercleErr->setExtraString( string("SpinEdit Err") );
	//--------------------------------------------------------
	pCercleAsserv = new PanelSpinEditText();
    pCercleAsserv->setPosAndSize( X+62, 2, 40, 20 );
    pCercleAsserv->setClickLeft( click_left_cercle_asserv );
    pCercleAsserv->setReleaseLeft( release_left_cercle_asserv );

    sprintf( s,"%0.0f", fLimitCorrection0 );
    pCercleAsserv->changeText( s );

    //  Valeur de l'edition dimension de la
    pCercleAsserv->set( 1, 600, 760, 4 );
    pCercleAsserv->set_delta( 20, -100 );
    //pCercleAsserv->set_nb( 2 );
    pCercleAsserv->set_val( fLimitCorrection0 );
    pCercleAsserv->set_pVal( &fLimitCorrection0 );

    panelStatus->add( pCercleAsserv );
    pCercleAsserv->setExtraString( string("SpinEdit Asservi") );
	//--------------------------------------------------------
	//--------------------------------------------------------
	pCercleSuivi1 = new PanelSpinEditText();
    pCercleSuivi1->setPosAndSize( X+62+30, 2, 40, 20 );
    pCercleSuivi1->setClickLeft( click_left_cercle_suivi1 );
    pCercleSuivi1->setReleaseLeft( release_left_cercle_suivi1 );

    sprintf( s,"%0.0f", fDiamSuivi1 );
    pCercleSuivi1->changeText( s );

    //  Valeur de l'edition dimension de la
    pCercleSuivi1->set( 1, 600, 760, 4 );
    pCercleSuivi1->set_delta( 20, -100 );
    //pCercleAsserv->set_nb( 2 );
    pCercleSuivi1->set_val( fDiamSuivi1 );
    pCercleSuivi1->set_pVal( &fDiamSuivi1 );

    panelStatus->add( pCercleSuivi1 );
    pCercleSuivi1->setExtraString( string("SpinEdit fDiamSuivi1") );
	//--------------------------------------------------------
	//#define POLICE "usr/share/fonts/truetype/liberation/LiberationMono-Bold.ttf",
	//#define POLICE "usr/share/fonts/truetype/ubuntu/Ubuntu-B.ttf",
	//#define POLICE "fonts/IFAOGrecBold.ttf" ,
	#define POLICE "fonts/greek-wsi-regular.ttf"   	// 360 glyphe voir alpha ...

	pAlpha = new PanelText( 	(char*)"a", (char*)POLICE , 60, 0, 14, 0xFFFFFFFF );
	pDelta = new PanelText( 	(char*)"d", (char*)POLICE , 200, 0, 14, 0xFFFFFFFF );
	
    //pAD = new PanelText( (char*)"=\ --h --\' --\"",		            PanelText::NORMAL_FONT, 60+10, 2 );
    char strAD[] = "= --h --\' --\"";
    char strDC[] = "= --  --\' --\"";
    strDC[4] = 176;		// caractere degre °
    
    pAD = new PanelText( (char*)strAD,		PanelText::NORMAL_FONT, 60+10, 2 );
    pDC = new PanelText( (char*)strDC,		PanelText::NORMAL_FONT, 200+10, 2 );

	panelStatus->add( pAD );
	panelStatus->add( pDC );

    pAlpha->setPos( 60-2,  2);
    pDelta->setPos( 200-2, 1);
    /*
    */
    
	panelStatus->add( pAlpha );
	panelStatus->add( pDelta );
	//WindowsManager::getInstance().add( pAlpha );
	//WindowsManager::getInstance().add( pDelta );
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
       pStellarium->setColor(0x404040FF);
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
    pButtonDeplac->setVal( bMouseDeplace );

    /*
    if (err_old != panelCourbe->get_err() )
    {
        err_old = panelCourbe->get_err();
        char str[255];
        sprintf( str, "%0.2f", err_old );
        pErr->changeText( (char*)str );
    }

    if (urg_old != fLimitCorrection0 )
    {
        urg_old = fLimitCorrection0;
        char str[255];
        sprintf( str, "%0.2f", urg_old );
        pUrg->changeText( (char*)str );
    }    if ( bSuivi )           pSuivi->setColor(COLOR_WHITE);
    else                    pSuivi->setColor(COLOR_GREY);

    */
	if (bMouseDeplace)              				pDeplacement->setColor(COLOR_WHITE);
    else                            				pDeplacement->setColor(COLOR_GREY);

    if (bCorrection)                				pAsservi->setColor(COLOR_WHITE);
    else                            				pAsservi->setColor(COLOR_GREY);

    if ( Serial::getInstance().isConnect() )        pArduino->setColor(COLOR_WHITE);
    else                                            pArduino->setColor(COLOR_GREY);

    if ( bSuivi )                   				pSuivi->setColor(COLOR_WHITE);
    else                            				pSuivi->setColor(COLOR_GREY);
    
    if ( bCorrection != pButtonAsserv->getVal() )   set_asservissement();

    if ( bFirstStatus )
    {
        logf( (char*)"[WARNING]Change mode " );
        set_mode(bModeManuel);
        set_asservissement();
    }

    bFirstStatus = false;
    if ( bModeManuel != pButtonMode->getVal() )     set_mode(bModeManuel);
    

	if ( panelCourbe!= NULL && pCercleErr->get_pVal()==NULL )
	{
		double* pErr = panelCourbe->get_perr();
		logf( (char*)"err=%0.2f", *pErr );
	    pCercleErr->set_pVal( pErr );
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void status_nuit(unsigned long color )
{
	pCercleAsserv->setColor( color );
	pCercleErr->setColor( color );

	pFlecheHaut->setColor( color );
	pFlecheBas->setColor( color );

	pUrgentUp->setColor( color );
	pUrgentDown->setColor( color );

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

