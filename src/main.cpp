#ifndef MAIN_CPP
#define MAIN_CPP

#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

//#include "Python.h"

#include "main.h"
#include "camera_v4l2.h"
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
#include <GL/freeglut_ext.h>
#include <mutex>
#include "star_catalogue.h"
#include "catalog.h"
#include "MathAstro.h"
#include "keyboard.h"
#include "ctools.h"
//-----------------------------------------------------------------
//-----------------------------------------------------------------
#define SIZEPT  20
//--------------- DEBUG -------------------------------------------
//#define DEBUG 1
//#define IDLEGL
//#define DEBUG_CALLBACK_OPENGL // determine l'ordre d'appel des fonctions GL
#ifdef DEBUG_CALLBACK_OPENGL
	volatile float timerAppelIdle;
#endif
/*
log  :     01	0.00000000 idleGL:timer
log  : 			0.00000000
log  :     02	0.00000000 idleStatus
log  : 			0.00299978
log  :     03	0.00299978 Camera_mgr, Captures
log  : 			0.00299978
log  :     04	0.00299978 WindowsManager::idleGL()
log  : 			0.00399971
log  :     05	0.00399971 glutPostRedisplay
log  : 			0.00399971
log  :     06	0.00399971 glutPassiveMotionFunc(761, 661)
log  :     07	-1.02300000 glutSpecialFunc
log  :     08	0.00399971 glutPassiveMotionFunc(821, 661)
log  :     09	0.00399971 glutKeyboardUpFunc
log  :     10	0.00399971 glutKeyboardFunc
log  :     11	0.00399971 glutKeyboardFuncCtrl
log  :     12	0.00399971 glutPassiveMotionFunc(881, 665)
log  :     13	0.00399971 displayGL() clearBuffer()
log  :	 		0.00399971
log  :     14	0.00399971 glutSwapBuffers()
log  : 			0.01599979
log  :     15	0.01599979 idleGL
*/
//-----------------------------------------------------------------
#define COLOR_GREY			0x404040FF
#define COLOR_WHITE			0xFFFFFFFF
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
vector<string> t_sHelp1 =
{
	"---- TOUCHE DE FONCTION ----",
	"    F1\t: Help",
	"    F2\t: Panneau de controle de la camera",
	"    F3\t: Panneau de suivi",
	"    F4\t: Courbe de suivi",
	"    F5\t: Console de log",
	"    F6\t: Console arduino",
	"    F7\t: Affiche/Cache les images",
	"    F8\t: Parametre APN",
	"   F10\t: Mode DEBUG WindowManager",
	"   F11\t: Charge la prochaine image",
	"   F12\t: Efface la derniere image", 
	"    CR\t: Plein Ecran",
	"",
	"---- MODE NORMAL ----",
	"ctrl+TAB\t: camera suivante" ,
	"TAB\t: Change l'affichage des fichiers" ,
	" Alt+A\t: Arduino bavard",
	"   b/B\t: Affiche les informations des fichiers fits",
	"     f\t: Affiche les coordonnes azimutales",
	"     F\t: Active/Desactive la simu",
	"     i\t: Prend une photo sur le PENTAX",
	"     I\t: Inverse les couleur pour la recherhce d'une etoile",
	"     j\t: Affichage info fits",
	"     J\t: Lance ASI Studio",
	"     l\t: List les ports /dev + les controles ",
	"     L\t: List les variables",
	"Ctrl+N\t: Mode nuit on/off",
	"     n\t: Interroge Vizier",
	"     N\t: Efface les etoiles Vizier",
	"     o\t: Ouvre/Ferme la fenetre pleiades",
	"Ctrl+o\t: Ouvrir un fichier image",
	"     p\t: Pause de l'affichage camera",
	"     P\t: Image suivante",
	"     q\t: Lance Polaris",
	"     Q\t: Lance Stellarium",	
	"   t/T\t: Coefficient comparaison etoile",
	"     W\t: Surveille un repertoire",
	"     -\t: Toutes les images sont affichees en icones",
	"Alt+C \t: Affiche le deuxieme cercle (collimation)",
};
vector<string> t_sHelp2 = 
{
	"---- TRANSFORM MATRIX ----",
	"   a/A\t: Vecteur en ascension droite",
	"   d/D\t: Vecteur en declinaison",
	"     O\t: Deplacement à la souris",
	"     M\t: Calcul la matrice de transformation",
	"     m\t: Mode souris / mode suivi",
	"     y\t: Affiche les vecteurs",
	"",
	"Depl. souris :",
	"\tclick gauche positionne la reference",
	"\tclick et maintient milieu positionne le deplacement",
	"",
	"---- TRACES ----",
	"     C\t: Lance/arrete l\'enregistrement de trace",
	"     c\t: Nouvelle trace",
	"     e\t: Affiche les traces (ctrl+tab)",
	"     E\t: Supprime la derniere trace",
	"     z\t: Charge les traces",
	"     Z\t: Sauve les traces",
	"",
	"---- CAMERA ----",
	"     a\t: rappelle les parametres de la camera",
	"     A\t: enregistre les parametres de la camera",
	"     H\t: Start/Stop affiche la camera (video)",
	"     h\t: Enregistre une image de la camera courante",
	" alt+h\t: Enregistre une image en demandant le nom",
	"     l\t: Liste les controles",
	"   HOME\t: Fullscreen",
    "" ,    
};
vector<string> t_sHelp3 = 
{
    "---- Click Gauche ----",
	"Ctrl+Shift+Alt Click\t: Recherche tt les etoile v1 " ,
	"Shift+Alt      Click\t: Recherche tt les etoile v2" ,
	"Ctrl+Shift     Click\t: Affiche la distribution de la ligne" ,
	"Ctrl+Alt       Click\t: Affiche le graphisme flux" ,
    //"---- Click Droit ----",
    "",
	"Ctrl           Click\t: GOTO" ,
	"Alt            Click\t: SYNC" ,
};
vector<string> t_sHelp4 =
{
	"---- SUIVI ----",
	"     K\t: Lance/Stop le suivi",
	"     k\t: Lance l'asservissement",
	"     r\t: Charge ficher de suivi (.guid)",
	"     R\t: Reset Suivi (!!! Toute les data sont effacees)",
	"     s\t: Recherche toutes les etoiles",
	"     S\t: Efface toutes les etoiles",
    "     U\t: Affichage du centre de la camera on/off",
    "     u\t: Affichage du suivi on/off",
	" Alt+u\t: Suivi au centre de l'ecran",
	"     V\t: Initialise les coordonnees de suivi",
	"     v\t: Sauvegarde fichier de suivi (.guid)",
	"     w\t: Centre l'asservissement",
	"",
	"---- FFT ----",
	"   g/G\t: Change la valeur du filtre"   ,
	"     x\t: Affiche/Cache la fft",
	"     X\t: Choix de l'abcisse ou de l'ordonnée",
	"",
	"---- Vizier ----",
	" Alt+e\t: Affiche catalog (CDS)"   ,
	" Alt+r\t: Affiche les etoiles"   ,
	" Alt+t\t: Affiche les etoiles FindStar"   ,
	"   a/z\t: Rotation"   ,
	"   w/x\t: Translation X"   ,
	"   c/v\t: Zoom X"   ,
	"   q/s\t: Translation Y"   ,
	"   d/f\t: Zoom Y"   ,
	" Alt+n\t: Compare les etoiles avec le catalogue Vizier"   ,
	" Alt+N\t: Compare les etoiles et  affiche la courbe"   ,
	"Ct+Al+n\t: Limite la magnitude d'affichage"   ,
	"Ctrl+H\t: Restaure la magnitude d'affichage a 20.0"   ,
	"   x/X\t: Affiche/Ferme la courbe de calcul de la magnitude"   ,
	"",
	"Alt+l/L\t: Correction du calcul magnitude"   ,
	"Alt+k/K\t: Correction du calcul magnitude"   ,
	"",
	"---- DEBUG ----",
	" Alt+g\t: Info fits"   ,
	"",
	"",
	"Ctrl+q\t: --- SORTIE DU LOGICIEL ---",
};
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
VarManager& var = VarManager::getInstance();
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
string              currentDirectory = "/home/rene/Documents/astronomie/logiciel/script/images/";
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelWindow *       panelHelp		= NULL;
PanelScrollY *      panelScrHelp	= NULL;
PanelWindow *       panelResultat	= NULL;
PanelCourbe *       panelCourbe		= NULL;
PanelStdOut *       panelStdOut		= NULL;
PanelEnregistre*    panelEnregistre	= NULL;
PanelSimple *       panelStatus		= NULL;


PanelText*          pCamFilename;
PanelText*          pRef;
PanelText*          pEtoile;
PanelText*          pEcart;
PanelText*          pCoordSuivi;
PanelText*          pSuivi;
PanelText*          pArduino;
PanelText*          pStellarium;
PanelText*          pPas;
PanelText*          pDeplacement;
PanelText*          pRacket;
PanelText*          pAD;
PanelText*          pDC;
PanelText*          pAsservi;
PanelText*          pFramesCapture;

PanelText*          pHertz;
PanelText*          pFPS;

PanelText *			pAlpha;
PanelText *			pDelta;
PanelText *			pAzi;
PanelText *			pHau;

Pleiade*            pPleiade = NULL;

double				dLatloc		= 0.0;
double				dLonloc		= 0.0;

double              ac;
double              dc;
int					appelIdle = 0;
int					posStatusX = 0;
char                background[]="frame-0.raw";
double				prevTime    = -1.0f;

int                 width  = 1600;
int                 height = 900;
int                 widthScreen  = 0;
int                 heightScreen = 0;

int                 xPos;
int                 yPos;

GLdouble            fov = 60.0;

char                dev_name[] = "/dev/video0000";

bool                bPrintLum = true;

bool                bPng    = false;
bool                bFull   = false;
bool                bPause  = false;
bool                bSuivi  = false;
bool                bModeManuel = false;
bool                bNuit   = false;
bool                bDebug  = false;
bool                bArretUrgence;
bool                bAffIconeCapture = true;;


bool                bPanelHelp     		= false;
bool                bPanelResultat 		= false;
bool                bPanelCourbe   		= false;
bool                bPanelStdOut   		= false;
bool                bPanelSerial		= false;
bool                bPanelEnregistre	= false;
bool                bAfficheVec			= false;
bool                bMouseDeplace		= false;
bool                bMouseDeplaceVers	= false;
bool                bFileBrowser		= false;
bool                bStellarium			= false;
bool                bArduino			= false;
bool                bPleiade			= false;
bool                bSauve				= false;
bool                bOneFrame			= false;
bool                bStdOut				= false;
bool                bSimu				= false;
bool                bAffCentre			= false;
bool                bAffSuivi			= true;
bool                bSound				= true;
bool                bInverseCouleur		= false;
bool                bCentrageSuivi		= false;
bool                bFirstStart			= true;
bool				bDesactiveLog		= false;
bool				bAffColimation		= false;
bool				bAffFindStar		= false;
bool				bGmagChange			= false;
bool				bPleiadeVizier		= false;
bool				bCoordHoriz			= false;
bool				bCoordHoriz_old		= false;
bool				bAffFitsCorrection	= false;

int                 wImg;
int                 hImg;
int                 nPlanesImg;

vector<string>      exclude;

int                 _r[256];
int                 _g[256];
int                 _b[256];

double				xSuivi;
double				ySuivi;
double				xSuivi1;
double				ySuivi1;
double				xSuiviSvg;
double				ySuiviSvg;
double				filtre      = 10.0;
int                 iDisplayCourbe  = 0;
bool                bDisplayCourbeX = true;
bool                bDisplayCourbeY = true;
double				dErr;

int                 iDisplayfft = 0;
bool                bDisplayfftX = true;
bool                bDisplayfftY = true;

double				fTimeMili = 0.0;
vector<string>		logs_string;
mutex				m_logs_string;
//--------------------------------------------------------------------------------------------------------------------
//              Ratio Witdh Height
//--------------------------------------------------------------------------------------------------------------------
int                 xCam;
int                 yCam;
int                 dxCam;
int                 dyCam;

double               zoom;

ivec2               vMouse;

#define             GET_OFFSET(x,y,width)   (3*y*width+x)
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
vector<vector<vec2> * >        t_vTrace;
bool                bAffTrace = false;
bool                bRecTrace = false;
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
struct etoile       atlas   = { 3, 49, 9.7425852,  24, 3, 12.300277 };
struct etoile       electre = { 3, 44, 52.5368818, 24, 6, 48.011217 };

int                 xClick;
int                 yClick;

double				pas = 4000;
double				cAD;
ivec2               calibreMove[2];
vec2                vRef;
vec3                vecAD[2];
vec3                vecDC[2];
vec3                vDepl[2];
mat3                mChange;
vec3                vOri;
vec3                vTr;
bool                bAsservissement = false;
double				fTimeCpt = 0.0;
double              fTimeCorrection = 3.0;
double              fLimitCorrection0 = 80.0;
double              fLimitCorrection1 = 82.0;
double              pas_sideral;

st_eq				pos_telescope = { -1.0, -1.0 };
//--------------------------------------------------------------------------------------------------------------------
string              sTab;
int                 nb_tab = 0;
//vector<int>			tTab;
//bool				bChangeTab = false;
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
vector<AlertBox*>   tAlert;
bool                bAlert = false;
string              sAlert;
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
double				fTimer10s = 0.0;
bool				bAsc = true;
bool				bDec = true;
bool				bSui = false;
bool				bJoy = false;
bool				bRet = false;
//--------------------------------------------------------------------------------------------------------------------
double				Xref  = -1.0;
double				Yref  = -1.0;
double				ZrefX = -1.0;
double				ZrefY = -1.0;
double				Wref  = -1.0;
//--------------------------------------------------------------------------------------------------------------------
bool				bAffCatalog	= true;
bool				bAffStar		= true;
Catalog				vizier = Catalog();
int					nbVizier;

int					iGlutModifier = 0;
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
string				workDirCaptures = 		"/home/rene/Documents/astronomie/logiciel/script/images/";
string				workDirSauveCourbe =	"/home/rene/.astropilot/suivi/";
string				workDirFileBrowser =	"/home/rene/Documents/astronomie/logiciel/script/images/";
string				filenameSauve =			"/home/rene/.astropilot/sauvegarde.txt";
string				workDirFits =			"/home/rene/Documents/astronomie/fits/";
//--------------------------------------------------------------------------------------------------------------------
CallbackSauveGuidage    cb_sguidage;
CallbackChargeGuidage   cb_cguidage;
CallbackFileBrowser     cb_file_browser;
CallbackFits            cb_fits;
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
const char short_options[] = "d:hfs:Llope:";
const struct option
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
long_options[] = {
        { "device", required_argument, NULL, 'd' },
        { "help",   no_argument,       NULL, 'h' },
        { "format", no_argument,       NULL, 'f' },
        { "size",   required_argument, NULL, 's' },
        { "log",    no_argument,       NULL, 'l' },
        { "LOG",    no_argument,       NULL, 'L' },
        { "out",    no_argument,       NULL, 'o' },
        { "pleiade",no_argument,       NULL, 'p' },
        { "exclude",required_argument, NULL, 'e' },
        { 0, 0, 0, 0 }
};
//--------------------------------------------------------------------------------------------------------------------
//
//
//
//
//
//
//
//
//
//
//
//
//--------------------------------------------------------------------------------------------------------------------
void usage(FILE *fp, int argc, char **argv)
{
        fprintf(fp,
                 "Usage: %s [options]\n\n"
                 "Version 0.0.7\n"
                 "Options:\n"
                 "-d | --device name   Video device name [/dev/video(n)]\n"
                 "-h | --help          Print this message\n"
                 "-f | --full          Full size windows\n"
                 "-l | --log           Affiche les logs\n"
                 "-L | --LOG           Affiche les logs ds le programme\n"
                 "-o | --out           Affiche les logs ds la fenetre log\n"
                 "-e | --exc           Exclude device\n"
                 "",
                 argv[0] );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void arret_urgence()
{
	logf((char*)"**[ARRET D'URGENCE]**" );
	// arrete le suivi
	bSuivi = false;
	var.set( "bSuivi", bSuivi );
	change_joy( xSuivi, ySuivi );

	// envoi la commande arduino  arret urgence
	char cmd[]="n";

	Serial::getInstance().reset();
	Serial::getInstance().write_string(cmd);
	Serial::getInstance().reset();

	// Arrete l'asservissment
	bAsservissement = false;
	set_asservissement();
	Camera_mgr::getInstance().setAsserv( NULL );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void sound_alert()
{
	son( "/home/rene/.astropilot/sounds/alert.wav" );
	son( "/home/rene/.astropilot/sounds/alert.wav" );
	son( "/home/rene/.astropilot/sounds/alert.wav" );
	return;
    string cmd = "aplay /home/rene/.astropilot/sounds/alert.wav;";
    cmd = cmd;
    system( (char*)cmd.c_str() );
    system( (char*)cmd.c_str() );
    system( (char*)cmd.c_str() );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void commande_polaris()
{
    VarManager&         var = VarManager::getInstance();

    string filename = "/home/rene/Documents/astronomie/logiciel/python/polaris/polaris.py  >/dev/null 2>&1 &";
    string command = "";

    command = command + filename;


    logf( (char*) command.c_str() );

    
    int ret = system( (char*) command.c_str() );
    if ( ret != 0 )
    {
        string mes = "Erreur Pyhton polaris : " +  to_string(ret);
        alertBox(mes);
        logf( (char*)mes.c_str() );
    }
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void commande_stellarium()
{
    //VarManager&         var = VarManager::getInstance();
    string command = "stellarium >/dev/null 2>&1 &";
    logf( (char*) command.c_str() );
    int ret = system( (char*) command.c_str() );

    if ( ret != 0 )
    {
        string mes = "Erreur execution stellarium : " +  to_string(ret);
        alertBox(mes);
        logf( (char*)mes.c_str() );
    }
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void commande_asi_studio()
{
    //VarManager&         var = VarManager::getInstance();
    string command = "/home/rene/programmes/shell/asi >/dev/null 2>&1 &";
    logf( (char*) command.c_str() );
    int ret = system( (char*) command.c_str() );

    if ( ret != 0 )
    {
        string mes = "Erreur execution asi_studio : " +  to_string(ret);
        alertBox(mes);
        logf( (char*)mes.c_str() );
    }
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void commande_magnitude( string filename )
{
    //VarManager&         var = VarManager::getInstance();
	string command;
    if ( filename == "" )
	    command = "/home/rene/.astropilot/gnuplot/magnitude.sh magnitude.png >/dev/null 2>&1 &";
	else
	{
		std::ifstream fichier;
		fichier.open(filename, std::ios_base::app);

		if ( fichier ) 
		{
			fichier.close();
			char fn[256];
			snprintf( fn, sizeof(fn), "/home/rene/.astropilot/gnuplot/%s", filename.c_str() );
			logf( (char*)"Effacement de : %s", fn );
			std::remove( fn );
		}

	    command = "/home/rene/.astropilot/gnuplot/magnitude.sh magnitude_" +filename+ ".png >/dev/null 2>&1 &";
	}

    logf( (char*) command.c_str() );
    int ret = system( (char*) command.c_str() );

    if ( ret != 0 )
    {
        string mes = "Erreur execution magnitude.sh : " +  to_string(ret);
        alertBox(mes);
        logf( (char*)mes.c_str() );
    }
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void test_creer_dir( char* pathname )
{
	struct stat 	buf;

	if ( mkdir( pathname, 0755 ) != 0 )
	{
		switch( errno ) {
		case EEXIST:
			logf( (char*)"Le dossier \"%s\" existe", pathname );
			break;
		default:
			logf( (char*)"[ ERREUR ] Probleme avec le dosier \"%s\"", pathname );
			break;
		}
	}
	else
		logf( (char*)"Le dossier \"%s\" a ete cree", pathname );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool test_dir( char* pathname )
{
	struct stat 	buf;

	if ( stat( pathname, &buf ) != 0 )
	{
		logf( (char*)"Le dossier \"%s\" n'existe pas", pathname );
		return false;
	}
	logf( (char*)"Le dossier \"%s\" existe", pathname );
	return true;
}
//--------------------------------------------------------------------------------------------------------------------
//
/*
    RA_ICRS (deg)   (F15.11) Right ascension (ICRS) at Ep=2016.0 (ra) [ucd=pos.eq.ra;meta.main]
    DE_ICRS (deg)   (F15.11) Declination (ICRS) at Ep=2016.0 (dec) [ucd=pos.eq.dec;meta.main]
    Source          (I19)   Unique source identifier (unique within a particular Data Release) (source_id) [ucd=meta.id;meta.main]
    e_RA_ICRS (mas) (F7.4)  Standard error of right ascension (ra_error) [ucd=stat.error;pos.eq.ra]
    e_DE_ICRS (mas) (F7.4)  Standard error of declination (dec_error) [ucd=stat.error;pos.eq.dec]
    Plx (mas)       (F9.4)  ? Parallax (parallax) [ucd=pos.parallax.trig]
    e_Plx (mas)     (F7.4)  ? Standard error of parallax (parallax_error) [ucd=stat.error;pos.parallax.trig]
    PM (mas/yr)     (F9.3)  ? Total proper motion (pm) [ucd=pos.pm;pos.eq]
    pmRA (mas/yr)   (F9.3)  ? Proper motion in right ascension direction, pmRA*cosDE (pmra) [ucd=pos.pm;pos.eq.ra]
    e_pmRA (mas/yr) (F6.3)  ? Standard error of proper motion in right ascension direction (pmra_error) [ucd=stat.error;pos.pm;pos.eq.ra]
    pmDE (mas/yr)   (F9.3)  ? Proper motion in declination direction (pmdec) [ucd=pos.pm;pos.eq.dec]
    e_pmDE (mas/yr) (F6.3)  ? Standard error of proper motion in declination direction (pmdec_error) [ucd=stat.error;pos.pm;pos.eq.dec]
    RUWE            (F7.3)  ? Renormalised unit weight error (ruwe) [ucd=stat.error]
    FG (e-/s)       (F16.5) ? G-band mean flux (phot_g_mean_flux) [ucd=phot.flux;em.opt]
    e_FG (e-/s)     (E11.4) ? Error on G-band mean flux (phot_g_mean_flux_error) [ucd=stat.error;phot.flux;em.opt]
    Gmag (mag)      (F9.6)  ? G-band mean magnitude (phot_g_mean_mag) [ucd=phot.mag;em.opt]
    FBP (e-/s)      (E11.4) ? Integrated BP mean flux (phot_bp_mean_flux) [ucd=phot.flux;em.opt.B]
    e_FBP (e-/s)    (E11.4) ? Error on the integrated BP mean flux (phot_bp_mean_flux_error) [ucd=stat.error;phot.flux;em.opt.B]
    BPmag (mag)     (F9.6)  ? Integrated BP mean magnitude (phot_bp_mean_mag) [ucd=phot.mag;em.opt.B]
    FRP (e-/s)      (E11.4) ? Integrated RP mean flux (phot_rp_mean_flux) [ucd=phot.flux;em.opt.R]
    e_FRP (e-/s)    (E11.4) ? Error on the integrated RP mean flux (phot_rp_mean_flux_error) [ucd=stat.error;phot.flux;em.opt.R]
    RPmag (mag)     (F9.6)  ? Integrated RP mean magnitude (phot_rp_mean_mag) [ucd=phot.mag;em.opt.R]
    BP-RP (mag)     (F9.6)  ? BP-RP colour (bp_rp) [ucd=phot.color;em.opt.B;em.opt.R]
    RV (km/s)       (F7.2)  ? Radial velocity (radial_velocity) [ucd=spect.dopplerVeloc.opt;em.opt.I]
    e_RV (km/s)     (F5.2)  ? Radial velocity error (radial_velocity_error) [ucd=stat.error;spect.dopplerVeloc.opt;em.opt.I]
    Vbroad (km/s)   (F8.4)  ? Spectral line broadening parameter (vbroad) [ucd=spect.dopplerVeloc.opt;em.opt.I]
    GRVSmag (mag)   (F9.6)  ? Integrated Grvs magnitude (grvs_mag) [ucd=phot.mag;em.opt]
    QSO             (I1)    [0/1] Flag indicating the availability of additional information in the QSO candidates table (in_qso_candidates) [ucd=meta.code.status]
    Gal             (I1)    [0/1] Flag indicating the availability of additional information in the galaxy candidates table (in_galaxy_candidates) [ucd=meta.code.status]
    NSS             (I1)    [0/7] Flag indicating the availability of additional information in the various Non-Single Star tables (non_single_star) [ucd=meta.code.status]
    XPcont          (I1)    [0/1] Flag indicating the availability of mean BP/RP spectrum in continuous representation for this source (has_xp_continuous) [ucd=meta.code.status]
    XPsamp          (I1)    [0/1] Flag indicating the availability of mean BP/RP spectrum in sampled form for this source (has_xp_sampled) [ucd=meta.code.status]
    RVS             (I1)    [0/1] Flag indicating the availability of mean RVS spectrum for this source (has_rvs) [ucd=meta.code.status]
    EpochPh         (I1)    [0/1] Flag indicating the availability of epoch photometry for this source (has_epoch_photometry) [ucd=meta.code.status]
    EpochRV         (I1)    [0/1] Flag indicating the availability of epoch radial velocity for this source (has_epoch_rv) [ucd=meta.code.status]
    MCMCGSP         (I1)    [0/1] Flag indicating the availability of GSP-Phot MCMC samples for this source (has_mcmc_gspphot) [ucd=meta.code.status]
    MCMCMSC         (I1)    [0/1] Flag indicating the availability of MSC MCMC samples for this source (has_mcmc_msc) [ucd=meta.code.status]
    And             (I1)    [0/1] Flag indicating that the source is present in the Gaia Andromeda Photometric Survey (GAPS) (in_andromeda_survey) [ucd=meta.code.status]
    Teff (K)        (F7.1)  ? Effective temperature from GSP-Phot Aeneas best library using BP/RP spectra (teff_gspphot) [ucd=phys.temperature.effective]
    logg ([cm/s2])  (F7.4)  ? Surface gravity from GSP-Phot Aeneas best library using BP/RP spectra (logg_gspphot) [ucd=phys.gravity]
    [Fe/H] ([-])    (F7.4)  ? Iron abundance from GSP-Phot Aeneas best library using BP/RP spectra (mh_gspphot) [ucd=phys.abund.Z]
    Dist (pc)       (F10.4) ? Distance from GSP-Phot Aeneas best library using BP/RP spectra (distance_gspphot) [ucd=pos.distance;pos.eq]
    A0 (mag)        (F7.4)  ? Monochromatic extinction A_0 at 547.7nm from GSP-Phot Aeneas best library using BP/RP spectra (azero_gspphot) [ucd=phys.absorption;em.opt]
    HIP             (I6)    ? HIP cross-id number, van Leeuwen, Cat. I/311 (hip_original_ext_source_id) [ucd=meta.number;phys.atmol.number]
    PS1             (I18)   ? PS1 cross-id name, Chambers et al., Cat. II/349 (ps1_original_ext_source_id) [ucd=meta.id.cross]
    SDSS13          (I19)   ? SDSS name, Albareti et al., 2017ApJS..233...25A (sdss13_ext_source_id) [ucd=meta.id.cross]
    SKYM2           (I9)    ? SkyMapperDR2 cross-id name, Onken et al., 2019PASA...36...33O (skym2_original_ext_source_id) [ucd=meta.id.cross]
    TYC2            (a12)   Tycho-2 cross-id name, Hog et al., Cat. I/259 (tyc2_original_ext_source_id) [ucd=meta.id.cross]
    URAT1           (a15)   URAT1 name, Zacharias et al., Cat. I/329 (urat1_original_ext_source_id) [ucd=meta.id.cross]
    AllWISE         (a19)   ALLWISE cross-id name, Cutri et al., Cat. II/328 (allwise_original_ext_source_id) [ucd=meta.id.cross]
    APASS9          (I8)    ? APASS9 identification, Henden et al., Cat. II/336 (apass9_original_ext_source_id) [ucd=meta.id.cross]
    GSC23           (a10)   GSC2.3 cross-id name, Lasker et al., Cat. I/305 (gsc23_original_ext_source_id) [ucd=meta.id.cross]
    RAVE5           (a16)   RAVE DR5 cross-id name, Kunder et al., Cat. III/279 (rave5_original_ext_source_id) [ucd=meta.id.cross]
    2MASS           (a17)   2MASS cross-id name, Cutri et al., Cat. II/246 (twomass_original_ext_source_id) [ucd=meta.id.cross]
    RAVE6           (a21)   RAVE DR6 cross-id name, Steinmetz et al., Cat. III/283 (rave6_original_ext_source_id) [ucd=meta.id.cross]
    RAJ2000 (deg)   (F15.11) Barycentric right ascension (ICRS) at Ep=2000.0 (added by CDS) (ra2000) [ucd=pos.eq.ra]
    DEJ2000 (deg)   (F15.11) Barycentric declination (ICRS) at Ep=2000.0 (added by CDS) (dec2000) [ucd=pos.eq.dec]*/
//--------------------------------------------------------------------------------------------------------------------
/*
#Title: Gaia DR3 source catalog (1811709771 sources)
#---Details of Columns:
    RA_ICRS (deg)   (F15.11) Right ascension (ICRS) at Ep=2016.0 (ra) [ucd=pos.eq.ra;meta.main]
    DE_ICRS (deg)   (F15.11) Declination (ICRS) at Ep=2016.0 (dec) [ucd=pos.eq.dec;meta.main]
    Source          (I19)   Unique source identifier (unique within a particular Data Release) (source_id) [ucd=meta.id;meta.main]
    Gmag (mag)      (F9.6)  ? G-band mean magnitude (phot_g_mean_mag) [ucd=phot.mag;em.opt]
    pmRA (mas/yr)   (F9.3)  ? Proper motion in right ascension direction, pmRA*cosDE (pmra) [ucd=pos.pm;pos.eq.ra]
    e_pmRA (mas/yr) (F6.3)  ? Standard error of proper motion in right ascension direction (pmra_error) [ucd=stat.error;pos.pm;pos.eq.ra]
    pmDE (mas/yr)   (F9.3)  ? Proper motion in declination direction (pmdec) [ucd=pos.pm;pos.eq.dec]
    e_pmDE (mas/yr) (F6.3)  ? Standard error of proper motion in declination direction (pmdec_error) [ucd=stat.error;pos.pm;pos.eq.dec]
--------------- --------------- ------------------- --------- --------- ------ --------- ------
                                                                        e_p              e_p   
                                                    G         pmRA      mRA (m pmDE      mDE (m
RA_ICRS (deg)   DE_ICRS (deg)   Source              mag (mag) (mas/yr)  as/yr) (mas/yr)  as/yr)
--------------- --------------- ------------------- --------- --------- ------ --------- ------
*/
void vizier_parse_line( Catalog* pVizier, string & line )
{
	if ( line.size() == 0 )		return;
    if ( line.size() < 61 )		{ logf_thread( (char*)"[ Erreur ] Longueur ligne < 61 \"%s\"", line.c_str() ); return; }
	//printf( (char*)"%s\n", line.c_str() );
	
    double fRA	= stod( line.substr(0,15), 0 );
    double fDE	= stod( line.substr(16,15), 0 );
    string name = line.substr(32, 19);
    double fMag = stod( line.substr(52,9), 0 );
    
	double pmRA = 0.0;
	double eRA  = 0.0;
	double pmDE = 0.0;
	double eDE  = 0.0;

    if ( line.size() < 95 )		
    { 
    	logf_thread( (char*)"[ Warning ] Longueur ligne < 95  lg=%d - \"%s\"", line.size(), line.c_str() ); 
    }
    else
    {
		pmRA = stod( line.substr(62,9), 0 );
		eRA  = stod( line.substr(72,6), 0 );
		pmDE = stod( line.substr(79,9), 0 );
		eDE  = stod( line.substr(89,6), 0 );
	}
    //int    iHip = stoi( line.substr(59,6), 0, 10 );
 
    StarCatalog* p = new StarCatalog( fRA, fDE, fMag, pmRA, eRA, pmDE, eDE, name, nbVizier++ );
    pVizier->add(p);


    logf_thread( (char*)"main::vizier_parse_line() Etoile '%s'\t(%0.7lf,\t%0.7lf)\tmag=%0.4lf\tpmRA=%0.4lf, eRA=%0.4lf  pmDE=%0.4lf, eDE=%0.4lf ",
    					 (char*)name.c_str(), (double)fRA, (double)fDE, (double)fMag, 
    					 (double)pmRA, (double)eRA, (double)pmDE, (double)eDE );
    
    //logf_thread( (char*)"main::vizier_parse_line() Etoile '%s'\t(%0.7lf,\t%0.7lf)\tmag=%0.4lf\tpmRA=%0.4lf, eRA=%0.4lf  pmDE=%0.4lf, eDE=%0.4lf 	",
    //					 (char*)name.c_str(), (double)fRA, (double)fDE, (double)fMag );

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#define ICRS
// ------------------------------------
	#ifdef ICRS
		#define TYP_COORD "RA_ICRS,DE_ICRS"
	#else
		#define TYP_COORD "RAJ2000,DEJ2000"
	#endif
// ------------------------------------
#define MOUV_PROPRE
// ------------------------------------
	#ifdef MOUV_PROPRE
		#define MOUV ",pmRA,e_pmRA,pmDE,e_pmDE"
	#else
		#define MOUV ""
	#endif
// ------------------------------------
#define GMAG_MAX "17.58"
//#define GMAG_MAX "19.58"

//--------------------------------------------------------------------------------------------------------------------
// Lance le script python find_gaia_dr3
// ------------------------------------
//
// python3 find_gaia_dr3.py -m 6000 --add="RA_ICRS,DE_ICRS,Source,Gmag,pmRA,e_pmRA,pmDE,e_pmDE" --Gmag="<13" --sort -r 1742 M1 >client.txt; less client.txt 
//
//--------------------------------------------------------------------------------------------------------------------
//
// python3 find_gaia_dr3.py -m 6000 
//							--add="RA_ICRS,DE_ICRS,Source,Gmag,pmRA,e_pmRA,pmDE,e_pmDE" 
//							--Gmag="<13" 
//							--sort 
//							-r 1742
//							M1 
//--------------------------------------------------------------------------------------------------------------------
void vizier_capture_thread( Catalog* pVizier, string sCoord, PanelCapture* p_panel_capture )
{
    logf_thread( (char*)"main::vizier_capture_thread()" );
    log_tab(true);
    string find;
    VarManager& var = VarManager::getInstance();
    
    if ( sCoord == "" )    {
        find = "find_gaia_dr3.py -r 10200 -m 3000 --Gmag=\"<8\" m45";
    }
    else    {
    	string sGmagMax = to_string( var.getf("fGmagMax") );
       	find =  "find_gaia_dr3.py -m 6000 --add=\"" TYP_COORD ",Source,Gmag" MOUV "\" --Gmag=\"<" +sGmagMax+ "\"" + sCoord ;
    }
   
	bGmagChange = false;
	string     rep = "/home/rene/Documents/astronomie/logiciel/python/cds.cdsclient/cdsclient/";
	string     cmd = "python3 " + rep + find;
	int        iRead = 0;
	bool       bEntete = false;
	FILE *     pipe;
	char       buf1[BUFSIZ]; //BUFSIZ est une constante connue du système


    nbVizier = 0;

	logf_thread( (char*)"Lance la requete : %s", find.c_str() );
	logf_thread( (char*)"dans le repertoire  %s", rep.c_str() );
 
 	//return;
 
    if ((pipe = popen(cmd.c_str(), "r")) != NULL)    {
    
        while (fgets(buf1, sizeof(buf1), pipe) != NULL)        {
        	for( int i=0; i<sizeof(buf1); i++ )	if (buf1[i]=='\n')	buf1[i]=0;
        	
            string s = string( buf1 );
			
            if ( s.find( "---------------" ) == 0 )            {
                iRead++;
                bEntete = true;
                continue;
            }

            if ( s.find( "#END#" ) == 0 )            {
				//log_thread( (char*)"END" );
                bEntete = false;
                continue;
            }
			if ( iRead == 2   )			vizier_parse_line( pVizier, s );
        }
        pclose(pipe);
    }
    else    {
        fprintf(stderr, "Echec de popen\n");
        exit(1);
    }
    logf_thread( (char*)"%d etoiles trouvees", pVizier->size() );
    log_tab(false);
    logf_thread( (char*)"main::vizier_capture_thread()  FIN" );
    
    // evite la sauvegarde dans un fichier
    //return;

 	//____________________________________
 	// On previens la fenetre capture
 	if ( p_panel_capture != NULL )
 	{
 		p_panel_capture->findGaiaDR3_end();
 	}
}

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void vizier_load_stars( Catalog* pVizier, string s, double ra, double de )
{
    if ( pVizier->size() != 0 )		pVizier->efface();

    PanelCapture* p_panel_capture = NULL;
    thread( &vizier_capture_thread, pVizier, s, p_panel_capture ).detach();
    Camera_mgr::getInstance().setRefCatalog( ra, de );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void vizier_capture_load_stars( Catalog* pVizier, string s, PanelCapture* p_panel_capture )
{
    if ( pVizier->size() != 0 )		pVizier->efface();

    logf_thread( (char*)"vizier_capture_load_stars() %d etoile(s)", pVizier->size() );
    thread( &vizier_capture_thread, pVizier, s, p_panel_capture ).detach();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void vizier_load_stars( Catalog* pVizier, string s )
{
    if ( pVizier->size() != 0 )		pVizier->efface();

    PanelCapture* p_panel_capture = NULL;
    thread( &vizier_capture_thread, pVizier, s, p_panel_capture ).detach();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void vizier_load_stars( string s, double ra, double de )
{
    if ( vizier.size() != 0 )		vizier.efface();

	vizier_load_stars( &vizier, s, ra, de );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void vizier_load_stars( string s )
{
    if ( vizier.size() != 0 )		vizier.efface();

	vizier_load_stars( &vizier, s );
}
/*
//--------------------------------------------------------------------------------------------------------------------
//
// Force le lancement de la commande 'g' sur arduino 
// voir idleGL()
//
//--------------------------------------------------------------------------------------------------------------------
void get_info_arduino()
{
	fTimer10s = 9.0;
    logf_thread ( (char*)"main.c::get_info_arduino() fTimer = %0.2f", fTimer10s );
}
*/
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
string get_basename( string s)
{
    int pos = -1;
    string r = string(s);
    while( pos=r.find("/") != string::npos )
    {
        r  = r.substr(pos);
        pos = -1;
    }
    if ( pos == -1|| r.length() == 0 )  return s;
    return r;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
unsigned int get_color( vcf4 v )
{
    unsigned int r = v.x *255.0;
    unsigned int g = v.y *255.0;
    unsigned int b = v.z *255.0;
    unsigned int a = v.w *255.0;

    return r<<24 | g<<16 | b<<8 | a;
}
//-------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool fexists(const char *filename)
{
  ifstream ifile(filename);
  return (bool)ifile;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool isPleiade()
{
    if ( pPleiade == NULL )         return false;

    if  ( Camera_mgr::getInstance().getCurrent() == pPleiade )        return true;
    return false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CallbackChargeGuidage::callback( bool bb, int ii, char* str)
{
    VarManager& var = VarManager::getInstance();
    logf( (char*)"CallbackChargeGuidage::callback( %s, %d, \"%s\" )", bb?(char*)"true":(char*)"false", ii, (char*)str );
    
    if ( bb )     
    {
        logf( (char*)"Charge guidage %s", (char*)str );

        var.set( "FileResultat", string(str) );

        panelCourbe->charge_guidage( string(str) );
        /*
        if ( panelCourbe != NULL )  
        {
            panelCourbe->get_vOrigine().x = vOri.x;
            panelCourbe->get_vOrigine().y = vOri.y;
        }
        */
    }
    
    workDirSauveCourbe = FileBrowser::getInstance().getWorkingDir();
    var.set( "DirSauveCourbe", workDirSauveCourbe );
    FileBrowser::getInstance().setFiltre( "" );
    FileBrowser::getInstance().setFocus();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CallbackSauveGuidage::callback( bool bb, int ii, char* str)
{
    VarManager& var = VarManager::getInstance();
    logf( (char*)"CallbackSauveGuidage::callback( %s, %d, \"%s\" )", BOOL2STR(bb), ii, (char*)str );

    if ( bb )     
    {
        if ( ii == 1 )
        {
            string s = string(str);
            if ( s.find( ".guid" ) == string::npos )     s = s + ".guid";
            filenameSauve = string(s);
            var.set( "FileResultat", string(filenameSauve) );
            logf( (char*)"  Sauve guidage %s", (char*)s.c_str() );
            //charge_guidage( string(str) );
            workDirSauveCourbe = FileBrowser::getInstance().getWorkingDir();
            var.set( "DirSauveCourbe", workDirSauveCourbe );
            FileBrowser::getInstance().setFiltre( "" );
            
            bSauve = true;
            FileBrowser::getInstance().cache();    
	        panelCourbe->sauve_guidage();
        }
        else
        {
            logf( (char*)"  selection souris %s", (char*)str );
            workDirSauveCourbe = FileBrowser::getInstance().getWorkingDir();
            var.set( "DirSauveCourbe", workDirSauveCourbe );
        }
    }
    else
    {
        logf( (char*)"  ABANDON Sauve guidage %s", (char*)str );
        workDirSauveCourbe = FileBrowser::getInstance().getWorkingDir();
        var.set( "DirSauveCourbe", workDirSauveCourbe );
        bSauve = false;
    }
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CallbackFileBrowser::callback( bool bb, int ii, char* str)
{
    FileBrowser& fb = FileBrowser::getInstance();
    
    logf( (char*)"CallbackFileBrowser::callback( %s, %d, \"%s\" )", BOOL2STR(bb), ii, (char*)str );
    log_tab(true);
    if ( bb )     
    {
        string f = fb.getFilename();
        string d = fb.getWorkingDir();
        logf( (char*)"charge image %s%s", (char*)d.c_str(), (char*)f.c_str() );
        charge_image( d, f );
    }
    workDirFileBrowser = fb.getWorkingDir();
    VarManager& var = VarManager::getInstance();
    var.set( "DirFileBrowser", workDirFileBrowser );
    fb.setFocus();
    log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CallbackFits::callback( bool bb, int ii, char* str)
{
    FileBrowser& fb = FileBrowser::getInstance();
    
    logf( (char*)"CallbackFits::callback( %s, %d, \"%s\" )", bb?(char*)"true":(char*)"false", ii, (char*)str );
    if ( bb && ii == 1)     
    {
        string f = fb.getFilename();
        string d = fb.getWorkingDir();
        logf( (char*)"  charge fichier fits %s%s", (char*)d.c_str(), (char*)f.c_str() );
        //charge_fits( d+f );
        charge_image( d, f );
    }
    workDirFits = fb.getWorkingDir();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void callback_enregistre( bool b, int ii, char* str)
{
    logf( (char*)"main::callback_enregistre( %s, %d, \"%s\" )  line %d", BOOL2STR(b), ii, (char*)str, __LINE__ );

    if ( ii == 1 )
    {
        FileBrowser::getInstance().setNewline(false);
        FileBrowser::getInstance().cache();    
    }
    
    Camera* p = Camera_mgr::getInstance().getCurrent();
    if ( p )        p->setFilename( string(str) );

    FileBrowser::getInstance().cache();    
    /*
    vector<string> cc = split( string(str), "/" );
    string rep = "/";
    for( int i=0; i<cc.size()-1; i++ )    rep = rep + cc[i] + "/";
    
    logf( (char*)"  surveillande de : %s", (char*)rep.c_str() );
    Surveillance::getInstance().start(rep);
    */

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void callback_enregistre_cam(char* str)
{
    logf( (char*)"callback_enregistre_cam( \"%s\" )", (char*)str );
    FileBrowser::getInstance().cache();
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void callback_charge_cam(char* str)
{
    logf( (char*)"callback_charge_cam( \"%s\" )", (char*)str );
    FileBrowser::getInstance().cache();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
vector<string> split (string s, string delimiter)
{
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;

    while ((pos_end = s.find (delimiter, pos_start)) != string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        if ( token.size() != 0 )
            res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void captureOnTop(Capture* p)
{
    Captures::getInstance().active(p);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void updatePanelPause(bool b)
{
    if ( b != bPause)            
    {
        bPause = b; 
        var.set("bPause", bPause);
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void ferme_file_browser()
{
    FileBrowser::getInstance().cache();    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
string getCurrentDirectory()
{
    return currentDirectory;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void setCurrentDirectory( string mes )
{
    currentDirectory = mes;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void alertBox( string mes )
{
    bAlert = true;
    sAlert = mes;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void charge_image(string dirname, string basename)
{
    if (    basename.find( ".jpg")  != string::npos
         || basename.find( ".jpeg") != string::npos
         || basename.find( ".JPG")  != string::npos
         || basename.find( ".fits") != string::npos
         || basename.find( ".fit")  != string::npos
         || basename.find( ".tga")  != string::npos
         || basename.find( ".png")  != string::npos
    ){

        Captures::getInstance().charge_image(dirname, basename);
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void alertBoxQuit()
{
    int n = tAlert.size();
    if ( n <= 0 )           return;
    
    AlertBox*   p;
    p = tAlert[n-1];
    p->quit();
    delete p;
    tAlert.pop_back();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int getScreenDX()
{
    //int dx = glutGet(GLUT_SCREEN_WIDTH);
    //if (dx==3200)            dx = 1920;
    return widthScreen;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int getWidth()
{
    return width;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int getHeight()
{
    return height;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int getScreenDY()
{
    //int dy = glutGet(GLUT_SCREEN_HEIGHT);
    return heightScreen;
}

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
std::vector<string>& getExclude()
{
    return exclude;
}

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
double pond2mag( double ponderation )
{
    return  -(log( ponderation ) / log(2.0)) + 17.0;
}
/*
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
double hms2rad(struct hms& HMS)
{
    return (HMS.h + HMS.m/60.0 + HMS.s/3600.0) / 12.0*M_PI;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
double dms2rad(struct dms& DMS)
{
	double signe = 1.0;
	if ( DMS.d < 0.0 )		signe = -1.0;		
    return signe*(abs(DMS.d) + DMS.m/60.0 + DMS.s/3600.0) / 180.0*M_PI;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void rad2hms(double r, struct hms& HMS)
{
    double sign = 1.0;
    double hDouze = 0.0;
    if ( r<0.0 )
    {
        sign = -1.0;
        //r *=-1.0;
        hDouze = 24.0;
    }
    double h = hDouze + (r / M_PI * 12.0);
    int H = h;
    HMS.h = H;
    
    double m = (h-H) * 60.0;
    //if ( h < 0.0 )  m *= -1.0;
    int M = m;
    HMS.m = M;
    
    double s = (m-M) * 60.0;
    HMS.s = s;
    
    //HMS.h *= sign;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
char* deg2str_hms(double r, char* str, int len)
{
	struct hms 			HMS;
	
    if ( r<0.0 )	r = 360.0 + r;
    
    double h = (r / 360.0 * 24.0);
    int H = h;
    HMS.h = H;
    
    double m = (h-H) * 60.0;
    int M = m;
    HMS.m = M;
    
    double s = (m-M) * 60.0;
    HMS.s = s;
    
    int is = floor(s);
    int cs = floor( (s-(double)is)*100.0);

	snprintf( (char*)str, len, "%02.0lfh %02.0lfm %02d.%02ds", HMS.h, HMS.m, is, cs );

    return str;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void rad2dms(double v, struct dms& DMS)
{
    double sign = 1.0;

    if ( v<0.0 )        sign = -1.0;

    double d = abs(v) / M_PI * 180.0;
    int D = d;
    DMS.d = D;
    
    double m = (d-D) * 60.0;
    //if ( d < 0.0 )  m *= -1.0;
    int M = m;
    DMS.m = M;
    
    double s = (m-M) * 60.0;
    DMS.s = s;

    DMS.d *= sign;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
char* deg2str_dms(double d, char* str, int len)
{
 	struct dms 			DMS;
 	
    double sign = 1.0;

    if ( d<0.0 )
    {
        sign = -1.0;
        d *=-1.0;
    }

    int D = d;
    DMS.d = D;
    
    double m = (d-D) * 60.0;
    //if ( d < 0.0 )  m *= -1.0;
    int M = m;
    DMS.m = M;
    
    double s = (m-M) * 60.0;
    DMS.s = s;

    DMS.d *= sign;
    
    int is = (int)floor(s);
    int cs = (int)floor( (s-(double)is)*100.0);

	snprintf( (char*)str, len, "%02.0lf° %02.0lf' %02d.%02d\"", DMS.d, DMS.m, is, cs );
	return str;
}

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void deg2dms(double v, struct dms& DMS)
{
	rad2dms( DEG2RAD(v), DMS );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void deg2hms(double v, struct hms& HMS)
{
	rad2hms( DEG2RAD(v), HMS );
}
*/
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void displayGLnuit_cb(void)
{
    if ( var.getb("bNuit") )        glColor4f( 1.0, 0.0, 0.0, 1.0 );
    else                            glColor4f( 1.0, 1.0, 1.0, 1.0 );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int getOffset( int x, int y, int width )
{
    return 3*(x) + 3*(y)* width;
}
#include <malloc.h>
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void updatePanelResultat()
{
    Camera_mgr& mgr = Camera_mgr::getInstance();
	WindowsManager& wm= WindowsManager::getInstance();

    char sStr[255];

	if ( Camera_mgr::getInstance().getCurrent() == NULL )			return;
	
	
    vec2* pv = Camera_mgr::getInstance().getSuivi();
    if ( pv != NULL )
    {
    	if ( !panelResultat->getVisible() && bPanelResultat )		panelResultat->setVisible( true) ;
    	
        double dx = xSuivi - pv->x;
        double dy = ySuivi - pv->y;

        vec3 w = vec3( xSuivi, ySuivi, 0.0);
        vec3 v = w - vec3( pv->x, pv->y, 0.0);
        double l = v.length();

        if ( bCentrageSuivi )
            sprintf( sStr, "Centre\t(%0.2f, %0.2f)", xSuivi, ySuivi );
        else
            sprintf( sStr, "Reference\t(%0.2f, %0.2f)", xSuivi, ySuivi );
        pRef->changeText(sStr);
        
        sprintf( sStr, "Etoile\t(%0.2f, %0.2f)", pv->x, pv->y );
        pEtoile->changeText(sStr);

        if ( bAsservissement )
        {
            sprintf( sStr, "Asserv\t\t%0.1f/%0.1fpx", l, fLimitCorrection0 );
        }
        else{
            sprintf( sStr, "Ecart\t\t%0.1f/%0.1fpx", l, fLimitCorrection0 );
        }
        pEcart->changeText(sStr);
        
        double xx = pv->x;
        double yy = pv->y;
        
        mgr.tex2screen(xx,yy);

        xx += 40;
        yy += 35;
        
        if ( (xx + panelResultat->getDX()) > (wm.getWidth()+10) )       xx -= panelResultat->getDX();
        if ( (yy + panelResultat->getDY()) > (wm.getHeight()+10) )      yy -= panelResultat->getDY();
        panelResultat->setPos( xx, yy );
    }
    else
    {
    	if ( bPanelResultat )		panelResultat->setVisible(false) ;
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void displayGL(void)
{
#ifdef DEBUG_CALLBACK_OPENGL
    logf( (char*)"    %02d\t%.8f displayGL() clearBuffer()", ++appelIdle, -timerAppelIdle +Timer::getInstance().getGlutTime()  );
#endif
    //logf( (char*)"*** DISPLAY GL ***" );
	WindowsManager::getInstance().clearBufferGL(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#ifdef DEBUG_CALLBACK_OPENGL
    logf( (char*)"\t\t%.8f",  +Timer::getInstance().getGlutTime() -timerAppelIdle  );
#endif
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

    if ( var.getb("bNuit") )        glColor4f( 1.0, 0.0, 0.0, 0.2 );
    else                            glColor4f( 1.0, 1.0, 1.0, 1.0 );

	WindowsManager::getInstance().displayGL();

#ifdef DEBUG_CALLBACK_OPENGL
    logf( (char*)"    %02d\t%.8f glutSwapBuffers()", ++appelIdle, -timerAppelIdle +Timer::getInstance().getGlutTime()  );
#endif

	glutSwapBuffers();
#ifdef DEBUG_CALLBACK_OPENGL
    logf( (char*)"\t\t%.8f",  +Timer::getInstance().getGlutTime() -timerAppelIdle  );
#endif

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void reshapeGL(int newWidth, int newHeight)
{
    logf((char*) "main::reshapeGL(%d, %d) ---------------", newWidth, newHeight);
    log_tab(true);
    //bDesactiveLog = true;

	WindowsManager& wm = WindowsManager::getInstance();
	wm.setScreenSize( newWidth, newHeight );

    resizeHelp(newWidth, newHeight);
    resizeCourbe(newWidth, newHeight);

	panelStatus->setPosAndSize( 0, newHeight-20, newWidth, 20 );
	panelStatus->setCanMove	(false);
	pFPS->setPos( newWidth-100, 5);
	pHertz->setPos( newWidth-150, 5);

	glViewport(0, 0, (GLint)newWidth, (GLint)newHeight);

	if ( !bFull )
    {
	    width = newWidth;
	    height = newHeight;
    }
    else
    {
        width = getScreenDX();
        height = getScreenDY();
    }
    Camera_mgr::getInstance().resize( newWidth, newHeight );
    Captures::getInstance().reshapeGL( newWidth, newHeight );

    bDesactiveLog = false;
    log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void write_image(void)
{
    //camera.mainloop();
}
//--------------------------------------------------------------------------------------------------------------------
// peur-etre emis dans un temps thread
//--------------------------------------------------------------------------------------------------------------------
void son(string s)
{
	logf_thread( (char*)"Son : %s", s.c_str() );
	if ( !bSound )		return;
	
	string cmd = "aplay -q " + s;// + "  2>&1 >/dev/null";
	system( cmd.c_str() );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void change_hertz(double hz)
{
    static char sHz[255];
    sprintf( sHz, "%.0fHz", round(hz));
    pHertz->changeText(sHz);
    //camera.mainloop();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void arduino_online(bool b)
{
	if ( bArduino == b )			return;
	
    logf( (char*)"main::Change Arduino %s", (char*)BOOL2STR(b) );
    
    bArduino = b;
    
    if ( !b )	
    {
		changeAsc( false );
		changeDec( false );
		changeSui( false );
		changeRetourPos( false );
		changeJoy( false);
		
		pArduino->setColor(VCF4_2_COLOR32(cGris));
		PanelConsoleSerial::getInstance().setPrompt("No connect> ");
	}    
	else
	{
		pArduino->setColor(VCF4_2_COLOR32(cJaune));
		PanelConsoleSerial::getInstance().setPrompt("Arduino> ");
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void change_joy(int x, int y)
{
    //logf( (char*)"main::change_joy(%d,%d)", x, y );
    static char sJoyXY[255];
    
    vec2 v = vec2( x, y );
	snprintf( sJoyXY, sizeof(sJoyXY), "" VEC2_PRINTFN(0) " - ...", VEC2_AFF(v) );

    pCoordSuivi->changeText((char*)sJoyXY );

	if ( bNuit)
	{
		if ( bSuivi )           pSuivi->setColor(VCF4_2_COLOR32(cRouge));
		else                    pSuivi->setColor(VCF4_2_COLOR32(cRougeF));
	}
    else
    {
		if ( bSuivi )           pSuivi->setColor(VCF4_2_COLOR32(cJaune));
		else                    pSuivi->setColor(VCF4_2_COLOR32(cGris));
	}
	/*
	*/
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void change_joy(double x, double y)
{
	change_joy( (int)x, (int)y );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void change_ad_status(double ad)
{
    if ( pos_telescope.ad == ad && bCoordHoriz == bCoordHoriz_old )			return;

	Captures::getInstance().change_ad( ad );
    VarManager& var = VarManager::getInstance();

	if ( pos_telescope.ad != ad	)
	{
		var.set("pos_telescope.ad", ad);
		pos_telescope.ad = ad;
	}

    char    buff[255];
	buff[0] = '=';
	buff[1] = ' ';

 	if ( bCoordHoriz )
	{
		st_az			azim;
		st_eq			j2000 = {pos_telescope.ad, pos_telescope.dc};
		Serveur_mgr::getInstance().compute_azim( azim, j2000 );

		deg2str_dms( azim.azi, buff+2, sizeof(buff)-2 );

		pAlpha->setVisible(false);;
		pAzi->setVisible(true);
	}
	else
	{
		deg2str_hms( ad, buff+2, sizeof(buff)-2 );

		pAlpha->setVisible(true);;
		pAzi->setVisible(false);
	}

   pAD->changeText( buff );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void change_dc_status(double dc)
{
    if ( pos_telescope.dc == dc && bCoordHoriz == bCoordHoriz_old )					return;

	Captures::getInstance().change_dc( dc );
    VarManager& var = VarManager::getInstance();

	if ( pos_telescope.dc != dc	)
	{
		var.set("pos_telescope.dc", dc);
		pos_telescope.dc = dc;
	}
	    
    char    buff[255];
	buff[0] = '=';
	buff[1] = ' ';
	    
	if ( bCoordHoriz )
	{
		st_az			azim;
		st_eq			j2000 = {pos_telescope.ad, pos_telescope.dc};
		Serveur_mgr::getInstance().compute_azim( azim, j2000 );
		
		deg2str_dms( azim.hau, buff+2, sizeof(buff)-2 );
		
		pDelta->setVisible(false);
		pHau->setVisible(true);
	}
	else
	{
		deg2str_dms( dc, buff+2, sizeof(buff)-2 );

		pDelta->setVisible(true);
		pHau->setVisible(false);
	}
    
    //logf( (char*)"main::change_dc_status %s", buff );
    
    pDC->changeText( buff );
    bCoordHoriz_old= bCoordHoriz;

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void compute_matrix()
{
	Camera_mgr& 	cams = Camera_mgr::getInstance();
	Camera* 		pCam = cams.getCurrent();
	if ( pCam )
	{
		pCam->compute_matrix();
	}
	
    vec3 v0 = vecAD[1] - vecAD[0];
    vec3 v1 = vecDC[1] - vecDC[0];
    vec3 v2 = vec3( 0.0, 0.0, 1.0 );
    mat3 m = mat3( v0, v1, v2 );
    mat3 mi = m.inverse();
    mChange = mi;

    logf( (char*)"Compute matrice de changement de repere...");
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void sauve_traces(void)
{
    string filename = "/home/rene/.astropilot/traces.txt";
    logf( (char*)"Sauvegarde des traces dans '%s'", (char*)filename.c_str() );
    
    int nbt = t_vTrace.size();
    if ( nbt < 1 )          return;
    
    std::ofstream fichier;
    fichier.open(filename, std::ios_base::trunc);

    if ( !fichier ) 
    {
        logf( (char*)"[ERROR]impossble d'ouvrir : '%s'", (char*)filename.c_str() );
        return;
    }

    fichier << "Nombre de trace : " <<  nbt << "\n";

    for( int j=0; j<nbt; j++ )
    {
        fichier << "Trace No : " <<  j << "\n";
        vector<vec2>* courbe = t_vTrace[j];
        for(int i=0; i<courbe->size(); i++)
        {
            fichier << "( " << (*courbe)[i].x << " , " <<  (*courbe)[i].y << " )";
            fichier << "\n";
        }
    }
    fichier.close();
    
    //logf( (char*)"Lecture de %d donnees dans %d lignes", n, nbLigne );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void charge_traces(void)
{
    string filename = "/home/rene/.astropilot/traces.txt";
    logf( (char*)"Lecture des traces dans '%s'", (char*)filename.c_str() );
    
    std::ifstream fichier;
    fichier.open(filename, std::ios_base::app);
    if ( !fichier ) 
    {
        logf( (char*)"[ERROR]impossble d'ouvrir : '%s'", (char*)filename.c_str() );
        return;
    }

    //----------------- Suppression du tableau ----------------------------
    int nbt = t_vTrace.size();
    int i, j;
    
    for( int i=nbt-1; i>=0; i-- )
    {
        vector<vec2>* trace = t_vTrace[i];
        trace->clear();
        delete trace;
        t_vTrace.pop_back();
        trace = 0;
    }
    //------------- Lecture du fichier ------------------------------------
    string line;
    double x, y;
    vec2 v;
    int n = -1;
    vector<vec2>* trace;
    int nbl = 0;
    
    
    while ( getline (fichier, line) )
    {
        //n = -1;
        //logf( (char*)"%s", line.c_str() );
        if ( line.find("trace") != std::string::npos )                  continue;
        
        if ( line.find("Trace No : ") != std::string::npos )
        {
            sscanf( line.c_str(), "Trace No : %d", &n );
            logf( (char*)"Charge la trace No : %d", n );
            trace = new (vector<vec2>)();
            t_vTrace.push_back( trace );
            continue;
        }
        trace = t_vTrace[n];

        sscanf( line.c_str(), "( %lf , %lf )", &x, &y );
        //logf ( (char*)" v(%f,%f)", x, y );
        v.x = x;
        v.y = y;

        trace->push_back( vec2(x,y) );
        nbl++;
    }
    
    fichier.close();
    logf( (char*)"Lecture de %d lignes de %d traces", nbl, t_vTrace.size() );
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void charge_fits(string filename)
{
    logf( (char*)"Chargement des valeurs dans '%s'", (char*)filename.c_str() );

    std::ifstream fichier;
    fichier.open(filename, std::ios_base::app);

    if ( !fichier ) 
    {
        logf( (char*)"[ERROR]impossble d'ouvrir : '%s'", (char*)filename.c_str() );
        return;
    }

    fichier.seekg( 0, fichier.end );
    int lenght = fichier.tellg();
    fichier.seekg( 0, fichier.beg );

    #define LENGTH_HDU      2880
    char buffer[LENGTH_HDU];
    fichier.read(buffer, LENGTH_HDU);
    
    for( int i=0; i<(LENGTH_HDU/80); i++ )
    {
        string k = "";
        string v = "";
        for( int j=0; j<8; j++ )
        {
            k = k + buffer[i*80+j];
        }
        
        if ( i==0 )
        {
            if ( k.find("SIMPLE") == std::string::npos )
            {
                logf( (char*)"[ERREUR] Ce n'est pas unfichier FITS standard 'SIMPLE' non trouve" );
                return;
            }
        }

        for( int j=10; j<32; j++ )
        {
            v = v + buffer[i*80+j];
        }
        logf( (char*)"%s : %s", (char*)k.c_str(), (char*)v.c_str() );

        if ( k.find("END") == 0 )       break;
    }

    fichier.close();
    

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void suivi(void)
{
    if ( Camera_mgr::getInstance().haveNewFrame() )
    {
        Camera_mgr::getInstance().haveUseFrame(false);
    }
    else
        return;
    

    vec2*      	pV = Camera_mgr::getInstance().getSuivi();
    
    if ( bSuivi && pV != NULL )
    {
        panelCourbe-> idle_guidage( *pV );        
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void getSuiviParameter(void)
{
/*
    Camera_mgr&  cam_mgr = Camera_mgr::getInstance();
    //cam_mgr.active();

    panelCourbe->set_xCam( cam_mgr.get_xCam() );
    panelCourbe->set_yCam( cam_mgr.get_yCam() );
    panelCourbe->set_dxCam( cam_mgr.get_dxCam() );
    panelCourbe->set_dyCam( cam_mgr.get_dyCam() );

    vCameraSize.x = 1920.0;
    vCameraSize.y = 1080;
    vCameraSize = cam_mgr.get_vCameraSize();

    rw = (double)vCameraSize.x/(double)panelCourbe->get_dxCam();
    rh = (double)vCameraSize.y/(double)panelCourbe->get_dyCam();
*/
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void idleGL(void)
{
#ifdef DEBUG_CALLBACK_OPENGL
    logf( (char*)"    %02d\t%.8f idleGL", ++appelIdle, -timerAppelIdle +Timer::getInstance().getGlutTime() );
#endif
	setColor();
	log_thread_aff();
    
	Timer&          timer	= Timer::getInstance();
    Camera_mgr&		cams	= Camera_mgr::getInstance();


    if ( bFirstStart && bPause )
    {
        bOneFrame = true;
    }
    
    if ( bCoordHoriz )
    {
    	double ad = pos_telescope.ad;
    	double dc = pos_telescope.dc;
    	
    	bCoordHoriz_old = !bCoordHoriz;
    	change_ad_status(ad);
    	bCoordHoriz_old = !bCoordHoriz;
    	change_dc_status(dc);
    }

    #ifdef IDLEGL
    log_tab(false);
    logf( (char*)"[%0.4f] IDLE GL ***", timer.getCurrentTime() );
    #endif

	fTimeMili = (double)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    char sFPS[] = "fps 0000";
    sprintf( sFPS,"fps %d", *Timer::getInstance().getvFPSCounter() );
    pFPS->changeText((char*)sFPS);

#ifdef DEBUG_CALLBACK_OPENGL
	appelIdle = 0;
	timerAppelIdle = +Timer::getInstance().getGlutTime();
    log( (char*)"---------------------------------------------------------------" );
    logf( (char*)"    %02d\t%.8f idleGL:timer", ++appelIdle, +Timer::getInstance().getGlutTime() -timerAppelIdle  );
#endif
    timer.Idle();
#ifdef DEBUG_CALLBACK_OPENGL
    logf( (char*)"\t\t%.8f",  +Timer::getInstance().getGlutTime() -timerAppelIdle  );
#endif


    #ifdef IDLEGL
    logf( (char*)"[%0.4f] IDLE GL ***", timer.getCurrentTime() );
    log_tab(true);
    #endif


#ifdef DEBUG_CALLBACK_OPENGL
    logf( (char*)"    %02d\t%.8f idleStatus", ++appelIdle, +Timer::getInstance().getGlutTime() -timerAppelIdle  );
#endif
    idleStatus();
#ifdef DEBUG_CALLBACK_OPENGL
    logf( (char*)"\t\t%.8f",  +Timer::getInstance().getGlutTime() -timerAppelIdle  );
#endif

    //-----------------------------------------------------------------------
    // Gestion de alertbox
    //-----------------------------------------------------------------------
    updatePanelResultat();
    //-----------------------------------------------------------------------
    // Gestion de alertbox
    //-----------------------------------------------------------------------
    if ( bAlert )
    {
        bAlert = false;
        AlertBox* p = new AlertBox( sAlert );
        tAlert.push_back( p );
        p->onTop();
    }
    //-----------------------------------------------------------------------
    // Gestion de la sauvegarde des coordonnees
    // ecran des differentes fenetres
    //-----------------------------------------------------------------------
    if ( panelEnregistre && panelEnregistre->getHaveMove() )
    {
        panelEnregistre->resetHaveMove();

        var.set("PanelEnregistre_X",  panelEnregistre->getX() );
        var.set("PanelEnregistre_Y",  panelEnregistre->getY() );
        var.set("PanelEnregistre_DX", panelEnregistre->getDX() );
        var.set("PanelEnregistre_DY", panelEnregistre->getDY() );
        
    }

    if ( panelStdOut && panelStdOut->getHaveMove() )
    {
        panelStdOut->resetHaveMove();

        var.set("PanelStdOut_X",  panelStdOut->getX() );
        var.set("PanelStdOut_Y",  panelStdOut->getY() );
        var.set("PanelStdOut_DX", panelStdOut->getDX() );
        var.set("PanelStdOut_DY", panelStdOut->getDY() );
        
    }

    if ( panelHelp->getHaveMove() )
    {
        panelHelp->resetHaveMove();
	    panelScrHelp->setSize( panelHelp->getDX(), panelHelp->getDY() );

        var.set("PanelHelp_X",  panelHelp->getX() );
        var.set("PanelHelp_Y",  panelHelp->getY() );
        var.set("PanelHelp_DX", panelHelp->getDX() );
        var.set("PanelHelp_DY", panelHelp->getDY() );
        
    }

    if ( panelCourbe->getHaveMove() )
    {
        panelCourbe->resetHaveMove();
        panelCourbe->build_unites_text();

        var.set("PanelCourbe_X",  panelCourbe->getX() );
        var.set("PanelCourbe_Y",  panelCourbe->getY() );
        var.set("PanelCourbe_DX", panelCourbe->getDX() );
        var.set("PanelCourbe_DY", panelCourbe->getDY() );
        
    }
    
   	cams.testMove();

#ifdef DEBUG_CALLBACK_OPENGL
    logf( (char*)"    %02d\t%.8f Camera_mgr, Captures", ++appelIdle, +Timer::getInstance().getGlutTime() -timerAppelIdle  );
#endif
    //-----------------------------------------------------------------------
    // Mise a jour des buffers de la camera
    // et des pointeurs vers la texture background
    //-----------------------------------------------------------------------
    
    cams.update();
#ifdef DEBUG_CALLBACK_OPENGL
    logf( (char*)"\t\t%.8f",  +Timer::getInstance().getGlutTime() -timerAppelIdle  );
#endif

    //-----------------------------------------------------------------------
    // Idle serial
    //-----------------------------------------------------------------------
    double elapsedTime = -1;
	if ( prevTime < 0 )	{
		prevTime = fTimeMili;
	}
	else 	{
		elapsedTime = fTimeMili - prevTime;
		prevTime = fTimeMili;
	}
    
	if ( elapsedTime != -1 )			Serial::getInstance().idle(elapsedTime);

    //-----------------------------------------------------------------------
    // Lance le suivi des etoiles
    //-----------------------------------------------------------------------
    if ( bSuivi )   suivi();

    //-----------------------------------------------------------------------
    // Enregistrement des traces
    //-----------------------------------------------------------------------
    if ( bRecTrace )
    {
        int i = t_vTrace.size() - 1;
        if ( i >= 0 )
        {
            vector<vec2>* trace = t_vTrace[i];
            int nb = trace->size();
        
            vec2* vv = Camera_mgr::getInstance().getSuivi();
            if( vv != NULL )
            {
                logf( (char*)"trace[%d][%d] = (%0.2f,%0.2f)", i, nb, vv->x, vv->y );
                trace->push_back( vec2(vv->x, vv->y) );
            }
        }
    }
    //------------------------------------------------------
    //------------------------------------------------------
	// CHARGEMENT IMAGE SUR SURVEILLANCE
    //------------------------------------------------------
    //------------------------------------------------------
    if ( Surveillance::getInstance().idleGL() )
    {
        //Captures::getInstance().rotate_capture_plus(true);
    }

	cams.idleGL( timer.getElapsedTime() );

	/*
    //------------------------------------------------------
    //------------------------------------------------------
	// CALCUL DE L' ASSERVISSEMENT
    //------------------------------------------------------
    //------------------------------------------------------
	if ( elapsedTime != -1 )
	{
	    fTimeCpt += elapsedTime;
	    //logf( (char*)"Temps ecoule : %0.2f", fTimeCpt );
	    //if ( bAsservissement && fTimeCpt > 0.0 && !bModeManuel)
        vec2* pv = Camera_mgr::getInstance().getSuivi();
        
        if ( pv != NULL && bAsservissement )
        {
            vec3 res, v, w;  double l;

            w = vec3( xSuivi, ySuivi, 0.0);
            v = w - vec3( pv->x, pv->y, 0.0);
            l = v.length();

            if ( l > fLimitCorrection0 ) {
                logf( (char*)"[WARNING]Suivi l=%0.2f/%0.2f", l, fLimitCorrection0 ); 
				son( "/home/rene/.astropilot/sounds/alert.wav" );
                arret_urgence();
                thread( &sound_alert).detach();
            }

	        if ( fTimeCpt > 0.0 )
	        {
                fTimeCpt -= fTimeCorrection;
                if ( fTimeCpt > 0.0 )    fTimeCpt = 0.0;//-= fTimeCorrection;
	            
	            if ( fabs(v.x) > panelCourbe->get_err() || fabs(v.y) > panelCourbe->get_err() )
	            {
                    if ( !bMouseDeplace && Serial::getInstance().getFree() )
                    {
                        res = mChange * v;
                        int ad = (int) (res.x * -1000.0);
                        int dc = (int) (res.y * 1000.0);
                        char cmd[255];
                        sprintf( cmd, "a%dp;d%dp", ad, dc );
                        Serial::getInstance().write_string(cmd, false);
                        logf( (char*)"cmd='%s' delta(%0.2f,%0.2f) l=%0.2f",  
                                      cmd, v.x, v.y, l );

                    }
                    else
                    {
                        logf( (char*)"Arduino  pas pret" ); 
                    }
	            }
            }
	    }
	}
	*/
    //------------------------------------------------------
    //------------------------------------------------------
    PanelConsoleSerial::getInstance().idleGL();
#ifdef DEBUG_CALLBACK_OPENGL
    logf( (char*)"    %02d\t%.8f WindowsManager::idleGL()", ++appelIdle, -timerAppelIdle +Timer::getInstance().getGlutTime());
#endif
    WindowsManager::getInstance().idleGL( elapsedTime );
#ifdef DEBUG_CALLBACK_OPENGL
    logf( (char*)"\t\t%.8f",  +Timer::getInstance().getGlutTime() -timerAppelIdle  );
#endif

    
    onTop();    

    //Camera_mgr::getInstance().idleGL();
#ifdef DEBUG_CALLBACK_OPENGL
    logf( (char*)"    %02d\t%.8f glutPostRedisplay", ++appelIdle, -timerAppelIdle +Timer::getInstance().getGlutTime() );
#endif
	glutPostRedisplay();
#ifdef DEBUG_CALLBACK_OPENGL
    logf( (char*)"\t\t%.8f",  +Timer::getInstance().getGlutTime() -timerAppelIdle  );
#endif

    if ( bOneFrame )
    {
        bOneFrame = false;
    }
    bFirstStart = false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void print_all_panels(void)
{
    //cout <<"exit_handler()"<< endl;
	//bStdOut = false;
    logf( (char*)"main::print_all_panels()" );

    WindowsManager& 		wm 		= WindowsManager::getInstance();
    int 					nbPanel	= wm.getNbPanel();
	std::vector<Panel*>& 	childs 	= wm.getChilds();
	
    logf( (char*)"Affiche toutes les fenetres" );
	log_tab(true);
    logf( (char*)"Nb Fenetre %d", nbPanel );
    for( int i=nbPanel-1; i>=0; i-- )
    {
    	Panel *	p = childs[i];
	    //logf( (char*)"Panel ID %d-%s, %d enfants %s", p->getID(), p->getExtraString().c_str(), p->getNbPanel(), typeid(*p).name() );
	    logf( (char*)"Panel ID %03d-%s, %d enfants", p->getID(), p->getExtraString().c_str(), p->getNbPanel() );
		//delete_all_panels( p );
		//childs.erase( childs.begin()+i );
    }
	log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void delete_all_panels(Panel* p)
{
	std::vector<Panel*>& 	childs 	= p->getChilds();
	int						nbPanel	= p->getNbPanel();
	int						ID		= p->getID();

    //logf( (char*)"Panel ID %d-%s, %d enfants", ID, p->getExtraString().c_str(), nbPanel );
    if( nbPanel <= 0)		return;
    
	log_tab(true);
    for( int i=nbPanel-1; i>=0; i-- )
    {
		Panel * child = childs[i];
		delete_all_panels( child );
		childs.erase( childs.begin()+i );
    }

	log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void quit(void)
{
    //cout <<"exit_handler()"<< endl;
	bStdOut = false;
    logf( (char*)"exit_handler()" );

    LX200::getInstance().close_all();
	SYNSCAN::getInstance().close_all();
	Serveur_mgr::getInstance().close_all();
	Camera_mgr::getInstance().stopAllCameras();
	Connexion_mgr::getInstance().stopThreadDEV();
	Connexion_mgr::getInstance().stopThreadZWO();

    WindowsManager& 		wm 		= WindowsManager::getInstance();
    int 					nbPanel	= wm.getNbPanel();
	std::vector<Panel*>& 	childs 	= wm.getChilds();
	
    logf( (char*)"Fermeture de toute les fenetres" );
	log_tab(true);
    logf( (char*)"Nb Fenetre %d", nbPanel );
    for( int i=nbPanel-1; i>=0; i-- )
    {
    	Panel *	p = childs[i];
	    //logf( (char*)"Panel ID %d-%s, %d enfants %s", p->getID(), p->getExtraString().c_str(), p->getNbPanel(), typeid(*p).name() );
	    logf( (char*)"Panel ID %03d-%s, %d enfants", p->getID(), p->getExtraString().c_str(), p->getNbPanel() );
		delete_all_panels( p );
		childs.erase( childs.begin()+i );
    }
	log_tab(false);
	
	glutLeaveMainLoop();
	exit(0);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void initGL(int argc, char **argv)
{
	glClearColor(.8f,.8f,.8f,1.0f);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	glDisable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void rotateVisible()
{
    Panel*  pCapture = WindowsManager::getInstance().getCapture();

    if ( pCapture == panelHelp)       pCapture->setVisible(!pCapture->getVisible());
    //if ( pCapture == panelControl)    pCapture->setVisible(!pCapture->getVisible());
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void glutMouseFunc(int button, int state, int x, int y)	{
#ifdef DEBUG_CALLBACK_OPENGL
    logf( (char*)"    %02d\t%.8f glutMouseFunc", ++appelIdle, -timerAppelIdle +Timer::getInstance().getGlutTime() );
#endif
   	iGlutModifier = glutGetModifiers();
   	//if ( iGlutModifier )	logf( (char*)"glutGetModifiers = %d", iGlutModifier );
   	
    vMouse.x = x;
    vMouse.y = y;

    WindowsManager& wm  	= WindowsManager::getInstance();
    Camera_mgr&     cams	= Camera_mgr::getInstance(); 

	wm.mouseFunc(button, state, x, y);

    PanelWindow*    pPreviewCam = NULL;
    Panel *         pCapture = NULL;
    Panel*          pMouseOver = NULL;

    if ( panelStatus->isMouseOver(x, y) )
    {
        //logf( (char*)"Souris sur la barre de status" );
        //goto endglutMouseFunc;
        return;
    }
    //-------------------------------------
    // Deplacement telescope a la souris
    // bouton du mileu
    // ( mode monuel )
	Camera* 	pCam = cams.getCurrent();
    if ( bModeManuel && pCam != NULL  &&  pCam->getPanelCamera() != NULL )
    {
	    logf( (char*)"main::glutMouseFunc( %d, %d, %d )", button, x, y );
        pPreviewCam = pCam->getPanelCamera();
        pCapture    = wm.getCapture();
        //pMouseOver  = wm.findPanelMouseOver(x, y);
    
    
		if( (iGlutModifier & GLUT_ACTIVE_CTRL ) == 0 )
		{
			log_tab(true);
			if ( bMouseDeplace && button == GLUT_MIDDLE_BUTTON && state == 0 )
			{
				bMouseDeplaceVers = true;
				getSuiviParameter();

				cams.onBottom();
				
				int X = x;
				int Y = y;
				
				cams.screen2tex(X,Y);
				
				vDepl[0].x = xClick;
				vDepl[0].y = yClick;
				vDepl[0].z = 0.0;
				logf( (char*)"vDepl[0](%0.2f, %0.2f)", vDepl[0].x, vDepl[0].y );

				logf( (char*)"state = 0" );
			}
			if ( bMouseDeplace && button == GLUT_MIDDLE_BUTTON && state == 1 )
			{
				bMouseDeplaceVers = false;
				getSuiviParameter();

				cams.onBottom();
				
				int X = x;
				int Y = y;
				
				cams.screen2tex(X,Y);
				
				xClick = X;
				yClick = Y;

				vDepl[1].x = xClick;
				vDepl[1].y = yClick;
				vDepl[1].z = 0.0;
				
				logf( (char*)"Button middle state = 1" );
				logf( (char*)"vDepl[0] " VEC3_PRINTFN(0), VEC3_AFF(vDepl[0]) );
				logf( (char*)"vDepl[1] " VEC3_PRINTFN(0), VEC3_AFF(vDepl[1]) );


				vec3 v = vDepl[1] - vDepl[0];
				/*
				logf( (char*)"  delta (%0.2f, %0.2f)", v.x, v.y );
				
				vTr = mChange * v;
				int ad = (int) (vTr.x * -1000.0);
				int dc = (int) (vTr.y * 1000.0);
				char cmd[255];
				sprintf( cmd, "a%dp;d%dp", ad, dc );
				logf( (char*)"Envoi de la commande : \"%s\"",  cmd );
				
				v = vDepl[1] - vDepl[0];
				*/
				pCam->commande( v );
				char cmd[255];
				sprintf( cmd, "a%dp;d%dp", (int)v.x, (int)v.y );
				logf( (char*)"Envoi de la commande : \"%s\"",  cmd );
		        logf( (char*)"matrix = " MAT3_PRINTF , MAT3_AFF(pCam->get_matrix()) );

				Serial::getInstance().write_string(cmd);
			}

			//if ( bPause && button == 0 && state == 0 )	{
			if ( bModeManuel && button == GLUT_LEFT_BUTTON && state == 0 )	{
				getSuiviParameter();

				cams.onBottom();
				
				int X = x;
				int Y = y;
				
				cams.screen2tex(X,Y);
				
				xClick = X;
				yClick = Y;
				
				if ( iGlutModifier & GLUT_ACTIVE_ALT )
				{
				    xSuivi = xClick;
				    ySuivi = yClick;

				    var.set("xSuivi", xSuivi );
				    var.set("ySuivi", ySuivi );

				    panelCourbe->get_vOrigine().x = x;
				    panelCourbe->get_vOrigine().y = y;
				    panelCourbe->get_vOrigine().z = 0.0;
				    
				    change_joy( x, y );

				    logf( (char*)"initialise vOrigine(click) : (%d,%d)", x, y);
				}

				logf( (char*)"main::glutMouseFunc LeftClick (%d, %d)  vCamera (%dx%d)" , xClick, yClick, cams.getRB()->w.load(), cams.getRB()->h.load() );
				
			} 
			else
			if ( !bModeManuel && button == 0 && state == 0 && pMouseOver == pPreviewCam )	{

				logf( (char*)"Suivi=(%0.2f,%0.2f)" , xSuivi, ySuivi );

			} 

			log_tab(false);
		}//   if( !(iGlutModifier & GLUT_ACTIVE_CTRL ) )

	}//     if ( cams.getCurrent() != NULL  &&  cams.getCurrent()->getPanelCamera() != NULL )


    Camera_mgr::getInstance().onBottom();
    onTop();

endglutMouseFunc:
    //log_tab(false);
    //logf( (char*)"main::glutMouseFunc()");
    return;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void glutMotionFunc(int x, int y)	{	
#ifdef DEBUG_CALLBACK_OPENGL
    logf( (char*)"    %02d\t%.8f glutMotionFunc(%d, %d)", ++appelIdle, -timerAppelIdle +Timer::getInstance().getGlutTime(), x, y );
#endif

    vMouse.x = x;
    vMouse.y = y;
	
   	//iGlutModifier = glutGetModifiers();
    
	WindowsManager& wm = WindowsManager::getInstance();

	wm.setModifier(iGlutModifier);
	wm.motionFunc(x, y);

	onTop();

	//if ( bMouseDeplace  )        logf( (char*)" motion func (%d, %d)", x, y );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void glutPassiveMotionFunc(int x, int y)	{
#ifdef DEBUG_CALLBACK_OPENGL
    logf( (char*)"    %02d\t%.8f glutPassiveMotionFunc(%d, %d)", ++appelIdle, -timerAppelIdle +Timer::getInstance().getGlutTime(), x, y );
#endif

    vMouse.x = x;
    vMouse.y = y;
    
	WindowsManager& wm = WindowsManager::getInstance();

    wm.setModifier(iGlutModifier);
    wm.passiveMotionFunc(x, y);

    onTop();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void status_nuit( unsigned long );
void setColor()	
{
static bool bNuitOld =  -2;
	if (bNuitOld == bNuit && !bFirstStart)		return;

    logf( (char*)"main::setColor()  " );
	
	bNuitOld = bNuit;

    uint32_t color;
    if (bNuit)                  color = 0xFF0000FF;
    else                        color = COLOR_WHITE;

    PanelConsoleSerial::getInstance().getConsole()->setColor(color);

    if ( panelStdOut )				panelStdOut->setColor(color);
    panelHelp->setColor(color);
    panelScrHelp->setColor(color);
    panelCourbe->setColor(color);
    panelStatus->setColor(color);
    panelResultat->setColor(color);
    Captures::getInstance().setColor(color);
    Camera_mgr::getInstance().setColor(color);
    FileBrowser::getInstance().setColor(color);
    PanelConsoleSerial::getInstance().setColor(color);
    status_nuit(color);
#ifdef PANEL_LX200_DEBUG
    LX200::getInstance().setColor(color);
#endif
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void onTop()	
{
    WindowsManager& wm = WindowsManager::getInstance();
    Captures::getInstance().onTop();
#ifdef PANEL_LX200_DEBUG
    LX200& lx200 = LX200::getInstance();
    if ( lx200.is_connect() && bPanelStdOut )		wm.onTop(lx200.getPanel());
#endif
    
    if ( panelStdOut )								wm.onTop(panelStdOut);

    wm.onTop(panelStatus);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void resizeHelp(int width, int height)	{
    int dx = panelHelp->getDX();
    int dy = panelHelp->getDY();
    
    if ( dx==0)     return;

    int x = width - dx - 20;
    int y = 20 + 20 ;

    if ( var.existe("PanelHelp_X") )         x  = var.geti( "PanelHelp_X");
    if ( var.existe("PanelHelp_Y") )         y  = var.geti( "PanelHelp_Y");

    panelHelp->setPos( x,  y );
	panelScrHelp->setSize( width, height );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void resizeCourbe(int width, int height)	{
    int dx = width - 20;
    int dy = 600;
    int x = 10;
    int y = height - 10 - 20 - dy;

    if ( var.existe("PanelCourbe_X") )         x  = var.geti( "PanelCourbe_X");
    if ( var.existe("PanelCourbe_Y") )         y  = var.geti( "PanelCourbe_Y");
    if ( var.existe("PanelCourbe_DX") )        dx = var.geti("PanelCourbe_DX");
    if ( var.existe("PanelCourbe_DY") )        dy = var.geti("PanelCourbe_DY");


    panelCourbe->setPosAndSize( x,  y, dx, dy );
    panelCourbe->build_unites_text();
    //printf("resizeControl(%d, %d\n", width, height);
    //printf("  x=%d y=%d dx=%d dy=%d\n", x, y, dx, dy);
}
/*
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void update_err()
{
    char s[20];
    sprintf( s, "+%0.2f", panelCourbe->get_err() );
    panelCourbe->get_pXMax->changeText( (char*)s );
    panelCourbe->get_pYMax->changeText( (char*)s );

    sprintf( s, "-%0.2f", panelCourbe->get_err()  );
    pXMin->changeText( (char*)s );
    pYMin->changeText( (char*)s );
}
*/
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CreateCourbe()	{
	if ( panelCourbe != NULL )		return;

    panelCourbe = new PanelCourbe();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CreateResultat()	{
	if ( panelResultat != NULL )		return;
	
	WindowsManager& wm = WindowsManager::getInstance();

    panelResultat = new PanelWindow();
    panelResultat->setExtraString("panelResultat");
    
    panelResultat->setDisplayGL(displayGLnuit_cb);
    int dx = 700;
    int dy = 30;
    int x = 10;
    int y = height - 100;

    panelResultat->setPosAndSize( x, y, dx, dy);
    wm.add(panelResultat);
    
	int y0 = 5;
	int x0 = 10;
	int dx0 = 50;
	int l = 0;

	pRef	= new PanelText( (char*)"Reference ",  PanelText::NORMAL_FONT, x0, y0+16*l++  );
	pEtoile	= new PanelText( (char*)"Etoile ",      PanelText::NORMAL_FONT, x0, y0+16*l++  );
	pEcart	= new PanelText( (char*)"Ecart ",      PanelText::NORMAL_FONT, x0, y0+16*l++  );
	
	pRef->razTabSize();
	pRef->setTabSize(80);
	pEtoile->razTabSize();
	pEtoile->setTabSize(80);
	
	panelResultat->add( pRef ); 
	panelResultat->add( pEtoile ); 
	panelResultat->add( pEcart ); 

    panelResultat->setSize( x0 +200, 16*l+8);
    panelResultat->setBackground((char*)"images/background.tga");
    panelResultat->setVisible(bPanelResultat);
}
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
int x_help = 0;
int y_help = 0;
#define TAB_SIZE_HELP 60
void addString1( string s )
{
    if ( s.size() == 0 )       { y_help += 15; return; }
    //PanelText* p = new PanelText( (char*)s.c_str(),  		PanelText::NORMAL_FONT, x_help, y_help );
    PanelText* p = new PanelText( (char*)"",  		PanelText::NORMAL_FONT, x_help, y_help );

    p->razTabSize();
    p->setTabSize(TAB_SIZE_HELP);
    p->changeText( (char*)s.c_str() );
	panelScrHelp->add( p );

	y_help += 15;
}	
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void addString2( string s )
{
    if ( s.size() == 0 )       { y_help += 15; return; }
    PanelText* p = new PanelText( (char*)"",  		PanelText::NORMAL_FONT, x_help+400, y_help );

    p->razTabSize();
    p->setTabSize(TAB_SIZE_HELP);
    p->setTabSize(140);
    p->changeText( (char*)s.c_str() );
	panelScrHelp->add( p );

	y_help += 15;
}	
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void addString3( string s )
{
    if ( s.size() == 0 )       { y_help += 15; return; }
    PanelText* p = new PanelText( (char*)"",  		PanelText::NORMAL_FONT, x_help+400, y_help );

    p->razTabSize();
    p->setTabSize(TAB_SIZE_HELP+10);
    p->changeText( (char*)s.c_str() );
	panelScrHelp->add( p );

	y_help += 15;
}	
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void addString4( string s )
{
    if ( s.size() == 0 )       { y_help += 15; return; }
    PanelText* p = new PanelText( (char*)"",  		PanelText::NORMAL_FONT, x_help+800, y_help );

    p->razTabSize();
    p->setTabSize(TAB_SIZE_HELP);
    p->changeText( (char*)s.c_str() );
	panelScrHelp->add( p );

	y_help += 15;
}	
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CreateHelp()	
{
	if ( panelHelp != NULL )			return;
	
    int X = width - 50;
    int Y = 50;
    int DX = 400;
    int DY = 700;
    
    panelHelp = new PanelWindow();
    panelScrHelp = new PanelScrollY();
    panelHelp->add( panelScrHelp );
    
	panelHelp->setExtraString( (char*)"PanelHelp");
    panelHelp ->setDisplayGL(displayGLnuit_cb);
	panelHelp->setPosAndSize( X, Y, DX, DY);
	panelScrHelp->setPosAndSize( 0, 0, DX, DY);
	panelHelp->setVisible(bPanelHelp);
	panelHelp->setBackground( (char*)"images/background.tga");
    
	int y = 10;
	int dy = 15;
	int l=50;

    //----------------------------------------------------------------------------------------------------------------
	for( int i=0; i<t_sHelp1.size(); i++ )
	{
    	addString1( t_sHelp1[i] );
	}
    //----------------------------------------------------------------------------------------------------------------
    y_help = 0;
	for( int i=0; i<t_sHelp2.size(); i++ )
	{
    	addString2( t_sHelp2[i] );
	}
	for( int i=0; i<t_sHelp3.size(); i++ )
	{
    	addString3( t_sHelp3[i] );
	}
    //----------------------------------------------------------------------------------------------------------------
    y_help = 0;
	for( int i=0; i<t_sHelp4.size(); i++ )
	{
    	addString4( t_sHelp4[i] );
	}
    //----------------------------------------------------------------------------------------------------------------

    
	WindowsManager::getInstance().add(panelHelp);
	
	DY = ++l*dy;
	
	/*
    if ( var.existe("PanelHelp_X") )         X  = var.geti( "PanelHelp_X");
    if ( var.existe("PanelHelp_Y") )         Y  = var.geti( "PanelHelp_Y");
    if ( var.existe("PanelHelp_DX") )        DX = var.geti("PanelHelp_DX");
    if ( var.existe("PanelHelp_DY") )        DY = var.geti("PanelHelp_DY");
    */
    
    INIT_VARI( PanelHelp_X, 10);
    INIT_VARI( PanelHelp_Y, 10);
    INIT_VARI( PanelHelp_DX, 1400);
    INIT_VARI( PanelHelp_DY, 600);

    X  = var.geti( "PanelHelp_X" );
    Y  = var.geti( "PanelHelp_Y" );
    DX = var.geti( "PanelHelp_DX" );
    DY = var.geti( "PanelHelp_DY" );

    panelHelp->setSize(DX, DY);
	panelScrHelp->setSize(DX, DY);

    resizeHelp(width, height);
    panelHelp->setScissor( true );
    

    //X = width - 20 - DX;
    //Y = 20 + 20 ;
	panelHelp->setPos(X, Y);


    logf((char*)"** CreateHelp()  panelHelp  %d,%d %dx%d", X, Y, DX, DY);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#include "status.inc"

void CreateStatus()	{
	logf((char*)"main::CreateStatus()  width : %d", width);
	log_tab(true);

	WindowsManager& wm = WindowsManager::getInstance();
	wm.setScreenSize( width, height );

	int x=0, _x;
	int dx=width;
	int dy=20;
	int y=height-dy;
	char s[255];

	panelStatus = new PanelSimple();
	panelStatus->setExtraString( (char*)"PanelStatus");
	panelStatus->setPosAndSize( x, y, dx, dy );


	//-------------------------------------------------------------------------------------------

    pFPS = new PanelText( (char*)"0",		            PanelText::NORMAL_FONT, width-100, 2 );
	panelStatus->add( pFPS );

    pHertz = new PanelText( (char*)"0Hz",		            PanelText::NORMAL_FONT, width-150, 2 );
	panelStatus->add( pHertz );
	//change_hertz( 0.0 );

    pArduino = new PanelText( (char*)"Arduino",		    PanelText::NORMAL_FONT, width-280, 2 );
    pArduino->setColor(COLOR_GREY);
	panelStatus->add( pArduino );

    pStellarium = new PanelText( (char*)"Stellarium",   PanelText::NORMAL_FONT, width-230, 2 );
    pStellarium->setColor(COLOR_GREY);
	panelStatus->add( pStellarium );

    pPas = new PanelText( (char*)"pas:",		        PanelText::NORMAL_FONT, width-370, 2 );
    pPas->setColor(COLOR_GREY);
	panelStatus->add( pPas );

    pDeplacement = new PanelText( (char*)"Depl",		PanelText::NORMAL_FONT, width-405, 2 );
    pDeplacement->setColor(COLOR_GREY);
	panelStatus->add( pDeplacement );

    pRacket = new PanelText( (char*)"Racket",		PanelText::NORMAL_FONT, width-465, 2 );
    pRacket->setColor(COLOR_GREY);
	panelStatus->add( pRacket );

	//-------------------------------------------------------------------------------------------
    double d;
	#define POLICE "fonts/greek-wsi-regular.ttf"   	// 360 glyphe voir alpha ...

	pAlpha	= new PanelText( 	(char*)"a", (char*)POLICE , 50, 0, 14, 0xFFFFFFFF );
	pDelta	= new PanelText( 	(char*)"d", (char*)POLICE , 200, 0, 14, 0xFFFFFFFF );
	pAzi	= new PanelText( 	(char*)"A", PanelText::LARGE_FONT, 200+10, 2 );
	pHau	= new PanelText( 	(char*)"H", PanelText::LARGE_FONT, 200+10, 2 );
	
	
    pAlpha->setPos( 60-2,  2);
    pAzi->setPos( 60-2,  2);
    pDelta->setPos( 200-2, 1);
    pHau->setPos( 200-2, 1);

    
	panelStatus->add( pAlpha );
	panelStatus->add( pDelta );
	panelStatus->add( pAzi );
	panelStatus->add( pHau );

    pAD = new PanelText( (char*)"",		            PanelText::NORMAL_FONT, 60+10, 2 );

   	bCoordHoriz_old = !bCoordHoriz;
    change_ad_status( pos_telescope.ad );
	panelStatus->add( pAD );
    
    pDC = new PanelText( (char*)"",		            PanelText::NORMAL_FONT, 200+10, 2 );

   	bCoordHoriz_old = !bCoordHoriz;
    change_dc_status( pos_telescope.dc );
	panelStatus->add( pDC );

	string sErr = "Status !!!";

	//-------------------------------------------------------------------------------------------
 	wm.add( panelStatus );
 	panelStatus->setBackground((char*)"images/background.tga");
	_x = 900+50;

    pAsservi = new PanelText( (char*)"GUID",		    PanelText::NORMAL_FONT, _x, 2 );
	panelStatus->add( pAsservi );

	_x += 35;
    pSuivi = new PanelText( (char*)"Suivi",		        PanelText::NORMAL_FONT, _x, 2 );
	panelStatus->add( pSuivi );

	_x += 40;
    pCoordSuivi = new PanelText( (char*)"(---, ---) - ...",   PanelText::NORMAL_FONT, _x, 2 );
	panelStatus->add( pCoordSuivi );

	_x += 140;
    pFramesCapture = new PanelText( (char*)"--/--",   PanelText::NORMAL_FONT, _x, 2 );
	panelStatus->add( pFramesCapture );

 	
 	create_windows_button();
 	set_asservissement();

    //logf((char*)"** CreateStatus()  panelSatuts  %d,%d %dx%d", x, y, dx, dy);
	log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CreatePleiades()
{
    if ( ! test_dir( (char*)"/home/rene/.astropilot/pleiades" ) )		
    {
		logf_thread( (char*)"[ ERREUR ] Repertoire \"/home/rene/.astropilot/pleiades\" " );
    	bPleiade = false;
    	return;
    }
    if ( bPleiade )
    {
        pPleiade = new Pleiade();
        Camera_mgr::getInstance().add( pPleiade );
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CreateStdOut()	{
    if ( panelStdOut != NULL )		return;
    
   	panelStdOut = new PanelStdOut();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CreateEnregistre()	{
    if ( panelEnregistre != NULL )		return;
    
   	panelEnregistre = new PanelEnregistre();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CreateAllWindows()	{
    bool Old = bDesactiveLog;
    
    bDesactiveLog = false;
    panelEnregistre = NULL;
    
	CreateStdOut();
    CreateHelp();
    CreateResultat();
    CreateCourbe();
    CreateStatus();
    CreatePleiades();
	CreateEnregistre();

	// Initialise certaine variables    
    panelCourbe->get_vOrigine().x = xSuivi;
    panelCourbe->get_vOrigine().y = ySuivi;
    panelCourbe->get_vOrigine().z = 0.0;
    change_joy( xSuivi, ySuivi );
    change_perr( &dErr );

    bDesactiveLog = Old;
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void changeDec( bool b )
{
    pButtonDec->setVal( b );
    if ( b != bDec )
    {
        logf( (char*)"Declinaison : %d", (int)b );
        bDec = b;
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void changeAsc( bool b )
{
    pButtonAsc->setVal( b );
    if ( b != bAsc )
    {
        logf( (char*)"Asc Droite : %d", (int)b );
        bAsc = b;
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void changeSui( bool b )
{
    pButtonSui->setVal( b );
    if ( b != bSui )
    {
        logf( (char*)"Suivi : %d", (int)b );
        bSui = b;
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void changeJoy( bool b )
{
    pButtonJoy->setVal( b );
    if ( b != bJoy )
    {
        bJoy = b;
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool getJoy()
{
    return bJoy;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void changeRetourPos( bool b )
{
    pButtonRet->setVal( b );
    if ( b != bRet )
    {
        logf( (char*)"RetourPos : %d", (int)b );
        //inverse_texture( pButtonAsc, b, "asc" );
        bRet = b;
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void log_tab( bool b)
{
	if ( bDesactiveLog )		return;

    if ( b )            nb_tab++;
    else                nb_tab--;
    if ( nb_tab<0 )     nb_tab = 0;
    
    sTab = "";
    for( int i=0; i<nb_tab; i++ )
    {
        sTab = sTab + "|  ";
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void log( char* chaine, bool bThr )
{
	if ( bDesactiveLog )		return;

    string aff = sTab + string(chaine);
    if ( bFirstStart )
    {
		m_logs_string.lock();
		logs_string.push_back( aff );
		m_logs_string.unlock();
		return;
	}

	//---------------------------------------------------
	// Si affichage pendant log_thread
	if ( bThr )
		printf( "log+ : %s\n", aff.c_str() );
	else
		printf( "log  : %s\n", aff.c_str() );
		
	

    if ( panelStdOut && bStdOut )			panelStdOut->affiche( (char*)aff.c_str() );
    //-----------------------------------------------------------
    // Si le programme n'est pas entre dans idleGL()
    //-----------------------------------------------------------

    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void log( char* chaine )
{
	log( chaine, false );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void logf(char *fmt, ...)
{
    char chaine[1024];
    va_list arglist;

    va_start( arglist, fmt );
    vsnprintf( chaine, sizeof(chaine), fmt, arglist );
    va_end( arglist );
    
    log((char*)chaine);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void log_thread_aff()
{
	if ( bFirstStart )			return;

	m_logs_string.lock();
	
	int n = logs_string.size();
	if ( n == 0 )
	{
		m_logs_string.unlock();
		return;
	}
	
	for( int i=0; i < n; i++ )
	{
		log( (char*)logs_string[i].c_str(), true );
	}

	n = logs_string.size();
	//printf( "nb log_thread_aff() = %d\n", n );
    
	try
	{
	  logs_string.clear();
	}
	catch(std::exception& e)
	{
		printf( "try catch nb log_thread \n" );
		std::cout << e.what() << std::endl;
		//return EXIT_FAILURE;
	}
    
	m_logs_string.unlock();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
//#define DEBUG_LOG_THREAD
void log_thread( char* chaine )
{
   #ifdef DEBUG_LOG_THREAD
	string n = to_string( logs_string.size() );
	string aff = sTab + n + "-" + chaine;
   #else
	string aff = sTab + chaine;
   #endif	

	m_logs_string.lock();
    logs_string.push_back( aff );
	m_logs_string.unlock();
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void logf_thread(char *fmt, ...)
{
    char chaine[1024];
    va_list arglist;

    va_start( arglist, fmt );
    vsnprintf( chaine, sizeof(chaine), fmt, arglist );
    va_end( arglist );
    
    log_thread((char*)chaine);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void parse_size( string s )
{
    std::size_t found = s.find("x");
    int l = s.size();
    
    if (found!=std::string::npos)
    {
        logf_thread( (char*)"Period found at: %d ", found );

        char buff[100];
        sprintf( buff, "%s", s.c_str() );
        buff[found] = 0;
        buff[l] = 0;
        
        char *sWidth = buff;
        char *sHeight = buff + found +1;
        
        width  = atoi(sWidth);
        height = atoi(sHeight);
        
        logf_thread( (char*) "Parse %dx%d", sWidth, sHeight );
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void parse_option( int argc, char**argv )
{
	logf_thread( (char*)"main::parse_option()" );

    bool bOK = false;
    char        cSize[100];

    for (;;) {
            int idx;
            int c;

            c = getopt_long(argc, argv, short_options, long_options, &idx);

			if (c == -1)
			{
           		//printf( "[Erreur] parse option\n" );
				break;
			}

            switch (c) {
            case 0: /* getopt_long() flag */
                    break;

            case 'd':
                    strncpy( dev_name, optarg, sizeof(dev_name));
                    Camera_mgr::getInstance().add(dev_name);
                    //log((char*)"Fin**");
                    bPng = false;
                    break;

            case 'e':
                    strncpy( dev_name, optarg, sizeof(dev_name));
                    Connexion_mgr::getInstance().get_exclude().push_back( dev_name );
                    break;

            case 'h':
                    usage(stdout, argc, argv);
                    exit(EXIT_SUCCESS);

            case 'f':
                    bFull = true;
                    width  = widthScreen;
                    height = heightScreen;
                    break;

            case 'l':
            		bDesactiveLog = false;
            		bStdOut = true;
            		logf_thread( (char*)"option -l" );
                    break;

            case 'L':
            		bDesactiveLog = true;
            		bStdOut = true;
            		logf_thread( (char*)"option -L" );
                    break;

            case 'o':
            		bDesactiveLog = false;
            		bStdOut = false;
            		logf_thread( (char*)"option -o" );
            		//printf( (char*)"option -o\n" );
                    break;


            case 's':
                    strncpy( cSize, optarg, sizeof(cSize) );

                    logf_thread( (char*)"%s", cSize );
                    parse_size(cSize);
                    break;

            default:
                    usage(stderr, argc, argv);
                    exit(EXIT_FAILURE);
                    break;
            }
    }
    
    //camera.setDevName(dev_name);

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void parse_option_size( int argc, char**argv )
{
    char        cSize[100];

    for (;;) {
            int idx;
            int c;

            c = getopt_long(argc, argv, short_options, long_options, &idx);

            if (-1 == c)
                    break;

            switch (c) {
            case 's':
                    strncpy( cSize, optarg, sizeof(cSize) );

                    //string      sSize=string((const char*)cSize);
                    printf( "%s\n", cSize );
                    parse_size(cSize);
                    break;
            }
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#include <X11/Xlib.h>
void getX11Screen()
{
	logf_thread( (char*)"main.c::getX11Screen()" );
    Display *display;
    Screen *screen;

    // open a display
    display = XOpenDisplay(NULL);

    // return the number of available screens
    int count_screens = ScreenCount(display);

	logf_thread( (char*)"|  Total count screens: %d", count_screens);


    for (int i = 0; i < count_screens; ++i) {
        screen = ScreenOfDisplay(display, i);
        
        logf_thread( (char*)"|  Screen %d: %dX%d", i + 1, screen->width, screen->height);
        
        widthScreen = screen->width;
        heightScreen = screen->height;
    }
    //glVecAD();

    // close the display
    XCloseDisplay(display);
    


    Display *dpy;
    XRRScreenResources *scr;
    XRRCrtcInfo *crtc_info;

    //dpy = XOpenDisplay(":0");
    dpy = XOpenDisplay(NULL);
    scr = XRRGetScreenResources (dpy, DefaultRootWindow(dpy));
    //0 to get the first monitor   
    crtc_info = XRRGetCrtcInfo (dpy, scr, scr->crtcs[0]);         

    logf_thread( (char*)"|  Nb crtc   : %d", scr->ncrtc );         
    logf_thread( (char*)"|  Nb Output : %d", scr->noutput );         
    logf_thread( (char*)"|  N  mode   : %d", scr->nmode );    
    
    for( int i=0; i<scr->ncrtc; i++ )
    {
        crtc_info = XRRGetCrtcInfo (dpy, scr, scr->crtcs[i]);
        logf_thread( (char*)"|  XRRGetCrtcInfo %d - %dx%d", i, crtc_info->width, crtc_info->height );
        if ( i == 0 )
        {
            widthScreen  = crtc_info->width;
            heightScreen = crtc_info->height;
        }
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void charge_var()
{
    var.charge();

	//------------------------------------------------------------------------------------
	//                                BOOLEEN
    LOAD_VARB( bModeManuel, false );
    LOAD_VARB( bMouseDeplace, false );
    LOAD_VARB( bSuivi, false );
    LOAD_VARB( bPanelCourbe, false );
    LOAD_VARB( bPanelHelp, true );
    LOAD_VARB( bPanelResultat, false );
    LOAD_VARB( bPanelStdOut, false );
    LOAD_VARB( bPanelEnregistre, false );
    LOAD_VARB( bPanelSerial, false );

    LOAD_VARB( bAfficheVec, true )
    LOAD_VARB( bPause, true )
    LOAD_VARB( bFull, true )

    LOAD_VARB( bAsservissement, true )
    LOAD_VARB( bNuit, false )

    LOAD_VARB( bSimu, false );
	LOAD_VARB( bAffSuivi, false);
	LOAD_VARB( bAffCentre, false);
	LOAD_VARB( bSound, true); 
	LOAD_VARB( bInverseCouleur, false); 

	LOAD_VARB( bPleiade, false );
	LOAD_VARB( bAffColimation, false );
	LOAD_VARB( bAffFindStar, false );

	INIT_VARB( bVerboseArduino, false );
	INIT_VARB( bAffFitsCorrection, true );

	LOAD_VARB( bAffCatalog, true);
	LOAD_VARB( bAffStar, true);

	LOAD_VARB( bAffIconeCapture, true );
	LOAD_VARB( bCoordHoriz, false );

	//------------------------------------------------------------------------------------
	//                                REEL
    LOAD_VARF( vecAD[0].x, 0.0 )
    LOAD_VARF( vecAD[0].y, 0.0 )
    LOAD_VARF( vecAD[0].z, 0.0 )
    
    LOAD_VARF( vecAD[1].x, 0.0 )
    LOAD_VARF( vecAD[1].y, 0.0 )
    LOAD_VARF( vecAD[1].z, 0.0 )

    LOAD_VARF( vecDC[0].x, 0.0 )
    LOAD_VARF( vecDC[0].y, 0.0 )
    LOAD_VARF( vecDC[0].z, 0.0 )
    
    LOAD_VARF( vecDC[1].x, 0.0 )
    LOAD_VARF( vecDC[1].y, 0.0 )
    LOAD_VARF( vecDC[1].z, 0.0 )

    LOAD_VARF( xSuivi, 800.0 )
    LOAD_VARF( ySuivi, 450.0 )

	LOAD_VARF( fTimeCorrection, 3.0 );
	LOAD_VARF( fLimitCorrection0, 45 );
	LOAD_VARF( fLimitCorrection1, 45 );
	LOAD_VARF( filtre, 14.0);

	LOAD_VARF( pos_telescope.ad, 5.0);
	LOAD_VARF( pos_telescope.dc, 5.0);

	LOAD_VARF( Xref, 999);
	LOAD_VARF( Yref, 555);
	LOAD_VARF( ZrefX, 935);
	LOAD_VARF( ZrefY, 1030 );
	LOAD_VARF( Wref, 5.5);
   	
	LOAD_VARF( dErr, 0.63 );
	INIT_VARF( fGmagMax, 17.6 );


	//------------------------------------------------------------------------------------
	//                          POSITIONS DES FENETRES
	INIT_VARI( PanelCourbe_X, 10 );
	INIT_VARI( PanelCourbe_Y, 10 );
	INIT_VARI( PanelCourbe_DX, 1496 );
	INIT_VARI( PanelCourbe_DY, 400 );

	INIT_VARI( PanelHelp_X, 210 );
	INIT_VARI( PanelHelp_Y, 100 );
	INIT_VARI( PanelHelp_DX, 1200 );
	INIT_VARI( PanelHelp_DY, 650 );

	INIT_VARI( PanelSerial_X, 10 );
	INIT_VARI( PanelSerial_Y, 10 );
	INIT_VARI( PanelSerial_DX, 586 );
	INIT_VARI( PanelSerial_DY, 600 );

	INIT_VARI( PanelStdOut_X, 122 );
	INIT_VARI( PanelStdOut_Y, 180 );
	INIT_VARI( PanelStdOut_DX, 867 );
	INIT_VARI( PanelStdOut_DY, 569 );
	
	INIT_VARF( dLatloc, 0 );
	INIT_VARF( dLonloc, 0 );

	//------------------------------------------------------------------------------------

    if ( bFull )        		glutFullScreen();

    compute_matrix();
   	
   	bCoordHoriz_old = bCoordHoriz;


    change_ad_status( pos_telescope.ad );
    change_dc_status( pos_telescope.dc );
}
//--------------------------------------------------------------------------------------------------------------------
//
//										MM   MM   AAA   II  NN    N
//										M M M M  A   A  II  N N   N
//										M  M  M  AAAAA  II  N  N  N
//										M     M  A   A  II  N   N N
//										M     M  A   A  II  N    NN
//
//--------------------------------------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
	//----------------------------------------------------------------------
	// Parse ligne de commande
	//----------------------------------------------------------------------
    parse_option(argc, argv);
    atexit(exit_handler);
    
    xCam = 0;
    yCam = 0;
    dxCam = 1920;
    dyCam = 1080;

	//----------------------------------------------------------------------
	// Init info X11
	//--------------
	// resolution etc...
	//----------------------------------------------------------------------
    getX11Screen();

	//----------------------------------------------------------------------
	// OPENGL
	//--------
	// Initialise les fonctions openGL
	//----------------------------------------------------------------------
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	xPos = 0.0;
	yPos = 0.0;
    width = getScreenDX();
    height = getScreenDY();
    
    logf_thread( (char*)"Screen size %dx%d", width, height );

	glutInitWindowPosition(xPos, yPos);
	glutInitWindowSize( width, height );

	if (glutCreateWindow("AstroPilot") == 0)		return 1;

	glutReshapeFunc(reshapeGL);
	glutDisplayFunc(displayGL);
	glutIdleFunc(idleGL);

	logf_thread( (char*)"Fenetre %d,%d %dx%d ", xPos, yPos, width, height );

	glutKeyboardFunc( KeyboardFunc);
	glutKeyboardUpFunc( KeyboardUpFunc);
	glutSpecialFunc( SpecialFunc);
	glutSpecialUpFunc( SpecialUpFunc);
	
	glutMouseFunc(glutMouseFunc);
	glutMotionFunc(glutMotionFunc);
	glutPassiveMotionFunc(glutPassiveMotionFunc);

	initGL(argc, argv);
    glewInit();
    
	//----------------------------------------------------------------------
	// FENETRES
	//-----------
	// Init des repertoires (si il n'existe pas)
	// Charge les variables
	// Initialise les fenetres
	//----------------------------------------------------------------------
    test_creer_dir( (char*)"/home/rene/.astropilot" );
    test_creer_dir( (char*)"/home/rene/.astropilot/vizier" );
    charge_var();
    CreateAllWindows();

	//----------------------------------------------------------------------
	// CONNECTIONS
	//---------------
	// Initialise les traitement de connections
	//   Arduino, Stellarium(PC), Stellarium(Android), Camera
	//
	//----------------------------------------------------------------------
    Camera_mgr::getInstance().reOrder();
    Connexion_mgr::getInstance().startDEV();
    Connexion_mgr::getInstance().startZWO();
    PanelConsoleSerial::getInstance().setVisible( bPanelSerial );
    Serveur_mgr::getInstance().start_init();
    Serveur_mgr::getInstance().start_deplacement();
    LX200::getInstance().start_lx200();
    SYNSCAN::getInstance().start_synscan();
	//----------------------------------------------------------------------

    var.startSauve();
	    
    double clearColor = 0.0;
    glClearColor( clearColor, clearColor, clearColor,1.0);
    
	//----------------------------------------------------------------------
    // FileBrowser
    //------------
    // Pre-Charge la texture pour eviter un bug 
	//----------------------------------------------------------------------
    WindowsManager::getInstance().loadResourceImage( "images/file.png" );
    WindowsManager::getInstance().loadResourceImage( "images/dir.png" );
    FileBrowser::getInstance();

	//----------------------------------------------------------------------
    //BluetoothManager::getInstance();
    WindowsManager::genereMipMap( false );
	setColor();
    compute_matrix();

	// -----------------------------------
	// -----------------------------------
	// -----------------------------------         EN ROUTE    
    glutMainLoop();
	// -----------------------------------         BYE BYE
	// -----------------------------------
	// -----------------------------------

	return 0;
}



#endif
