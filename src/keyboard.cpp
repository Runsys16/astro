#define KEYBOARD_CPP

#include "camera.h"
#include "camera_mgr.h"
#include "connexion_mgr.h"
#include "timer.h"
#include "pleiade.h"
#include "panel_console_serial.h"
#include "serveur_mgr.h"
#include "lx200.h"
#include "synscan.h"
#include "capture.h"
#include "var_mgr.h"
#include "alert_box.h"
#include "file_browser.h"
#include "captures.h"
#include "bluetooth.h"
#include "panel_courbe.h"
#include "panel_enregistre.h"
#include "panel_stdout.h"
#include "star_catalogue.h"
#include "catalog.h"
#include <GL/freeglut_ext.h>
#include <mutex>
#include "MathAstro.h"
#include "keyboard.h"
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
//VarManager& var = VarManager::getInstance();
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void KeyboardFuncCtrlAlt(unsigned char key, int x, int y)
{
#ifdef DEBUG_CALLBACK_OPENGL
    logf( (char*)"    %02d\t%.8f KeyboardFuncCtrlAlt", ++appelIdle, -timerAppelIdle +Timer::getInstance().getGlutTime()  );
	logf( (char*)"main::KeyboardFuncCtrlAlt(%d, %d, %d)", (unsigned)key, x, y );
#endif

    bool bShift = (iGlutModifier & GLUT_ACTIVE_SHIFT ) ? true : false;

    WindowsManager&     wm      = WindowsManager::getInstance(); 
	VarManager&			var		= VarManager::getInstance();
    //Camera_mgr&         cam_mgr = Camera_mgr::getInstance();
	
	switch(key)
	{ 
	//----------------------------------------------------------------------------
	case 14:
	    {
	        logf( (char*)"Key = Ctrl+Alt+n : Magnitude limite" );
            log_tab(true);
            bGmagChange = true;
            
            float fGmagMax = var.getf("fGmagMax");
            if ( bShift )			fGmagMax -= 0.1;
            else					fGmagMax += 0.1;
            var.set("fGmagMax", fGmagMax );
	        
	        logf( (char*)"fGmagMax = %0.2f", var.getf("fGmagMax") );
	        
            log_tab(false);
		}
	    break;
	//----------------------------------------------------------------------------
    default:
		{
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------
// Symboles de fonction  ASCII Touche CTRL
//	décimal 	hexa. 	clavier 	terme anglais 		   terme français
// NULL 0	  	00	 	Ctrl+@ 	Null 					Nul
// SOH 	1  		01 		Ctrl+A 	Start of heading 		Début d'entête
// STX 	2  		02 		Ctrl+B 	Start of text 			Début de texte
// ETX 	3  		03 		Ctrl+C 	End of text 			Fin de texte
// EOT 	4  		04 		Ctrl+D 	End of transmit 		Fin de communication
// ENQ 	5  		05	 	Ctrl+E 	Enquiry 				Demande
// ACK 	6  		06 		Ctrl+F 	Acknowledge 			Accusé de réception
// BELL 7  		07	 	Ctrl+G 	Bell 					Sonnerie
// BS 	8  		08 		Ctrl+H 	Backspace 				Retour arrière
// HT 	9  		09 		Ctrl+I 	Horizontal tab 			Tabulation horizontale
// LF 	10  	0A 		Ctrl+J 	Line feed 				Interligne
// VT 	11  	0B 		Ctrl+K 	Vertical tab 			Tabulation verticale
// FF 	12  	0C 		Ctrl+L 	Form feed 				Page suivante
// CR 	13  	0D 		Ctrl+M 	Carriage return 		Retour en début de ligne
// SO 	14  	0E 		Ctrl+N 	Shitf out 				Hors code
// SI 	15  	0F 		Ctrl+O 	Shift in 				En code
// DLE 	16  	10 		Ctrl+P 	Data line escape 		Echappement en transmission
// DC1 	17  	11 		Ctrl+Q 	Device control 1 		Commande auxiliaire n° 1
// DC2 	18  	12 		Ctrl+R 	Device control 2 		Commande auxiliaire n° 2
// DC3 	19  	13 		Ctrl+S 	Device control 3 		Commande auxiliaire n° 3
// DC4 	20  	14 		Ctrl+T 	Device control 4 		Commande auxiliaire n° 4
// NAK 	21  	15 		Ctrl+U 	Negative acknowledge 	Accusé de réception négatif
// SYN 	22  	16 		Ctrl+V 	Synchronous idle 		Synchronisation
// ETB 	23  	17 		Ctrl+W 	End of transmit block 	Fin de bloc transmis
// CAN 	24  	18 		Ctrl+X 	Cancel 					Annulation
// EM 	25  	19 		Ctrl+Y 	End of medium 			Fin de support
// SUB 	26  	1A 		Ctrl+Z 	Substitute 				Remplacement
// ESC 	27  	1B 		Ctrl+[ 	Escape 					Echappement
// FS 	28  	1C 		Ctrl+\ 	File separator 			Séparateur de fichier
// GS 	29  	1D 		Ctrl+] 	Group separator 		Séparateur de groupe
// RS 	30  	1E 		Ctrl+^ 	Record separator 		Séparateur d'enregistrement
// US 	31  	1F 		Ctrl+_ 	Unit separator 			Séparateur d'unité
// SP 	32  	20 		Space 	Espacement
// DEL 	127  	7F 		Delete 	Effacement
//--------------------------------------------------------------------------------------------------------------------
void KeyboardFuncCtrl(unsigned char key, int x, int y)
{
#ifdef DEBUG_CALLBACK_OPENGL
    logf( (char*)"    %02d\t%.8f KeyboardFuncCtrl", ++appelIdle, -timerAppelIdle +Timer::getInstance().getGlutTime()  );
	logf( (char*)"main::KeyboardFuncCtrl(%c, %d, %d)", (char)key, x, y );
#endif

    WindowsManager&     wm      = WindowsManager::getInstance(); 
    Camera_mgr&         cam_mgr = Camera_mgr::getInstance();
	VarManager&			var		= VarManager::getInstance();
	
    bool bShift = (iGlutModifier & GLUT_ACTIVE_SHIFT ) ? true : false;

	switch(key){ 
	
    // CTRL A
    case 1:
		{
		    //deleteStars();
        }
		break;
	/*
	// CTRL D
    case 4:
		{
        }
        break;
    */
	// CTRL E
    case 5:
		{
			if ( bShift )	{
				logf( (char*)"Key (ctrl+Shift+e) : Exporte Vizier" );
				Capture* p = Captures::getInstance().getCurrentCapture();
				if ( p != NULL )	{
					logf( (char*)" Export %s", p->getBasename().c_str() );
					p->export_vizier();
				}
			}
			else	{
				logf( (char*)"Key (ctrl+e) : Exporte etoiles" );
				Capture* p = Captures::getInstance().getCurrentCapture();
				if ( p != NULL )	{
					logf( (char*)" Export %s", p->getBasename().c_str() );
					p->export_stars();
				}
			}
            //Captures::getInstance().fullscreen();
        }
        break;
	// CTRL F
    case 6:
		{
            logf( (char*)"Key (ctrl+f) : FullScreen" );
            if ( !Captures::getInstance().fullscreen() )
            	Camera_mgr::getInstance().fullscreen();
        }
        break;
	// CTRL+H
    case 8:
		{
            logf( (char*)"Key (ctrl+H) : resetSatrCompare" );
            log_tab(true);
			Captures::getInstance().resetVizierMagMax();
            log_tab(false);
        }
        break;
	// TAB
    case 9:
		{
            logf( (char*)"Key (ctrl+TAB) : Change de camera" );
            log_tab(true);
			Camera_mgr&  cam_mgr = Camera_mgr::getInstance();
			cam_mgr.active_next();
            log_tab(false);
        }
        break;
	// CTRL N
    case 14:
        {
	        logf( (char*)"Key (N) : Mode NUIT");
            log_tab(true);
		    bNuit = !bNuit;
		    logf( (char*)"  bNuit = %s", BOOL2STR(bNuit) );
		    var.set("bNuit", bNuit);

		    setColor();
            log_tab(false);
        }
        break;

	// CTRL O
    case 15:
		{
		    logf( (char*)"Key (Ctrl+o) : File browser affiche une image" );
		    FileBrowser& fb = FileBrowser::getInstance();
		    fb.getVisible();
		    fb.setFiltre( "" );

		    if ( var.existe("DirFileBrowser") )
		    {
		        workDirFileBrowser = *var.gets( "DirFileBrowser" );
		    }

		    fb.change_dir( workDirFileBrowser );
		    fb.setFilename( "" );
		    bFileBrowser = !bFileBrowser;
		    fb.setCallBack( &cb_file_browser );
		    fb.setTitle( "Ouvre une image" );
            fb.loadSkin( PanelWindow::VERT );
            fb.setBorderSize( 6 );
		    fb.setSTitle( "Selectionnez une image FITS FIT PNG JPG JPEG" );
		    
		    if ( bFileBrowser )         fb.affiche();
		    else                        fb.cache();
       	}
       	break;
	// CTRL P
    case 16:
		{
	    logf( (char*)"Key (ctrl+p) : Noop" );
        }
        break;
	// CTRL q
	case 17:
	    {
	    logf( (char*)"Key (ctrl+Q) : ByeBye !!" );
	    quit();
	    }
        break;
        // Ctrl W
	case 0x17:
	    {
	    logf( (char*)"Key (ctrl+W) : Suppression de l'image courante !!" );
	    Captures::getInstance().supprime();
	    }
        break;

	case 0xB2:
	    {
	    logf( (char*)"Key (ctrl+²) : Changement de camera !!" );
            log_tab(true);
			Camera_mgr&  cam_mgr = Camera_mgr::getInstance();
			cam_mgr.active_prev();
            log_tab(false);
	    }
        break;

    default:
		{
		    logf( (char*)"KeyboardFuncCtrl  key= '%c'", (char)key );
		    logf( (char*)"KeyboardFuncCtrl  key= %#04X : %d", (int)key, (int)key );
        }
        break;
    }		
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void KeyboardFuncAlt(unsigned char key, int x, int y)
{
#ifdef DEBUG_CALLBACK_OPENGL
    logf( (char*)"    %02d\t%.8f KeyboardFuncAlt", ++appelIdle, Timer::getInstance().getCurrentTime()  );
	logf( (char*)"main::KeyboardFuncAlt(%c, %d, %d)", (char)key, x, y );
#endif

    WindowsManager&     wm      = WindowsManager::getInstance(); 
    Camera_mgr&         cam_mgr = Camera_mgr::getInstance();
	VarManager&			var		= VarManager::getInstance();

    bool bShift = (iGlutModifier & GLUT_ACTIVE_SHIFT ) ? true : false;
	
	switch(key){ 
	//----------------------------------------------------------------------------
    case 'A':
        {
	        logf( (char*)"Key (alt+shift+a) : Arduino bavard" );
	        
        	if ( var.existe("bVerboseArduino") )    {
        		bool b = !var.getb("bVerboseArduino");
        		var.set("bVerboseArduino", b);
			    logf( (char*)"bVerboseArduino = %s", BOOL2STR(b) );
        	}
        //logf( (char*)"Key (b) : Bluetooth disconnect" );
        //BluetoothManager::getInstance().disconnect();
        }
        break;
	//----------------------------------------------------------------------------
    case 'C':
        {
	        logf( (char*)"Key (alt+shift+c) : Aff le cercle de colimation %s", BOOL2STR(!bAffColimation) );

			bAffColimation = !bAffColimation;
       		var.set("bAffColimation", bAffColimation);
        }
        break;
	//----------------------------------------------------------------------------
	case 'e':
	    {
	        logf( (char*)"Key (Alt+e) : Affiche le catalog" );
            log_tab(true);

	        bAffCatalog = !bAffCatalog;
            VarManager::getInstance().set("bAffCatalog", bAffCatalog);;

	        logf( (char*)"Affiche oui/non le catalogue Vizier : %s", BOOL2STR(bAffCatalog) );
            var.set( "bAffCatalog", bAffCatalog );
            log_tab(false);
	    }
	    break;
	//----------------------------------------------------------------------------
	case 'g':
	    {
	        logf( (char*)"Key (Alt+g) : Affiche dic current" );
            log_tab(true);

	    	Capture* pCurrent = Captures::getInstance().getCurrentCapture();
	    	if ( pCurrent == NULL )			break;
	        pCurrent->afficheFitsDic();

            log_tab(false);
	    }
	    break;
	//----------------------------------------------------------------------------
    case 'h':
        {
        logf( (char*)"Key (Alt+h) : Charge une image de la camera courante" );
        Camera_mgr& cams = Camera_mgr::getInstance();
		cams.enregistre_name();	        
        }
        break;
	//----------------------------------------------------------------------------
    case 'j':
        {
	        logf( (char*)"Key (Alt+j) : Affiche dic current" );
            log_tab(true);
	        PanelCapture* p = (PanelCapture*)wm.getCapture();

	        if ( p == NULL )
	        {
		        log( (char*)"[WARNING] Pas de fenetre sous la souris" );
	        }
	        else
	        if ( typeid(*p) == typeid(PanelCapture)	)
	        {
	    		if ( p )
	    		{
	    			double d = *( p->getStars()->getvB() );
					d += 0.1;
					p->getStars()->setB( d );
					logf( (char*)"B = %0.2f", (float)d );
				}				
		    }
        //logf( (char*)"Key (b) : Bluetooth disconnect" );
        //BluetoothManager::getInstance().disconnect();
            log_tab(false);
        }
        break;
	//----------------------------------------------------------------------------
	#define C_FACTOR   1.01
    case 'K':
        {
	        logf( (char*)"Key (alt+shift+K) : Comparaison etoiles ajustement");
	        
	        log_tab(true);
        	Captures& 	caps 	= Captures::getInstance();
        	Capture* 	cap		= caps.getCurrentCapture();
        	
        	if ( cap )	
        	{
				Stars* stars = cap->getPanelCapture()->getStars();
				if ( stars )
				{
					double cC = stars->getC();
					cC *= C_FACTOR;
					logf( (char*)"Coef C=%0.2lf", cC );
					stars->setC( cC );
					
					bDesactiveLog = true;
					cap->compareStar();
					cap->compareStar();
					bDesactiveLog = false;
					cap->compareStar();
				}
        	}
	        log_tab(false);
        }
        break;
	//----------------------------------------------------------------------------
    case 'k':
        {
	        logf( (char*)"Key (alt+k) : Comparaison etoiles ajustement");
	        
	        log_tab(true);
        	Captures& 	caps 	= Captures::getInstance();
        	Capture* 	cap		= caps.getCurrentCapture();
        	
        	if ( cap )	
        	{
				Stars* stars = cap->getPanelCapture()->getStars();
				if ( stars )
				{
					double cC = stars->getC();
					cC /= C_FACTOR;
					logf( (char*)"Coef C=%0.2lf", cC );
					stars->setC( cC );
					
					bDesactiveLog = true;
					cap->compareStar();
					cap->compareStar();
					bDesactiveLog = false;
					cap->compareStar();
				}
        	}
	        log_tab(false);
        }
        break;
	//----------------------------------------------------------------------------
	#define A_FACTOR   1.01
	case 'l':
		{
	        logf( (char*)"Key (alt+l) : Comparaison etoiles ajustement");
	        log_tab(true);
        	Captures& 	caps 	= Captures::getInstance();
        	Capture* 	cap		= caps.getCurrentCapture();
        	
        	if ( cap )	
        	{
				Stars* stars = cap->getPanelCapture()->getStars();
				if ( stars )
				{
					double A = stars->getA() / A_FACTOR;
					stars->setA( A );
					//logf( (char*)"Coef A=%0.2lf", A );
					bDesactiveLog = true;
					cap->compareStar();
					cap->compareStar();
					bDesactiveLog = false;
					cap->compareStar();
					cap->affine_compareStar(true);
					//cap->update_info_graph();
				}
        	}
	        log_tab(false);
        }
		break;
	//----------------------------------------------------------------------------
	case 'L':
		{
	        logf( (char*)"Key (alt+shift+l) :  Comparaison etoiles ajustement");
	        log_tab(true);
        	Captures& 	caps 	= Captures::getInstance();
        	Capture* 	cap		= caps.getCurrentCapture();
        	
        	if ( cap )	
        	{
				Stars* stars = cap->getPanelCapture()->getStars();
				if ( stars )
				{
					double A = stars->getA() * A_FACTOR;
					stars->setA( A );
					logf( (char*)"Coef A=%0.2lf", A );
					bDesactiveLog = true;
					cap->compareStar();
					cap->compareStar();
					bDesactiveLog = false;
					cap->compareStar();
					cap->affine_compareStar(false);
					//cap->update_info_graph();
				}
        	}
	        log_tab(false);
        }
		break;
	//----------------------------------------------------------------------------
    case 'm':
        {
	        logf( (char*)"Key (alt+m) : Comparaison etoiles parametre A");
            log_tab(true);

	        PanelCapture* p = (PanelCapture*)wm.getCapture();

	        if ( p == NULL )
	        {
		        log( (char*)"[WARNING] Pas de fenetre sous la souris" );
	        }
	        else
	        if ( typeid(*p) == typeid(PanelCapture)	)
	        {
	    		if ( p )
	    		{
	    			double d = *( p->getStars()->getvA() );
					d /= 1.1;
					p->getStars()->setA( d );
					logf( (char*)"A = %0.2f", (float)d );
				}				
		    }
        //logf( (char*)"Key (b) : Bluetooth disconnect" );
        //BluetoothManager::getInstance().disconnect();
            log_tab(false);
        }
        break;
	//----------------------------------------------------------------------------
	case 'n':
	    {
	        logf( (char*)"Alt+n : Compare les etoiles trouvees avec le CDS" );
            log_tab(true);
	        Panel* panel = wm.getCapture();
	        if ( panel == NULL )
	        {
		        log( (char*)"[WARNING] Pas de fenetre sous la souris" );
	        }
	        else
	        if ( typeid(*panel) == typeid(PanelCapture)	)
	        {
			    Capture* capture = Captures::getInstance().getCurrentCapture();
			    if (capture )		capture->compareStar();
			}
	        else
	        if ( typeid(*panel) == typeid(PanelCamera)	)
	        {
			    Camera* camera = Camera_mgr::getInstance().getCurrent();
			    if ( camera )		        camera->compareStar();
			}
            log_tab(false);
		}
	    break;
	//----------------------------------------------------------------------------
	case 'N':
	    {
	        logf( (char*)"Key (alt+shift+n) : Compare les etoiles trouvees avec le CDS " );
            log_tab(true);

	        logf( (char*)"      \t: voir le fichier ~/.astropilot/gnuplot/magnitude.png" );
	        Panel* panel = wm.getCapture();

	        if ( panel==NULL )
	        {
		        log( (char*)"[WARNING] Pas de fenetre sous la souris" );
	        }
	        else
	        if ( typeid(*panel) == typeid(PanelCapture)	)
	        {
				log( (char*)"Panel capture" );

			    Capture* capture = Captures::getInstance().getCurrentCapture();
			    if ( capture==NULL )		break;
	        	
	        	capture->getPanelCapture()->saveCompareStar();
	        	string filename;
	        	filename = string( capture->getBasename() );
		        thread( &commande_magnitude, filename ).detach();
	        }
			else
	        if ( typeid(*panel) == typeid(PanelCamera)	)
			{
				log( (char*)"Panel camera" );

				Camera* camera = Camera_mgr::getInstance().getCurrent();
				if ( camera==NULL )		break;
				camera->saveCompareStar();
		        thread( &commande_magnitude, "pleiades" ).detach();
			}
            log_tab(false);
		}
	    break;
	//----------------------------------------------------------------------------
	case 'r':
	    {
	        logf( (char*)"Key (alt+r) : affiche etoile " );

	        bAffStar = !bAffStar;
            VarManager::getInstance().set("bAffStar", bAffStar);;
	        logf( (char*)"Affiche oui/non etoiles : %s", BOOL2STR(bAffStar) );
	    }
	    break;
	//----------------------------------------------------------------------------
	case 'u':
	    {
	        logf( (char*)"key (alt+u) : Recentrage suivi " );
	        
	        Camera* p = Camera_mgr::getInstance().getCurrent();
	        if ( p!=NULL )
	        {
	        	p->recentreSuivi();
	        }
	        else
		        logf( (char*)"Pas de camera" );
		}
	    break;
	//----------------------------------------------------------------------------
	case 't':
	    {
	        logf( (char*)"Key (alt+z) : Affiche etoiles findstar" );
            log_tab(true);

	        bAffFindStar = !bAffFindStar;
            VarManager::getInstance().set("bAffFindStar", bAffFindStar);;

	        logf( (char*)"Affiche oui/non les etoiles find_star : %s", BOOL2STR(bAffFindStar) );
            var.set( "bAffCatalog", bAffCatalog );
            log_tab(false);
	    }
	    break;

	//----------------------------------------------------------------------------
	case 'w':
	    {
	        logf( (char*)"key (alt+w) : XRef " );

	        Xref -= 1.0;
            var.set( "Xref",  (double)Xref );
	        //logf( (char*)"Xref : %0.2f", (double)Xref );
	    }
	    break;
	//----------------------------------------------------------------------------
	case 'x':
	    {
	        Xref += 1.0;
            var.set( "Xref",  (double)Xref );
	        //logf( (char*)"Xref : %0.2f", (double)Xref );
	    }
	    break;
	//----------------------------------------------------------------------------
	case 'c':
	    {
	        ZrefX -= 1.0;
            var.set( "ZrefX", (double)ZrefX );
	        //logf( (char*)"Zref : %0.2f", (double)Zref );
	    }
	    break;
	//----------------------------------------------------------------------------
	case 'v':
	    {
	        ZrefX += 1.0;
            var.set( "ZrefX", (double)ZrefX );
	        //logf( (char*)"Zref : %0.2f", (double)Zref );
	    }
	    break;
	//----------------------------------------------------------------------------
	case 'q':
	    {
	        Yref -= 1.0;
            var.set( "Yref",  (double)Yref );
	        //logf( (char*)"Yref : %0.2f", (double)Yref );
	    }
	    break;
	//----------------------------------------------------------------------------
	case 's':
	    {
	        Yref += 1.0;
            var.set( "Yref",  (double)Yref );
	        //logf( (char*)"Yref : %0.2f", (double)Yref );
	    }
	    break;
	//----------------------------------------------------------------------------
	case 'd':
	    {
	        ZrefY -= 1.0;
            var.set( "ZrefY", (double)ZrefY );
	        //logf( (char*)"Zref : %0.2f", (double)Zref );
	    }
	    break;
	//----------------------------------------------------------------------------
	case 'f':
	    {
	        ZrefY += 1.0;
            var.set( "ZrefY", (double)ZrefY );
	        //logf( (char*)"Zref : %0.2f", (double)Zref );
	    }
	    break;
	//----------------------------------------------------------------------------
	case 'a':
	    {
	        Wref -= 0.25;
            var.set( "Wref",  (double)Wref );
	        //logf( (char*)"Wref : %0.2f", (double)Wref );
	    }
	    break;
	//----------------------------------------------------------------------------
	case 'z':
	    {
	        Wref += 0.25;
            var.set( "Wref",  (double)Wref );
	        //logf( (char*)"Wref : %0.2f", (double)Wref );
	    }
	    break;
	//----------------------------------------------------------------------------
    default:
		{
		    //cout << "Default..." << endl;
		    logf( (char*)"KeyboardFuncAlt  default key=%c", (char)key );
        }
        break;
    }		
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0')
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void KeyboardFunc(unsigned char key, int x, int y) {
#ifdef DEBUG_CALLBACK_OPENGL
    logf( (char*)"    %02d\t%.8f KeyboardFunc", ++appelIdle, -timerAppelIdle +Timer::getInstance().getGlutTime() );
#endif

	iGlutModifier	= glutGetModifiers();
    bFileBrowser	= FileBrowser::getInstance().getVisible();

    Camera_mgr&  		cam_mgr = Camera_mgr::getInstance();
    WindowsManager& 	wm 		= WindowsManager::getInstance();
	VarManager&			var		= VarManager::getInstance();
    
    logf( (char*)"main::KeyboardFunc( '%c', %d, %d)", (char)key, x, y );
    /*
    Panel* pC = wm.getCapture();
    logf( (char*)"  Capture : %s", (pC == NULL ? (char*)"NULL": (char*)pC->getExtraString().c_str()) );
    Panel* pF = wm.getFocus();
    logf( (char*)"  Focus   : %s", (pF == NULL ? (char*)"NULL": (char*)pF->getExtraString().c_str()) );
	*/    
    //logf( (char*)"  iGlutModifier = " BYTE_TO_BINARY_PATTERN BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(iGlutModifier>>8), BYTE_TO_BINARY(iGlutModifier) );
    //wm.setModifier(iGlutModifier);
    
    //------------------------------------------------------------------------
    if (tAlert.size() != 0 )
    {
        if ( bArretUrgence )   
        {
            if (key == 27 )     { 
                arret_urgence();
                if ( panelEnregistre != NULL && panelEnregistre->getShooting() )
                {
                	panelEnregistre->stop();
                }
            }
        }
        bArretUrgence = false;
        alertBoxQuit();
        return;
    }
    else
    //------------------------------------------------------------------------
    if ( bPanelSerial && PanelConsoleSerial::getInstance().keyboard(key, x, y) )
    {
    	return;
    }
    else
    //------------------------------------------------------------------------
    if ( bFileBrowser )
    {
        FileBrowser::getInstance().keyboard( key, x, y);
        return;
    }
    //------------------------------------------------------------------------
    else
    if ( panelEnregistre && panelEnregistre->getVisible() && panelEnregistre->keyboard(key, x, y) )      return;
    //------------------------------------------------------------------------
    else
    if (        cam_mgr.getCurrent() != NULL
//            &&  cam_mgr.getCurrent()->getControlVisible()       )
            &&  cam_mgr.getCurrent()->getPanelCamera() == wm.getCapture()       )
    {
        if ( cam_mgr.keyboard(key) )	{
        	return;
        }
    }


    //------------------------------------------------------------------------
    //------------------------------------------------------------------------


	if ( (iGlutModifier & GLUT_ACTIVE_CTRL_ALT) == GLUT_ACTIVE_CTRL_ALT )
	{
   	    KeyboardFuncCtrlAlt(key,  x,  y);
	    return;
	}
    else
    //------------------------------------------------------------------------
	if ( (iGlutModifier & GLUT_ACTIVE_ALT) == GLUT_ACTIVE_ALT )
	{
   	    KeyboardFuncAlt(key,  x,  y);
	    return;
	}
    else
    //------------------------------------------------------------------------
	if ( (iGlutModifier & GLUT_ACTIVE_CTRL) == GLUT_ACTIVE_CTRL )
	{
        KeyboardFuncCtrl(key,  x,  y);
        return;
	}
	//else
    //------------------------------------------------------------------------
	
    //------------------------------------------------------------------------
	switch(key){ 
	
	// CTRL I
    // touche tab
	case 9:
		//WindowsManager::getInstance().swapVisible();
    	{

            logf( (char*)"Key (TAB) : Image suivante" );
            log_tab(true);
            Captures::getInstance().rotate_capture_plus(false);
            log_tab(false);
        }
		break;
	// CTRL J  LF
	case 10:
		break;
	// CTRL M  CR
	case 13:
        logf( (char*)"Key (CR) : Plein ecran" );
		log_tab(true);
        
	    bFull = !bFull;
        var.set("bFull", bFull);

        if (bFull ){
     		glutFullScreen();
            //reshapeGL(vCameraSize.x,1200);
            logf( (char*)"w=%d h=%d", width, height );
            log( (char*)"FullScreen !!!" );
        }
        else    {
            logf( (char*)"w=%d h=%d", width, height );
		    glutPositionWindow( (widthScreen-width)/2, (heightScreen-height)/2  );
		    glutReshapeWindow( width, height );
            log( (char*)"NormalScreen !!!" );
		}
		log_tab(false);
    	break;
	// touche escape
	case 27:
	    {
	    logf( (char*)"Key (ESC) : ARRET D'URGENCE !!" );
	    alertBox( "Confirmez l'arret de la monture 'ESC'" );
	    bArretUrgence = true;
	    }
        break;
	//  touche ² 
    case 178:
    	{
            logf( (char*)"Key (2) : Image precedente" );
            log_tab(true);
	        Captures::getInstance().rotate_capture_moins(false);
            log_tab(false);
    	}
    	break;
    // touche '-'
    case 45:
    	{
            logf( (char*)"Key (-) : Image icones" );
            log_tab(true);
            //Captures::getInstance().iconize_all();
            Captures::getInstance().iconize_active();
            log_tab(false);
    	}
	    break;
    
    // touche '²' Au dessus de tab
    case 126:
    	{
           // Captures::getInstance().showIcones();
            logf( (char*)"Key (126) : Image precedente" );
            log_tab(true);
	        Captures::getInstance().rotate_capture_moins(true);
            log_tab(false);
    	}
	    break;
    
    case 'A':
        {
        if ( !bAfficheVec )
        {
            logf( (char*)"Key (A) : Pt1 Ascension droite si les vecteurs sont affiches (y)" );
            break; 
        }




        logf( (char*)"Key (A) : Pt1 Ascension droite" );
        Camera_mgr& 		cams = Camera_mgr::getInstance();





        if ( !bModeManuel )
        {
            vec2* pv = cams.getSuivi();
            if ( pv != NULL )
            {
                double x   = pv->x;
                double y   = pv->y;

                //change_joy( xSuivi, ySuivi );

                vecAD[1].x = x;
                vecAD[1].y = y;
                vecAD[1].z = 0.0;
            }
        }
        else
        {
            vecAD[1].x = xClick;
            vecAD[1].y = yClick;
            vecAD[1].z = 0.0;
        }



        logf( (char*)"AD[1] (%0.2f,%0.2f)", vecAD[1].x, vecAD[1].y );
      	Camera* pCam = cams.getCurrent();
        if ( pCam )
        {
        	string name = pCam->getName();
        	
	        logf( (char*)"Camera : %s", name.c_str() );
		    var.set( name + "_vecAD[1].x", vecAD[1].x);
		    var.set( name + "_vecAD[1].y", vecAD[1].y);
		    
	    	pCam->setptAD1( vecAD[1] );
	    }




        var.set("vecAD[1].x", vecAD[1].x);
        var.set("vecAD[1].y", vecAD[1].y);
        
        	
        }
        break;
    case 'a':
        {
        if ( !bAfficheVec )
        {
            logf( (char*)"Key (A) : Pt0 Ascension droite si les vecteurs sont affiches (y)" );
            break; 
        }

        logf( (char*)"Key (a) : Pt0 Ascension droite" );
        Camera_mgr& 		cams = Camera_mgr::getInstance();

        if ( !bModeManuel )
        {
            vec2* pv = cams.getSuivi();
            if ( pv != NULL )
            {
                double x   = pv->x;
                double y   = pv->y;

                //change_joy( xSuivi, ySuivi );

                vecAD[0].x = x;
                vecAD[0].y = y;
                vecAD[0].z = 0.0;
            }
        }
        else
        {
            vecAD[0].x = xClick;
            vecAD[0].y = yClick;
            vecAD[0].z = 0.0;
        }

      	Camera* pCam = cams.getCurrent();
        if ( pCam )
        {
        	string name = pCam->getName();
	        logf( (char*)"Camera : %s", name.c_str() );
		    var.set( name + "_vecAD[0].x", vecAD[0].x);
		    var.set( name + "_vecAD[0].y", vecAD[0].y);

	    	pCam->setptAD0( vecAD[0] );
	    }

        var.set("vecAD[0].x", vecAD[0].x);
        var.set("vecAD[0].y", vecAD[0].y);


        logf( (char*)"AD[0] (%0.2f,%0.2f)", vecAD[0].x, vecAD[0].y );
        char cmd[255];
        sprintf( cmd, "a-1000p" );
        logf( (char*)"Envoi de la commande : %s",  cmd );
        Serial::getInstance().write_string(cmd);
        }
        break;

    case 'b':
        {
		    Captures::getInstance().rotateInfoFitsPlus();
        }
        break;
    
    case 'B':
        {
		    Captures::getInstance().rotateInfoFitsMoins();
        }
        break;
    
    case 'c':
        {
        logf( (char*)"Key (c) :  une nouvelle traces" );
        t_vTrace.push_back( new (vector<vec2>)() );
        logf( (char*)"  nb trace = %d", t_vTrace.size() );
        }
        break;
    case 'C':
        {
        logf( (char*)"Key (C) : Rec trace" );
        bRecTrace = !bRecTrace;
        logf( (char*)"bRecTrace = %s", BOOL2STR(bRecTrace) );
        }
        break;

    case 'D':
        {
        if ( !bAfficheVec )
        {
            logf( (char*)"Key (A) : Pt1 Declinaison si les vecteurs sont affiches (y)" );
            break; 
        }

        logf( (char*)"Key (D) : Pt1 Declinaison" );
        Camera_mgr& 		cams = Camera_mgr::getInstance();

        if ( !bModeManuel )
        {
            vec2* pv = cams.getSuivi();
            if ( pv != NULL )
            {
                double x   = pv->x;
                double y   = pv->y;

                //change_joy( xSuivi, ySuivi );

                vecDC[1].x = x;
                vecDC[1].y = y;
                vecDC[1].z = 0.0;
            }
        }
        else
        {
            vecDC[1].x = xClick;
            vecDC[1].y = yClick;
            vecDC[1].z = 0.0;
        }
        var.set("vecDC[1].x", vecDC[1].x);
        var.set("vecDC[1].y", vecDC[1].y);
        logf( (char*)"DC[1] (%0.2f,%0.2f)", vecDC[1].x, vecDC[1].y );

      	Camera* pCam = cams.getCurrent();
        if ( pCam )
        {
        	string name = pCam->getName();
	        logf( (char*)"Camera : %s", name.c_str() );
		    var.set( name + "_vecDC[1].x", vecDC[1].x);
		    var.set( name + "_vecDC[1].y", vecDC[1].y);
	    
	    	pCam->setptDC1( vecDC[1] );
	        logf( (char*)"matrix = " MAT3_PRINTF , MAT3_AFF(pCam->get_matrix()) );
	    }

        compute_matrix();
        logf( (char*)"m3main = " MAT3_PRINTF , MAT3_AFF(mChange) );
        
        //bMouseDeplace = true;
        }
        break;

    case 'd':
        {
        if ( !bAfficheVec )
        {
            logf( (char*)"Key (A) : Pt0 Declinaison si les vecteurs sont affiches (y)" );
            break; 
        }

        logf( (char*)"Key (d) : Pt0 Declinaison" );
        Camera_mgr& 		cams = Camera_mgr::getInstance();
        
        if ( !bModeManuel )
        {
            vec2* pv = cams.getSuivi();
            if ( pv != NULL )
            {
                double x   = pv->x;
                double y   = pv->y;

                //change_joy( xSuivi, ySuivi );

                vecDC[0].x = x;
                vecDC[0].y = y;
                vecDC[0].z = 0.0;
            }
        }
        else
        {
            vecDC[0].x = xClick;
            vecDC[0].y = yClick;
            vecDC[0].z = 0.0;
        }
        var.set("vecDC[0].x", vecDC[0].x);
        var.set("vecDC[0].y", vecDC[0].y);
        logf( (char*)"DC[0] (%0.2f,%0.2f)", vecDC[0].x, vecDC[0].y );

      	Camera* pCam = cams.getCurrent();
        if ( pCam )
        {
        	string name = cams.getCurrent()->getName();
	        logf( (char*)"Camera : %s", name.c_str() );
		    var.set( name + "_vecDC[0].x", vecDC[0].x);
		    var.set( name + "_vecDC[0].y", vecDC[0].y);

	    	pCam->setptDC0( vecDC[0] );
	    }

        char cmd[255];
        sprintf( cmd, "d1000p" );
        logf( (char*)"Envoi de la commande : %s",  cmd );
        Serial::getInstance().write_string(cmd);
        }
        break;

    case 'e':
        {
        logf( (char*)"Key (e) : Affiche les traces" );
        bAffTrace = !bAffTrace;
        logf( (char*)"  bAffTrace = %s", BOOL2STR(bAffTrace) );
        }
        break;

    case 'E':
        {
        logf( (char*)"Key (E) : Efface les traces" );

        int i = t_vTrace.size();
        if ( i <= 0 ){
            logf( (char*)"  derniere atteinte" );
            break;
        }
        vector<vec2>* trace = t_vTrace[i-1];
        trace->clear();
        delete trace;
        t_vTrace.pop_back();
        trace = 0;
        logf( (char*)"  reste : %d trace", t_vTrace.size() );
        }
        break;


    case 'f':  // '-'
        {

			Serveur_mgr& serv = Serveur_mgr::getInstance();
			serv.affiche_azim( {pos_telescope.ad, pos_telescope.dc} );

			bCoordHoriz = !bCoordHoriz;
			
	        VarManager::getInstance().set( "bCoordHoriz", bCoordHoriz );

	        logf( (char*)"Key (f) : Coordonne azimutales %s", BOOL2STR(bCoordHoriz) );

			change_ad_status( pos_telescope.ad );
			change_dc_status( pos_telescope.dc );

        	logf( (char*)"WindowsManager NbTextures %d", wm.getNbTextures() );
        }
        break;

    case 'F':  // '-'
        {
        //bSimu = !bSimu;
        logf( (char*)"Key (F) : Simu pour les courbes de suivis (desactive) %s", BOOL2STR(bSimu) );
        VarManager::getInstance().set( "bSimu", bSimu );
        }
        break;

    case 'g':
        {
            filtre *= 0.9;
            if ( filtre<2.0)    filtre = 2.0;
            VarManager::getInstance().set( "filtre", filtre );
            logf( (char*)"Key (g) : filtre -10%% %0.2f", filtre );
        }
        break;
    case 'G':
        {
            filtre /= 0.9;
            if ( filtre>250.0)    filtre = 250.0;
            VarManager::getInstance().set( "filtre", filtre );
            logf( (char*)"Key (g) : filtre +10%% %0.2f", filtre );
        }
        break;
        
    case 'H':
        {
        logf( (char*)"Key (H) : Charge les images en continue de la camera courante" );
        Camera* p = Camera_mgr::getInstance().getCurrent();
        if ( p!=NULL )
        {
		if ( typeid(*p) == typeid(CameraZWO) )
			((CameraZWO*)p)->video();	        
        }
        }
        break;

    case 'h':
        {
        logf( (char*)"Key (h) : Charge une image de la camera courante" );
        Camera* p = Camera_mgr::getInstance().getCurrent();
        if ( p!=NULL )
        {
			p->enregistre();
	        
        }
        }
        break;

    case 'i':
        {
        	print_all_panels();
        /*
        	string name = "";
	        Camera* p = Camera_mgr::getInstance().getCurrent();
        	if( p ){
        		name = p->getPanelCamera()->getExtraString();
        	}
        	logf( (char*)name.c_str() );
        */
        }
        break;

    case 'I':
        {
        bInverseCouleur = !bInverseCouleur;
        logf( (char*)"Key (I) : Inversion de couleur on/off  %s ", BOOL2STR(bInverseCouleur) );
        var.set( "bInverseCouleur", bInverseCouleur );
        }
        break;
	case 'J':
		{
		    logf( (char*)"Key (Q) : Lance ASI Studio");
		    thread( &commande_asi_studio).detach();
        }
		break;
	case 'j':
		{
			bAffFitsCorrection = !bAffFitsCorrection;
			var.set("bAffFitsCorrection", bAffFitsCorrection );
		    logf( (char*)"Key (j) : bAffFitsCorrection = %s", BOOL2STR(bAffFitsCorrection) );
			Capture* p = Captures::getInstance().getCurrentCapture();
			if ( p && !p->isIconized() )	{
				p->onTop();
			}
        }
		break;
    case 'K':
        {
        logf( (char*)"Key (K) : Suivi on/off" );
            bSuivi = !bSuivi;
            var.set( "bSuivi", bSuivi );
            change_joy( xSuivi, ySuivi );
            logf( (char*)"  bSuivi (%s)", BOOL2STR(bSuivi) );
        }
        break;
    case 'k':
        {
        bAsservissement = !bAsservissement; 
        logf( (char*)"Key (k) : Asservissement de monture %s", BOOL2STR(bAsservissement) );
        set_asservissement();
        if ( bAsservissement ) 		cam_mgr.setAsserv( cam_mgr.getCurrent() );
        else					cam_mgr.setAsserv( NULL );
        
        setColor();
        }
        break;
	/*
	*/
    case 'l':
        {            
        logf( (char*)"Key (l) : Affiche les connexions" );
        logf( (char*)"---- Reseau -------" );

        LX200::getInstance().print_list();
        SYNSCAN::getInstance().print_list();
        Serveur_mgr::getInstance().print_list();
        Connexion_mgr::getInstance().print_list();
        Camera_mgr::getInstance().print_list();
        Captures::getInstance().print_list();
        Serial::getInstance().print_list();
        }
        break;

    case 'L':
        {            
        logf( (char*)"Key (L) : Affiche les variables" );
        VarManager::dbMap & db = var.getDB();
        VarManager::dbMap::iterator p;
        
        for(p = db.begin(); p!=db.end(); ++p)
        {
            switch(p->second.type)
            {
            case 'f':
                {
                double f = var.getf(p->first);
                logf( (char*)"%s -> %f", p->first.c_str(), f );
                }
                break;
            case 'i':
                {
                int i = var.geti(p->first);
                logf( (char*)"%s -> %d", p->first.c_str(), i );
                }
                break;
            case 'b':
                {
                bool b = var.getb(p->first);
                if (b)                      logf( (char*)"%s -> %s", p->first.c_str(), (char*)"TRUE" );
                else                        logf( (char*)"%s -> %s", p->first.c_str(), (char*)"FALSE" );
                break;
                }
            case 's':
                {
                string* s = var.gets(p->first);
                logf( (char*)"%s -> \"%s\"", p->first.c_str(), (*s).c_str() );
                break;
                }
             }
        }
            
        }
        break;

    case 'M':
        {
        logf( (char*)"Key (M) : Calcul la matrice de transformation" );
        compute_matrix();
        }
        break;
    case 'm':
        {
        logf( (char*)"Key (O) : Mode manuel (%s)", BOOL2STR(bModeManuel) );
        set_mode(!bModeManuel);
        }

        break;

    case 'n':
        {
	        logf( (char*)"Key (n) : Affiche GAIA");
	        log_tab(true);
		    Captures& caps = Captures::getInstance();
		    if ( caps.isMouseOverCapture(x, y)  )
		    {
		    	Capture*  p = Captures::getInstance().getCurrentCapture();
		    	if ( p && p->isFits() )	{
		    		p->getPanelCapture()->findGaiaDR3();
		    	}
		    }
		    
		    else
		    {
		        logf( (char*)"[Warning]Fenetre camera");
		        if ( Camera_mgr::getInstance().getCurrent() != NULL )
				    Camera_mgr::getInstance().getCurrent()->chargeGaiaDR3();
			}
	        log_tab(false);
        }
        break;

    case 'N':
        {
	        logf( (char*)"Key (N) : Efface les etoiles GAIA");
	        log_tab(true);
		    Captures& caps = Captures::getInstance();
		    if ( caps.isMouseOverCapture(x, y)  )
		    {
		    	Capture*  p = Captures::getInstance().getCurrentCapture();
		    	if ( p && p->isFits() )	{
		    		p->getPanelCapture()->eraseGaiaDR3();
		    	}
		    }
		    
		    else
		    {
		        logf( (char*)"[Warning]Fenetre camera");
		        if ( Camera_mgr::getInstance().getCurrent() != NULL )
				    Camera_mgr::getInstance().getCurrent()->eraseGaiaDR3();
		    }
		    

	        log_tab(false);
        }
        break;

    case 'o':
        {
            logf( (char*)"Key (o) : Affiche/cache la simu pleiade");
            if ( !bPleiade )
            {
                if ( pPleiade == NULL )             pPleiade = new Pleiade();
                Camera_mgr::getInstance().add( pPleiade );
                bPleiade = true;
				var.set("bPleiade", bPleiade );
                //bOneFrame = true;
            }
            else
            {
                //*
                Camera_mgr::getInstance().sup( pPleiade );
                bPleiade = false;
				var.set("bPleiade", bPleiade );
                pPleiade = NULL;
                //delete pPleiade;
                //*/
            }
        }
    	break;

    case 'O':
        {
	    logf( (char*)"Key (m) : Autorisation du deplcament par la souris (bouton du milieu) ...");

        if ( !bModeManuel )	        	bModeManuel = true;

	    bMouseDeplace = !bMouseDeplace;
	    var.set( "bMouseDeplace", bMouseDeplace );
	    logf( (char*)"  bMouseDeplace = %s", BOOL2STR(bMouseDeplace) );

	    if ( bMouseDeplace )			bMouseDeplaceVers = false;

        }

    	break;

    case 'p':  // '-'
        {
        updatePanelPause(!bPause);
        logf( (char*)"Key (p) : Pause on/off (%s)", BOOL2STR(bPause));
        }
        break;

    case 'P':  // '-'
        {
		    logf( (char*)"Key (P) : Une image");
		    updatePanelPause(true);
		    bOneFrame = true;
        }
        break;

    case 'Q':
        {
        	logf( (char*)"Lance Stellarium ..." );
            thread( &commande_stellarium).detach();
        }
        break;

    case 'q':
        {
	        logf( (char*)"Lance Polaris ...");
            thread( &commande_polaris).detach();
        }
        break;
    case 'r' :
        {
            logf( (char*)"Key (r) : Ouvre un fichier de suivi (.guid)");
            FileBrowser& fb = FileBrowser::getInstance();
            
            fb.setCallBack(&cb_cguidage);
            fb.setFiltre( ".guid" );
            fb.setTitle( "Charge un fichier de guidage" );
            fb.loadSkin( PanelWindow::BLUE );
            fb.setBorderSize( 6 );
            fb.setSTitle( "Selectionnez un fichier .guid" );
            if ( var.existe("DirSauveCourbe") )     workDirSauveCourbe = *var.gets( "DirSauveCourbe" );
            fb.change_dir( workDirSauveCourbe );
            fb.setNewline( true );
            fb.affiche();
        }
        break;

    case 'R' :
        {
        logf( (char*)"Key (R) : Reset les datas de suivi" );
        panelCourbe->reset_guidage();
        var.set( "FileResultat", string("---"));
        }
        break;

    case 's':
        {
            logf( (char*)"Key (s) : Trouve toutes les etoies" );
            log_tab(true);
            
            if ( Captures::getInstance().isMouseOverCapture(x, y)  )
            {
                Captures::getInstance().findAllStar();
            }
            else
            {
                if ( Camera_mgr::getInstance().getCurrent() != NULL )
                {
                    if ( Camera_mgr::getInstance().getCurrent()->getNbStars() == 0 )
                        Camera_mgr::getInstance().findAllStars();
                    else
            		    Camera_mgr::getInstance().deleteAllStars();
        		}
            }
            log_tab(false);
        }
        break;

    case 'S' :
        {
			logf( (char*)"Key (S) : Efface les etoiles !!" );
		    Captures& cap = Captures::getInstance();
		    if ( cap.isMouseOverCapture(x, y)  )
		    {
		        cap.deleteAllStars();
		    }
		    else
		    {
		        if ( Camera_mgr::getInstance().getCurrent() != NULL )
				    Camera_mgr::getInstance().deleteAllStars();
		    }
        }
        break;
	case 't':
		{
	        logf( (char*)"Key (t) : Compaaraison etoiles");
	        log_tab(true);
        	Captures& 	caps 	= Captures::getInstance();
        	Capture* 	cap		= caps.getCurrentCapture();
        	
        	if ( cap )	
        	{
				Stars* stars = cap->getPanelCapture()->getStars();
				if ( stars )
				{
					double A = stars->getA() / 1.1;
					stars->setA( A );
					//logf( (char*)"Coef A=%0.2lf", A );
					bDesactiveLog = true;
					cap->compareStar();
					cap->compareStar();
					bDesactiveLog = false;
					cap->compareStar();
					//cap->update_info_graph();
				}
        	}
	        log_tab(false);
        }
		break;
	case 'T':
		{
	        logf( (char*)"Key (T) : Compaaraison etoiles");
	        log_tab(true);
        	Captures& 	caps 	= Captures::getInstance();
        	Capture* 	cap		= caps.getCurrentCapture();
        	
        	if ( cap )	
        	{
				Stars* stars = cap->getPanelCapture()->getStars();
				if ( stars )
				{
					double A = stars->getA() * 1.1;
					stars->setA( A );
					logf( (char*)"Coef A=%0.2lf", A );
					bDesactiveLog = true;
					cap->compareStar();
					cap->compareStar();
					bDesactiveLog = false;
					cap->compareStar();
					//cap->update_info_graph();
				}
        	}
	        log_tab(false);
        }
		break;
	/*
	*/
    case 'U':
        {
        logf( (char*)"Key (U) : Affichage du centre de la camera on/off");
        bAffCentre = !bAffCentre;
        var.set( "bAffCentre", bAffCentre );
        }
        break;
    case 'u':
        {
        logf( (char*)"Key (u) : Affichage du suivi on/off");
        bAffSuivi = !bAffSuivi;
        var.set( "bAffSuivi", bAffSuivi );
        }
        break;

    case 'v':
        {
        if ( !bSauve ) 
        {
            FileBrowser& fb = FileBrowser::getInstance();

            bFileBrowser = fb.getVisible();
            fb.setFiltre( ".guid" );
            fb.setTitle( "Sauve un fichier de guidage" );
            fb.loadSkin( PanelWindow::BLUE );
            fb.setBorderSize( 6 );
            fb.setSTitle( "Sauvegarde un fichier .guid" );
            fb.change_dir( workDirSauveCourbe );
            fb.setCallBack( &cb_sguidage );
            fb.setNewline( true );
            
            fb.affiche();
            
            logf( (char*)"Sauvegarde !!!" );
        }
        else
        {
            bSauve = false;
            logf( (char*)"Arret sauvegarde !!!" );
        }
        }
        break;

    case 'V':
        {
            if (bModeManuel)
            {
                int x = xClick;
                int y = yClick;
                //tex2screen(x,y);

				vec2 vSuivi = vec2( x, y);

                var.set("xSuivi", vSuivi.x );
                var.set("ySuivi", vSuivi.y );

                panelCourbe->get_vOrigine().x = x;
                panelCourbe->get_vOrigine().y = y;
                panelCourbe->get_vOrigine().z = 0.0;
                
                change_joy( x, y );

                logf( (char*)"initialise vOrigine(click) : (%d,%d)", x, y);
                Camera_mgr::getInstance().setRefSuivi( vSuivi );
            }
            else
            {
                Camera_mgr& cams = Camera_mgr::getInstance();
            	if ( cams.getCurrent() != NULL )
            	{
		            vec2* pv = cams.getCurrent()->getPanelCamera()->getStars()->getSuivi();
					
		            if ( pv != NULL )
		            {
						vec2 vSuivi = *pv;
		                xSuivi = pv->x;
		                ySuivi = pv->y;


		                change_joy( xSuivi, ySuivi );

		                var.set("xSuivi", xSuivi );
		                var.set("ySuivi", ySuivi );
		                
			            Camera_mgr::getInstance().setRefSuivi( vSuivi );
			            logf( (char*)"Init reference suivi  : " VEC2_PRINTFN(0), VEC2_AFF(vSuivi) );

						// mise a jour de panelCourbe et convertion en vec3
				        panelCourbe->get_vOrigine() = vec3( vSuivi, 0.0 );
		            }
		            else
		            {
			            logf( (char*)"Pas d'etoiles de suivi (click milieu sur l'etoile)" );
		            }
				}                
            }
            bCentrageSuivi = false;
        }
        break;
    case 'w' :
        {
            //BluetoothManager::getInstance().centre_joystick();
            bCentrageSuivi = !bCentrageSuivi;
            if ( bCentrageSuivi )
            {
                // Sauvegarde dans xClick et yClick
                xSuiviSvg = xSuivi;
                ySuiviSvg = ySuivi;

                xSuivi = (double)wm.getWidth()/2;
                ySuivi = (double)wm.getHeight()/2;

                Camera_mgr::getInstance().screen2tex(xSuivi, ySuivi);
            }
            else
            {
                // Restauration de xSuivi et ySuivi
                xSuivi = xSuiviSvg;
                ySuivi = ySuiviSvg;
            }

            change_joy( xSuivi, ySuivi );
            //bModeManuel = true;

            logf( (char*)"initialise vOrigine(recentrage) : (%0.2f,%0.2f)", xSuivi, ySuivi);
        }
        break;

    case 'W' :
        {
            Surveillance& su = Surveillance::getInstance();
            
            FileBrowser& fb = FileBrowser::getInstance();

            bFileBrowser = fb.getVisible();
            fb.setFiltre( "" );
            fb.setTitle( "Surveille un repertoire" );
            fb.loadSkin( PanelWindow::ORANGE );
            fb.setBorderSize( 6 );
            fb.setSTitle( "Selectionnez le repertoire de surveillance" );
            fb.setNewline( true );
            if ( var.existe("DirSurveillance") )
            {
                workDirCaptures = *var.gets( "DirSurveillance" );
            }
            fb.change_dir( workDirCaptures );
            fb.setCallBack( &su );
            fb.setExtra( 12 );

            fb.affiche();

        }
        break;
    case 'x':
        {
        logf( (char*)"Key (x) : Graphique etoile" );
        log_tab(true);
        	Captures& 	captures 	= Captures::getInstance();
        	Capture* 	capture		= captures.getCurrentCapture();
        	bool		bGraph;
        	
        	if ( capture )	
        	{
				logf( (char*)"bGraph = %s", BOOL2STR(capture->getGraph()) );
        		capture->setAffGraph(true);
		    	capture->create_graph();
        	}
        log_tab(false);
        }
        break;

    case 'X':
		{
        logf( (char*)"Key (X) : Fermeture graphique etoile" );
        log_tab(true);
        	Captures& 	captures 	= Captures::getInstance();
        	Capture* 	capture		= captures.getCurrentCapture();
        	bool		bGraph;
        	
        	if ( capture )	
        	{
				logf( (char*)"bGraph = %s", BOOL2STR(capture->getGraph()) );
        		capture->deleteGraph();
        	}
        log_tab(false);
		}
        break;
    case 'y':
        {
        bAfficheVec = !bAfficheVec;
        var.set("bAfficheVec", bAfficheVec);
        logf( (char*)"Key (y) : Affiche les Vecteur AD et DC %s", BOOL2STR(bAfficheVec) );
        }
        break;

    case 'Y':
        {
        }
        break;

    case 'z':
        {
        logf( (char*)"Key (z) : Charge les traces" );
        charge_traces();
        }
        break;

    case 'Z':
        {
        logf( (char*)"Key (Z) : Sauve les traces" );
        sauve_traces();
        }
        break;

    default:
        {
        //if ( key != 14 )
            logf((char*)"main::KeyboardFunc() key: %d", key);
        }
        break;
	}
	
	//Camera_mgr::getInstance().keyboard( key );
    onTop();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void KeyboardUpFunc(unsigned char key, int x, int y)
{
	#ifdef DEBUG_CALLBACK_OPENGL
		logf( (char*)"    %02d\t%.8f KeyboardUpFunc", ++appelIdle, -timerAppelIdle +Timer::getInstance().getGlutTime() );
	#endif

	WindowsManager::getInstance().keyboardUpFunc( key, x, y);
    //WindowsManager::getInstance().onBottom(panelPreView);
    onTop();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void SpecialFunc(int key, int x, int y )
{
	#ifdef DEBUG_CALLBACK_OPENGL
		logf( (char*)"    %02d\t%.8f SpecialFunc", ++appelIdle, -timerAppelIdle + Timer::getInstance().getCurrentTime() );
	#endif

	VarManager& var = VarManager::getInstance();
	
	WindowsManager::getInstance().keyboardSpecialFunc( key, x, y);
    Captures::getInstance().glutSpecialFunc(key, x, y);
    Camera_mgr::getInstance().glutSpecialFunc(key, x, y);
    
    
    switch( key)
    {
    case GLUT_KEY_F1:
        {
        bPanelHelp = !bPanelHelp;
        var.set("bPanelHelp", bPanelHelp);
        logf( (char*)"Key F1: Toggle panelHelp %s", BOOL2STR(bPanelHelp) );
        log_tab( true );

        panelHelp->setVisible(bPanelHelp);
        if ( bPanelHelp )       WindowsManager::getInstance().onTop(panelHelp);

        log_tab( false );
        }
        break;
	case GLUT_KEY_F2:
        {
        log( (char*)"Key F2: Toggle panelCamera " );
        log_tab( true );
	        Camera_mgr::getInstance().togglePanel();
        log_tab( false );
        }
        break;
    case GLUT_KEY_F3:
        {
        bPanelResultat = !bPanelResultat;
        var.set("bPanelResultat", bPanelResultat);
        logf( (char*)"Key F3: Toggle panelResultat %s", BOOL2STR(bPanelResultat) );
        log_tab( true );

        panelResultat->setVisible(bPanelResultat);
        if ( bPanelResultat )       WindowsManager::getInstance().onTop(panelResultat);

        log_tab( false );
        }
        break;
    case GLUT_KEY_F4:
        {
        bPanelCourbe = !bPanelCourbe;
        var.set("bPanelCourbe", bPanelCourbe);
        logf( (char*)"Key F4: Toggle panelCourbe %s", BOOL2STR(bPanelCourbe) );
        log_tab( true );

        panelCourbe->setVisible(bPanelCourbe);
        set_courbe();
        
        if ( bPanelCourbe )       WindowsManager::getInstance().onTop(panelCourbe);

        log_tab( false );
        }
        break;
    case GLUT_KEY_F5:
        {
        bPanelStdOut = !bPanelStdOut;
        var.set("bPanelStdOut", bPanelStdOut);
        logf( (char*)"Key F5: Toggle panelStdOut %s", BOOL2STR(bPanelStdOut) );
        log_tab( true );

        if ( panelStdOut )		panelStdOut->setVisible(bPanelStdOut);
        if ( panelStdOut )		WindowsManager::getInstance().onTop(panelStdOut);

		LX200& lx200 = LX200::getInstance();

        log_tab( false );
        }
        break;
    case GLUT_KEY_F6:
        {
        bPanelSerial = !bPanelSerial;
        var.set("bPanelSerial", bPanelSerial);
        logf( (char*)"Key F6: Toggle serial (%s)", BOOL2STR(bPanelSerial) );
        log_tab( true );

        PanelConsoleSerial::getInstance().setVisible(bPanelSerial);

        log_tab( false );
        }
        break;
    case GLUT_KEY_F7:
        {
        bAffIconeCapture = !bAffIconeCapture;
        var.set( "bAffIconeCapture", bAffIconeCapture );
        logf( (char*)"Key F7: Affiche/cache les captures (bAffIconeCapture = %s)", BOOL2STR(bAffIconeCapture));

        log_tab( true );
        Captures::getInstance().switchAffIcones();
        log_tab( false );
        }
        break;
    case GLUT_KEY_F8:
        {
        if ( panelEnregistre == NULL )
        {
        	panelEnregistre = new PanelEnregistre();
        	panelEnregistre->setVisible(false);
        }
		logf( (char*)"Key F8: Toggle panelAPN", BOOL2STR(!panelEnregistre->getVisible()) );
    	log_tab( true );

		panelEnregistre->setVisible( !panelEnregistre->getVisible() );
		var.set( "bPanelEnregistre", panelEnregistre->getVisible()  );

	    log_tab( false );

        }
        break;

    case GLUT_KEY_F10:
        {
        bDebug = !bDebug;
        WindowsManager::getInstance().debug(bDebug);
        }
        break;
    case GLUT_KEY_F11:
        {
		logf( (char*)"Key F11: Affiche l'image suivante" );
		log_tab( true );
        Captures::getInstance().ajoute();
		log_tab( false );
        }
        break;
    case GLUT_KEY_F12:
        {
		logf( (char*)"Key F12: Efface l'image courante" );
		log_tab( true );
        Captures::getInstance().supprime();
		log_tab( false );
        }
        break;
        
    case GLUT_KEY_SHIFT_L:
    case GLUT_KEY_SHIFT_R:
    case GLUT_KEY_CTRL_L:
    case GLUT_KEY_CTRL_R:
    case GLUT_KEY_ALT_L:
    case GLUT_KEY_ALT_R:
        break;
	//----------------------------------------------------------
	// Touche HOME
    case 106:
    	{
        logf( (char*)"Key HOME : Camera fullscreen" );
	    Camera_mgr&     mgr = Camera_mgr::getInstance(); 
	    mgr.cam_full_screen();
	    }
        break;
        
    default:	
	    {
        logf( (char*)"main::SpecialFunc default (0x%X) \'%c\'", (int)key, key );

		}
		break;
		
    }



	//WindowsManager::getInstance().keyboardSpecialFunc( key, x, y);
    //WindowsManager::getInstance().onBottom(panelPreView);
    onTop();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void SpecialUpFunc(int key, int x, int y)
{
	#ifdef DEBUG_CALLBACK_OPENGL
		logf( (char*)"    %02d\t%.8f SpecialUpFunc", ++appelIdle, -timerAppelIdle +Timer::getInstance().getGlutTime() );
	#endif

	WindowsManager::getInstance().keyboardSpecialUpFunc( key, x, y);
    onTop();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

