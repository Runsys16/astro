#ifndef MAIN_CPP
#define MAIN_CPP

#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

#include "Python.h"

#include "main.h"
#include "v4l2.h"
#include "camera.h"
#include "camera_mgr.h"
#include "connexion_mgr.h"
#include "timer.h"
#include "pleiade.h"
#include "panel_console_serial.h"
#include "panel_spin_edit_text.h"
#include "serveur_mgr.h"
#include "capture.h"
#include "var_mgr.h"
#include "alert_box.h"
#include "file_browser.h"
#include "captures.h"
#include "bluetooth.h"
#include "panel_courbe.h"
#include "panel_apn.h"
#include "panel_stdout.h"
#include "star_catalogue.h"
#include "catalog.h"
#include <GL/freeglut_ext.h>

#define SIZEPT  20
//#define DEBUG 1
//#define IDLEGL
#define COLOR_GREY      0xFF404040
#define COLOR_WHITE      0xFFFFFFFF
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
vector<string> t_sHelp1 =
{
	"---- CAMERA ----",
	"     H\t: Change le nom d une image de la camera",
	"     h\t: Enregistre une image de la camera courante",
	"     A\t: enregistre les parametres de la camera",
	"     a\t: rappelle les parametres de la camera",
    "" ,    
	"\t  Brightness\t\tB/b" ,
	"\t  Contrast\t\tC/c" ,
	"\t  Saturation\t\tS/s" ,
	"\t  Hue\t\t\tH/h" ,
	"\t  Gamma\t\tG/g" ,
	"\t  Sharpness\t\tZ/z" ,
	"\t  Exposure\t\tE/e" ,
	"\t  Exposure auto\t\tD/d" ,
	"\t  White balance\tW/w" ,
	"",
	"---- MODE NORMAL ----",
	"ctrl+TAB\t: camera suivante" ,
	"TAB\t: Change l'affichage des fichiers" ,
	"     b\t: Affiche les informations des fichiers fits",
	"     B\t: Arduino bavard",
	"     f\t: Ouvrir un fichier image",
	"     F\t: Active/Desactive la simu",
	"     i\t: Prend une photo sur le PENTAX",
	"     I\t: Inverse les couleur pour la recherhce d'une etoile",
	"     J\t: Lance Stellarium",	
	"     j\t: Affichage info fits",
	"     k\t: Active/desactive le son",
	"     K\t: Lance un carree de recherche",
	"     l\t: List les ports /dev + les controles ",
	"     L\t: List les variables",
	"     N\t: Mode nuit on/off",
	"     o\t: Ouvre/Ferme la fenetre pleiades",
	"     p\t: Pause de l'affichage de pleiades",
	"     P\t: Image suivante",
	"     q\t: Lance un script python",
	"      \t: Lance VIZIER",
	"     Q\t: Mise en station via polaris",
//	"     r\t: Test alert BOX",
	"     W\t: Surveille un repertoire",
	"     -\t: Toutes les images sont affichees en icones",
	"",
	"---- TRANSFORM MATRIX ----",
	"   a/A\t: Vecteur en ascension droite",
	"   d/D\t: Vecteur en declinaison",
	"     m\t: Deplacement à la souris",
	"     M\t: Calcul la matrice de transformation",
	"     O\t: Mode souris / mode suivi",
	"     y\t: Affiche les vecteurs",
	"",
	"---- TRACES ----",
	"     C\t: Lance/arrete l\'enregistrement de trace",
	"     c\t: Nouvelle trace",
	"     e\t: Affiche les traces (ctrl+tab)",
	"     E\t: Supprime la derniere trace",
	"     z\t: Charge les traces",
	"     Z\t: Sauve les traces"
};
vector<string> t_sHelp2 = 
{
	"---- TOUCHE DE FONCTION ----",
	"    F1\t: Help",
	"    F2\t: Panneau de controle de la camera",
	"    F3\t: Panneau de suivi",
	"    F4\t: Courbe de suivi",
	"    F5\t: Console de log",
	"    F6\t: Console arduino",
	"    F7\t: Affiche/Cache les images",
	"    F8\t: Prends une photo avec le pentax",
	"   F10\t: Mode DEBUG WindowManager",
	"   F11\t: Charge la prochaine image",
	"   F12\t: Efface la derniere image", 
	"    CR\t: Plein Ecran", 
	"",
	"---- SUIVI ----",
	"ctrl+D\t: Efface toutes les etoiles",
	"     s\t: Recherche toutes les etoiles",
	"     S\t: Lance/Stop le suivi",
	"   t/T\t: change le temps de correction",
    "     U\t: Affichage du centre de la camera on/off",
    "     u\t: Affichage du suivi on/off",
	"     V\t: Initialise les coordonnees de suivi",
	"     v\t: Sauvegarde fichier de suivi (.guid)",
	"     r\t: Charge ficher de suivi (.guid)",
	"     w\t: Centre l'asservissement",
	"     Y\t: Lance l'asservissement",
	"",
	"---- FFT ----",
	"   g/G\t: Change la valeur du filtre"   ,
	"     x\t: Affiche/Cache la fft",
	"     X\t: Choix de l'abcisse ou de l'ordonnée",
	"",
	"---- Vizier ----",
	" Alt+e\t: Affiche catalog"   ,
	"   d/f\t: Rotation"   ,
	"   a/z\t: Translation X"   ,
	"   q/s\t: Translation Y"   ,
	"   w/x\t: Zoom X"   ,
	"   c/v\t: Zoom Y"   ,
	"",
	"ctrl+q\t: --- SORTIE DU LOGICIEL ---" 
};
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
VarManager& var = VarManager::getInstance();
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
string              currentDirectory = "/home/rene/Documents/astronomie/logiciel/script/image/atmp/2019-06-30/";
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
//PanelConsole *      pConsoleSerial;
PanelWindow *       panelHelp;
PanelWindow *       panelResultat;
PanelCourbe *       panelCourbe;
PanelStdOut *       panelStdOut;
PanelApn*           panelApn;
PanelSimple *       panelStatus;


PanelText*          pCamFilename;
PanelText*          pRef;
PanelText*          pEtoile;
PanelText*          pEcart;
PanelText*          pCoordSuivi;
PanelText*          pSuivi;
PanelText*          pArduino;
PanelText*          pStellarium;
PanelText*          pPas;
//PanelText*          pErr;
//PanelText*          pUrg;
PanelText*          pDeplacement;
PanelText*          pAD;
PanelText*          pDC;
//PanelText*          pMode;
PanelText*          pAsservi;
/*
PanelText*          pXMax;
PanelText*          pXMin;
PanelText*          pYMax;
PanelText*          pYMin;
*/

PanelText*          pHertz;
PanelText*          pFPS;
//PanelText *         pStatus;

Pleiade*            pPleiade;

double               ac;
double               dc;

//Device_cam          camera      = Device_cam();

char                background[]="frame-0.raw";
double               prevTime    = -1.0f;

int                 width  = 1600;
int                 height = 900;
//int                 width  = 950;
//int                 height = 534;
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


bool                bPanelControl  		= true;
bool                bPanelHelp     		= false;
bool                bPanelResultat 		= false;
bool                bPanelCourbe   		= false;
bool                bPanelStdOut   		= false;
bool                bPanelSerial		= false;
bool                bAfficheVec			= false;
bool                bMouseDeplace		= false;
bool                bMouseDeplaceVers	= false;
bool                bFileBrowser		= false;
bool                bStellarium			= false;
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

int                 wImg;
int                 hImg;
int                 nPlanesImg;

//ivec2               vCameraSize;
//GLubyte*            ptr;
vector<string>      exclude;

int                 _r[256];
int                 _g[256];
int                 _b[256];

double               xSuivi;
double               ySuivi;
double               xSuiviSvg;
double               ySuiviSvg;
double               filtre      = 10.0;
int                 iDisplayCourbe  = 0;
bool                bDisplayCourbeX = true;
bool                bDisplayCourbeY = true;

int                 iDisplayfft = 0;
bool                bDisplayfftX = true;
bool                bDisplayfftY = true;

double               fTimeMili;
//--------------------------------------------------------------------------------------------------------------------
//              Ratio Witdh Height
//--------------------------------------------------------------------------------------------------------------------
//double              rw;
//double              rh;

int                 xCam;
int                 yCam;
int                 dxCam;
int                 dyCam;

double               zoom;

ivec2               mouse;

#define             GET_OFFSET(x,y,width)   (3*y*width+x)
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
//vector<vec2>        t_vResultat;
//vector<vec2>        t_vSauve;
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

double               pas = 4000;
double               cAD;
ivec2               calibreMove[2];
vec2                vRef;
vec3                vecAD[2];
vec3                vecDC[2];
vec3                vDepl[2];
mat3                mChange;
vec3                vOri;
vec3                vTr;
bool                bCorrection = false;
double              fTimeCorrection = 3.0;
double               fTimeCpt = 0.0;
double              fLimitCorrection = 80.0;
double              pas_sideral;

double               fpos_ad = -1.0;
double               fpos_dc = -1.0;
//double               err = 2.0;
//#define err         err
//--------------------------------------------------------------------------------------------------------------------
string              sTab;
int                 nb_tab = 0;
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
vector<AlertBox*>   tAlert;
bool                bAlert = false;
string              sAlert;
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
double fTimer10s = 0.0;
bool bAsc = true;
bool bDec = true;
bool bSui = false;
bool bJoy = false;
bool bRet = false;
/*
//--------------------------------------------------------------------------------------------------------------------
double Xref = -402.0;
double Yref = -365.0;
double Zref = 782.0;
//
//--------------------------------------------------------------------------------------------------------------------
*/
double Xref  = -1.0;
double Yref  = -1.0;
double ZrefX = -1.0;
double ZrefY = -1.0;
double Wref  = -1.0;
bool   bAffCatalog = true;
Catalog vizier = Catalog();

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
string workDirCaptures = "/home/rene/Documents/astronomie/logiciel/script/image/atmp/2000-01-01/frame/";
string workDirSauveCourbe = "/home/rene/.astropilot/";
string workDirFileBrowser = "/home/rene/Documents/astronomie/logiciel/script/image/atmp/2000-01-01/";
string filenameSauve = "/home/rene/.astropilot/sauvegarde.txt";
string workDirFits = "/home/rene/Documents/astronomie/fits/";
//--------------------------------------------------------------------------------------------------------------------
CallbackSauveGuidage    cb_sguidage;
CallbackChargeGuidage   cb_cguidage;
CallbackFileBrowser     cb_file_browser;
CallbackFits            cb_fits;
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static const char short_options[] = "d:hfs:lpe:";
static const struct option
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
long_options[] = {
        { "device", required_argument, NULL, 'd' },
        { "help",   no_argument,       NULL, 'h' },
        { "format", no_argument,       NULL, 'f' },
        { "size",   required_argument, NULL, 's' },
        { "log",    no_argument,       NULL, 'l' },
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
static void usage(FILE *fp, int argc, char **argv)
{
        fprintf(fp,
                 "Usage: %s [options]\n\n"
                 "Version 1.3\n"
                 "Options:\n"
                 "-d | --device name   Video device name [/dev/video(n)]\n"
                 "-h | --help          Print this message\n"
                 "-f | --full          Full size windows\n"
                 "-l | --log           Affiche les logs\n"
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
    bCorrection = false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void sound_alert()
{
    string cmd = "aplay /usr/share/sounds/purple/alert.wav;";
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

    string filename = "/home/rene/Documents/astronomie/logiciel/calcul/calcul.py";
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
    string command = "stellarium &";
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
    string command = "/home/rene/Documents/astronomie/logiciel/asi-studio/ASIStudio 2>&1 >/dev/null &";
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
/*
    RA_ICRS (deg)   (F15.11) Barycentric right ascension (ICRS) at Ep=2015.5 (ra) [ucd=pos.eq.ra;meta.main]
    e_RA_ICRS (mas) (F7.4)  Standard error of right ascension (e_RA*cosDE) (ra_error) [ucd=stat.error;pos.eq.ra]
    DE_ICRS (deg)   (F15.11) Barycentric declination (ICRS) at Ep=2015.5 (dec) [ucd=pos.eq.dec;meta.main]
    e_DE_ICRS (mas) (F7.4)  Standard error of declination (dec_error) [ucd=stat.error;pos.eq.dec]
    Source          (I19)   Unique source identifier (unique within a particular Data Release) (source_id) (G2) [ucd=meta.id;meta.main]
    Plx (mas)       (F10.4) ? Absolute stellar parallax (parallax) [ucd=pos.parallax]
    e_Plx (mas)     (F7.4)  ? Standard error of parallax (parallax_error) [ucd=stat.error;pos.parallax]
    pmRA (mas/yr)   (F9.3)  ? Proper motion in right ascension direction (pmRA*cosDE) (pmra) (3) [ucd=pos.pm;pos.eq.ra]
    e_pmRA (mas/yr) (F6.3)  ? Standard error of proper motion in right ascension direction (pmra_error) [ucd=stat.error;pos.pm;pos.eq.ra]
    pmDE (mas/yr)   (F9.3)  ? Proper motion in declination direction (pmdec) (4) [ucd=pos.pm;pos.eq.dec]
    e_pmDE (mas/yr) (F6.3)  ? Standard error of proper motion in declination direction (pmdec_error) [ucd=stat.error;pos.pm;pos.eq.dec]
    Dup             (I1)    [0/1] Source with duplicate sources (duplicated_source) (21) [ucd=meta.code.status]
    FG (e-/s)       (E11.4) G-band mean flux (phot_g_mean_flux) [ucd=phot.flux;stat.mean;em.opt]
    e_FG (e-/s)     (E11.4) Error on G-band mean flux (phot_g_mean_flux_error) [ucd=stat.error;phot.flux;stat.mean]
    Gmag (mag)      (F7.4)  G-band mean magnitude (Vega) (phot_g_mean_mag) (23) [ucd=phot.mag;stat.mean;em.opt]
    e_Gmag (mag)    (F6.4)  Standard error of G-band mean magnitude (Vega) (added by CDS) (phot_g_mean_mag_error) (37) [ucd=stat.error;phot.mag;stat.mean]
    FBP (e-/s)      (E11.4) ? Mean flux in the integrated BP band (phot_bp_mean_flux) [ucd=phot.flux;stat.mean;em.opt.B]
    e_FBP (e-/s)    (E11.4) ? Error on the integrated BP mean flux (phot_bp_mean_flux_error) (25) [ucd=stat.error;phot.flux;stat.mean]
    BPmag (mag)     (F7.4)  ? Integrated BP mean magnitude (Vega) (phot_bp_mean_mag) (26) [ucd=phot.mag;stat.mean;em.opt.B]
    e_BPmag (mag)   (F6.4)  ? Standard error of BP mean magnitude (Vega) (added by CDS) (phot_bp_mean_mag_error) (37) [ucd=stat.error;phot.mag;stat.mean]
    FRP (e-/s)      (E11.4) ? Mean flux in the integrated RP band (phot_rp_mean_flux) [ucd=phot.flux.density;em.opt.R]
    e_FRP (e-/s)    (E11.4) ? Error on the integrated RP mean flux (phot_rp_mean_flux_error) (28) [ucd=stat.error;phot.flux;stat.mean]
    RPmag (mag)     (F7.4)  ? Integrated RP mean magnitude (Vega) (phot_rp_mean_mag) (29) [ucd=phot.mag;stat.mean;em.opt.R]
    e_RPmag (mag)   (F6.4)  ? Standard error of RP mean magnitude (Vega) (added by CDS) (phot_rp_mean_mag_error) (37) [ucd=stat.error;phot.mag;stat.mean]
    BP-RP (mag)     (F7.4)  ? BP-RP colour (photBpMeanMag-photRMeanMag) (bp_rp) [ucd=phot.color;em.opt.B;em.opt.R]
    RV (km/s)       (F7.2)  ? Spectroscopic radial velocity in the solar barycentric reference frame (radial_velocity) [ucd=spect.dopplerVeloc.opt;pos.barycenter]
    e_RV (km/s)     (F5.2)  ? Radial velocity error (radial_velocity_error) (30) [ucd=stat.error;spect.dopplerVeloc.opt;pos.barycenter]
    Teff (K)        (F7.2)  ? Stellar effective temperature (estimate from Apsis-Priam) (teff_val) [ucd=phys.temperature.effective]
    AG (mag)        (F7.4)  ? Estimate of extinction in the G band from Apsis-Priam (a_g_val) [ucd=phys.absorption.gal]
    E(BP-RP) (mag)  (F7.4)  ? Estimate of redenning E(BP-RP) from Apsis-Priam (e_bp_min_rp_val) [ucd=phot.color.excess]
    Rad (solRad)    (F6.2)  ? Estimate of radius from Apsis-FLAME (radius_val) [ucd=phys.size.radius]
    Lum (solLum)    (F9.3)  ? Esimate of luminosity from Apsis-FLAME (lum_val) [ucd=phys.luminosity]
*/
//--------------------------------------------------------------------------------------------------------------------
void vizier_parse_line( Catalog* pVizier, string & line )
{
    if ( line.size() < 294 )        return;

    double fRA = stod( line.substr(0,15), 0 );
    double fDE = stod( line.substr(16,15), 0 );
    string name = line.substr(34, 17);
    double fMag = stod( line.substr(167,15 ), 0 );
 
    StarCatalog* p = new StarCatalog( fRA, fDE, fMag, name );
    pVizier->add(p);
    
    logf( (char*)"main::vizier_parse_line() Etoile '%s'\t(%0.7f,\t%0.7f)\tmag=%0.4f", (char*)name.c_str(), (double)fRA, (double)fDE, (double)fMag );

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void vizier_thread( Catalog* pVizier, string s )
{
    logf( (char*)"main::vizier_thread()" );
    log_tab(true);
    string find;
    
    if ( s == "" )    {
        find = "find_gaia_dr3.py -r 10200 -m 3000 --Gmag=\"<8\" m45";
    }
    else    {
        find = "find_gaia_dr3.py -m 3000 --Gmag=\"<16\" " + s;
    }
    
    logf( (char*)"Lance la requete : %s", find.c_str() );

    string rep = "/home/rene/Documents/astronomie/logiciel/python/cds.cdsclient/cdsclient/";

    char buf1[BUFSIZ]; //BUFSIZ est une constante connue du système
    FILE *ptr;
    string cmd = rep + find;
    bool bRead = false;
    bool bEntete = false;
 
    if ((ptr = popen(cmd.c_str(), "r")) != NULL)    {
    
        while (fgets(buf1, BUFSIZ, ptr) != NULL)        {
            string s = string( buf1 );
            if ( s.find( "-----------" ) == 0 )            {
                bRead = !bRead;
                bEntete = true;
                continue;
            }

            if ( s.find( "#END#" ) == 0 )            {
                bRead = true;
                bEntete = false;
                continue;
            }
			if ( !bRead && bEntete  )			vizier_parse_line( pVizier, s );
        }
        pclose(ptr);
    }
    else    {
        fprintf(stderr, "Echec de popen\n");
        exit(1);
    }
    logf( (char*)"%d etoiles trouvees", pVizier->size() );
    log_tab(false);
    logf( (char*)"main::vizier_thread()  FIN" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void vizier_load_stars( Catalog* pVizier, string s, double ra, double de )
{
    pVizier->efface();

    thread( &vizier_thread, pVizier, s ).detach();
    Camera_mgr::getInstance().setRefCatalog( ra, de );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void vizier_load_stars( Catalog* pVizier, string s )
{
    pVizier->efface();

    thread( &vizier_thread, pVizier, s ).detach();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void vizier_load_stars( string s, double ra, double de )
{
	vizier_load_stars( &vizier, s, ra, de );
/*
*    vizier.efface();

    thread( &vizier_thread, s ).detach();
    Camera_mgr::getInstance().setRefCatalog( ra, de );
   */
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void vizier_load_stars( string s )
{
	vizier_load_stars( &vizier, s );
/*
    vizier.efface();

    thread( &vizier_thread, s ).detach();
*/
}
//--------------------------------------------------------------------------------------------------------------------
//
// Force le lancement de la commande 'g' sur arduino 
// voir idleGL()
//
//--------------------------------------------------------------------------------------------------------------------
void get_info_arduino()
{
	fTimer10s = 9.0;
    logf ( (char*)"main.c::get_info_arduino() fTimer = %0.2f", fTimer10s );
}
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

    return a<<24 | r<<16 | g<<8 | b;
}
//--------------------------------------------------------------------------------------------------------------------
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
    logf( (char*)"CallbackSauveGuidage::callback( %s, %d, \"%s\" )", bb?(char*)"true":(char*)"false", ii, (char*)str );

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
    
    logf( (char*)"CallbackFileBrowser::callback( %s, %d, \"%s\" )", bb?(char*)"true":(char*)"false", ii, (char*)str );
    if ( bb )     
    {
        string f = fb.getFilename();
        string d = fb.getWorkingDir();
        logf( (char*)"  charge capture %s%s", (char*)d.c_str(), (char*)f.c_str() );
        charge_image( d, f );
    }
    workDirFileBrowser = fb.getWorkingDir();
    VarManager& var = VarManager::getInstance();
    var.set( "DirFileBrowser", workDirFileBrowser );
    fb.setFocus();
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
    if ( p )        p->setFilenameRec( string(str) );

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
    Captures::getInstance().onTop(p);
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
    return (DMS.d + DMS.m/60.0 + DMS.s/3600.0) / 180.0*M_PI;
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
void rad2dms(double v, struct dms& DMS)
{
    double sign = 1.0;
    if ( v<0.0 )
    {
        sign = -1.0;
        v *=-1.0;
    }

    double d = v / M_PI * 180.0;
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

    vec2* pv = Camera_mgr::getInstance().getSuivi();
    if ( pv != NULL )
    {
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

        if ( bCorrection )
        {
            sprintf( sStr, "Asserv\t\t%0.2f/%0.2fpx", l, fLimitCorrection );
        }
        else{
            sprintf( sStr, "Ecart\t\t%0.2f/%0.2fpx", l, fLimitCorrection );
        }
        pEcart->changeText(sStr);
        
        double xx = pv->x + 40;
        double yy = pv->y + 35;
        
        mgr.tex2screen(xx,yy);

        if ( (xx + panelResultat->getDX()) > (wm.getWidth()+10) )       xx -= panelResultat->getDX();
        if ( (yy + panelResultat->getDY()) > (wm.getHeight()+10) )      yy -= panelResultat->getDY();
        panelResultat->setPos( xx, yy );
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void displayGL(void)
{
    //logf( (char*)"*** DISPLAY GL ***" );
	WindowsManager::getInstance().clearBufferGL(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

    if ( var.getb("bNuit") )        glColor4f( 1.0, 0.0, 0.0, 0.2 );
    else                            glColor4f( 1.0, 1.0, 1.0, 1.0 );

	WindowsManager::getInstance().displayGL();

	glutSwapBuffers();

    bStdOut = true;

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void reshapeGL(int newWidth, int newHeight)
{
    logf((char*) "main::reshapeGL(%d, %d) ---------------", newWidth, newHeight);
    log_tab(true);

	WindowsManager& wm = WindowsManager::getInstance();
	wm.setScreenSize( newWidth, newHeight );

    //resizePreview(newWidth, newHeight);
    //resizeControl(newWidth, newHeight);
    resizeHelp(newWidth, newHeight);
    resizeCourbe(newWidth, newHeight);

	panelStatus->setPosAndSize( 0, newHeight-20, newWidth, 20 );
	pFPS->setPos( newWidth-100, 5);
	pHertz->setPos( newWidth-150, 5);

	glViewport(0, 0, (GLint)newWidth, (GLint)newHeight);

	//change_fov();
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
    //cout << "reshapeGL("<< newWidth <<" "<< newHeight <<")"<< endl;
    log_tab(false);
    logf((char*) "main::reshapeGL(%d, %d) ---------------", newWidth, newHeight);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void write_image(void)
{
    //camera.mainloop();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void change_hertz(double hz)
{
    static char sHz[255];
    sprintf( sHz, "%.0fHz", hz);
    pHertz->changeText(sHz);
    //camera.mainloop();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void change_arduino(bool b)
{
    logf( (char*)"Change Arduin %s", (char*)BOOL2STR(b) );

    //if ( b )    pArduino->changeText((char*)"Arduino");
    //else        pArduino->changeText((char*)"----");
    
    if ( b )    pArduino->setColor(COLOR_WHITE);
    else        pArduino->setColor(COLOR_GREY);
    
    if ( b )    PanelConsoleSerial::getInstance().setPrompt("Arduino> ");
    else        PanelConsoleSerial::getInstance().setPrompt("No connect> ");

    if ( b )    Serial::getInstance().testVersionArduino();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void change_joy(int x, int y)
{
    static char sJoyXY[255];
    sprintf( sJoyXY, "(%d, %d)", x, y );//, BOOL2STR(bSuivi) );
    pCoordSuivi->changeText((char*)sJoyXY );
    if ( bSuivi )           pSuivi->setColor(COLOR_WHITE);
    else                    pSuivi->setColor(COLOR_GREY);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void change_joy(double x, double y)
{
    static char sJoyXY[255];
    sprintf( sJoyXY, "(%0.2f, %0.2f)", x, y );//, BOOL2STR(bSuivi) );
    pCoordSuivi->changeText((char*)sJoyXY );
    if ( bSuivi )           pSuivi->setColor(COLOR_WHITE);
    else                    pSuivi->setColor(COLOR_GREY);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void change_ad_status(double ad)
{
    if ( fpos_ad == ad)			return;

	Captures::getInstance().change_ad( ad );
    VarManager& var = VarManager::getInstance();

	var.set("fpos_ad", ad);
    fpos_ad = ad;

    //ad = DEG2RAD(ad);
    struct hms HMS;
    deg2hms( ad, HMS);
    
    char    buff[255];
    sprintf( buff, "AD: %02dh %02dm %0.2fs", (int)HMS.h, (int)HMS.m, HMS.s );
    
    pAD->changeText( buff );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void change_dc_status(double dc)
{
    if ( fpos_dc == dc)					return;

	Captures::getInstance().change_dc( dc );
    VarManager& var = VarManager::getInstance();

    var.set("fpos_dc", dc);
    fpos_dc == dc;
    
    //dc = DEG2RAD(dc);
    struct dms DMS;
    deg2dms( dc, DMS);
    
    char    buff[255];
    char 	signe[2];
    //if ( dc <0.0 )		signe[0] = '-';
    //else				signe[0] = 0;
    sprintf( buff, "DC: %s%02d %02d\' %0.2f\"", signe, (int)DMS.d, (int)DMS.m, DMS.s );
    
    pDC->changeText( buff );

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void compute_matrix()
{
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
        panelCourbe->idle_guidage( *pV );        
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
static void idleGL(void)
{
    Timer&          timer = Timer::getInstance();

    if ( bFirstStart && bPause )
    {
        bOneFrame = true;
    }
    bFirstStart = false;

    #ifdef IDLEGL
    log_tab(false);
    logf( (char*)"[%0.4f] IDLE GL ***", timer.getCurrentTime() );
    #endif

	fTimeMili = (double)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    char sFPS[] = "fps 0000";
    sprintf( sFPS,"fps %d", *Timer::getInstance().getvFPSCounter() );
    pFPS->changeText((char*)sFPS);
    timer.Idle();


    #ifdef IDLEGL
    logf( (char*)"[%0.4f] IDLE GL ***", timer.getCurrentTime() );
    log_tab(true);
    #endif


    idleStatus();

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
    if ( panelStdOut->getHaveMove() )
    {
        panelStdOut->resetHaveMove();

        var.set("xPanelStdOut",  panelStdOut->getX() );
        var.set("yPanelStdOut",  panelStdOut->getY() );
        var.set("dxPanelStdOut", panelStdOut->getDX() );
        var.set("dyPanelStdOut", panelStdOut->getDY() );
        
        //if ( panelStdOut->getX() != 0 )    alertBox("xPanelStdOut != 0");
    }

    if ( panelHelp->getHaveMove() )
    {
        panelHelp->resetHaveMove();

        var.set("xPanelHelp",  panelHelp->getX() );
        var.set("yPanelHelp",  panelHelp->getY() );
        var.set("dxPanelHelp", panelHelp->getDX() );
        var.set("dyPanelHelp", panelHelp->getDY() );
        
        //if ( panelStdOut->getX() != 0 )    alertBox("xPanelStdOut != 0");
    }

    if ( panelCourbe->getHaveMove() )
    {
        panelCourbe->resetHaveMove();
        panelCourbe->build_unites_text();

        var.set("xPanelCourbe",  panelCourbe->getX() );
        var.set("yPanelCourbe",  panelCourbe->getY() );
        var.set("dxPanelCourbe", panelCourbe->getDX() );
        var.set("dyPanelCourbe", panelCourbe->getDY() );
        
        //if ( panelStdOut->getX() != 0 )    alertBox("xPanelStdOut != 0");
    }
    //-----------------------------------------------------------------------
    // Mise a jour des buffers de la camera
    // et des pointeurs vers la texture background
    //-----------------------------------------------------------------------
    Camera_mgr::getInstance().update();
    
    Captures::getInstance().update();
    //-----------------------------------------------------------------------
    // Lance le suivi des etoiles
    //-----------------------------------------------------------------------
    if ( bSuivi )   suivi();

    /*
    if (!bModeManuel)
    {
        suivi();
    }
    */
    //-----------------------------------------------------------------------
    //
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
    //-----------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------
	
    double elapsedTime = -1;
	if ( prevTime < 0 )	{
		prevTime = fTimeMili;
	}
	else 	{
		elapsedTime = fTimeMili - prevTime;
		prevTime = fTimeMili;
	}
    
    //WindowsManager::getInstance().idleGL( elapsedTime );
    
	if ( elapsedTime != -1 )
	{
	    fTimer10s += elapsedTime;
	    if ( fTimer10s >= 10.0 )
	    {
	        fTimer10s -= 10.0;
		    //logf( (char*)"Timer 10s %0.2f", fTimer10s );
            
            char cmd[255];
            sprintf( cmd, "g" );
            Serial::getInstance().write_string(cmd, false);
	    }
	    //logf( (char*)"%0.2f", fTimer );
	}
    //------------------------------------------------------
    //------------------------------------------------------
	// CHARGEMENT IMAGE SUR SURVEILLANCE
    //------------------------------------------------------
    //------------------------------------------------------
    if ( Surveillance::getInstance().idleGL() )
    {
        Captures::getInstance().rotate_capture_plus(true);
    }

    //------------------------------------------------------
    //------------------------------------------------------
	// CALCUL DE L' ASSERVISSEMENT
    //------------------------------------------------------
    //------------------------------------------------------
	if ( elapsedTime != -1 )
	{
	    fTimeCpt += elapsedTime;
	    //logf( (char*)"Temps ecoule : %0.2f", fTimeCpt );
	    //if ( bCorrection && fTimeCpt > 0.0 && !bModeManuel)
        vec2* pv = Camera_mgr::getInstance().getSuivi();
        if ( pv != NULL && bCorrection )
        {
            vec3 res, v, w;  double l;

            w = vec3( xSuivi, ySuivi, 0.0);
            v = w - vec3( pv->x, pv->y, 0.0);
            l = v.length();

            if ( l > fLimitCorrection ) {
                logf( (char*)"[WARNING]Suivi l=%0.2f/%0.2f", l, fLimitCorrection ); 
                arret_urgence();
                //system( (char*)"aplay /usr/share/sounds/purple/logout.wav" );
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
    //------------------------------------------------------
    //------------------------------------------------------
    PanelConsoleSerial::getInstance().idleGL();
    WindowsManager::getInstance().idleGL( elapsedTime );
    
    onTop();    

    //Camera_mgr::getInstance().idleGL();
	glutPostRedisplay();
    //Camera_mgr::getInstance().idleGL();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void delete_all_panels(Panel* p)
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
static void quit(void)
{
    //cout <<"exit_handler()"<< endl;
	bStdOut = false;
    logf( (char*)"exit_handler()" );

    Serveur_mgr::getInstance().close_all();
	Camera_mgr::getInstance().stopAllCameras();
	Connexion_mgr::getInstance().stopThread();

    WindowsManager& 		wm 		= WindowsManager::getInstance();
    int 					nbPanel	= wm.getNbPanel();
	std::vector<Panel*>& 	childs 	= wm.getChilds();
	
    logf( (char*)"Fermeture de toute les fenetres" );
	log_tab(true);
    logf( (char*)"Nb Fenetre %d", nbPanel );
    for( int i=nbPanel-1; i>=0; i-- )
    {
    	Panel *	p = childs[i];
	    logf( (char*)"Panel ID %d-%s, %d enfants", p->getID(), p->getExtraString().c_str(), p->getNbPanel() );
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
static void initGL(int argc, char **argv)
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
static void rotateVisible()
{
    Panel*  pCapture = WindowsManager::getInstance().getCapture();

    if ( pCapture == panelHelp)       pCapture->setVisible(!pCapture->getVisible());
    //if ( pCapture == panelControl)    pCapture->setVisible(!pCapture->getVisible());
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void glutKeyboardFuncCtrl(unsigned char key, int x, int y)
{
    WindowsManager&     wm      = WindowsManager::getInstance(); 
    Camera_mgr&         cam_mgr = Camera_mgr::getInstance();
	
	switch(key){ 
	
    // CTRL A
    case 1:
		{
		    //deleteStars();
        }
		break;
	// CTRL D
    case 4:
		{
	    logf( (char*)"Key (ctrl+d) : Efface les etoiles !!" );
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
	// CTRL F
    case 6:
		{
            logf( (char*)"Key (ctrl+f) : FullScreen" );
            Captures::getInstance().fullscreen();
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

    default:
		{
		    cout << "Default..." << endl;
		    logf( (char*)"glutKeyboardFuncCtrl  key=%c", (char)key );
        }
        break;
    }		
    //logf( (char*)"glutKeyboardFuncCtrl  key=%d", (int)key );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void glutKeyboardFuncAlt(unsigned char key, int x, int y)
{
    WindowsManager&     wm      = WindowsManager::getInstance(); 
    Camera_mgr&         cam_mgr = Camera_mgr::getInstance();
	
	switch(key){ 
	
	case 'e':
	    {
	        bAffCatalog = !bAffCatalog;
            VarManager::getInstance().set("bAffCatalog", bAffCatalog);;
	        logf( (char*)"Xref  : %0.2f", (double)Xref );
	        logf( (char*)"Yref  : %0.2f", (double)Yref );
	        logf( (char*)"ZrefX : %0.2f", (double)ZrefX );
	        logf( (char*)"ZrefY : %0.2f", (double)ZrefY );
	        logf( (char*)"Wref  : %0.2f", (double)Wref );
	        logf( (char*)"bAffCataog : %s", BOOL2STR(bAffCatalog) );

            var.set( "bAffCatalog", bAffCatalog );
            
            logf( (char*)"Nb Etoiles %d", vizier.size() );
            //vizier.list();
            /*
            var.set( "Xref",  (double)Xref );
            var.set( "Yref",  (double)Yref );
            var.set( "ZrefX", (double)ZrefX );
            var.set( "ZrefY", (double)ZrefY );
            var.set( "Wref",  (double)Wref );
            */
	        
	    }
	    break;
	
	case 'd':
	    {
	        Wref -= 0.25;
            var.set( "Wref",  (double)Wref );
	        //logf( (char*)"Wref : %0.2f", (double)Wref );
	    }
	    break;
	case 'f':
	    {
	        Wref += 0.25;
            var.set( "Wref",  (double)Wref );
	        //logf( (char*)"Wref : %0.2f", (double)Wref );
	    }
	    break;
	case 'a':
	    {
	        Xref -= 1.0;
            var.set( "Xref",  (double)Xref );
	        //logf( (char*)"Xref : %0.2f", (double)Xref );
	    }
	    break;
	case 'z':
	    {
	        Xref += 1.0;
            var.set( "Xref",  (double)Xref );
	        //logf( (char*)"Xref : %0.2f", (double)Xref );
	    }
	    break;

	case 'q':
	    {
	        Yref -= 1.0;
            var.set( "Yref",  (double)Yref );
	        //logf( (char*)"Yref : %0.2f", (double)Yref );
	    }
	    break;

	case 's':
	    {
	        Yref += 1.0;
            var.set( "Yref",  (double)Yref );
	        //logf( (char*)"Yref : %0.2f", (double)Yref );
	    }
	    break;

	case 'w':
	    {
	        ZrefX -= 1.0;
            var.set( "ZrefX", (double)ZrefX );
	        //logf( (char*)"Zref : %0.2f", (double)Zref );
	    }
	    break;

	case 'x':
	    {
	        ZrefX += 1.0;
            var.set( "ZrefX", (double)ZrefX );
	        //logf( (char*)"Zref : %0.2f", (double)Zref );
	    }
	    break;
	case 'c':
	    {
	        ZrefY -= 1.0;
            var.set( "ZrefY", (double)ZrefY );
	        //logf( (char*)"Zref : %0.2f", (double)Zref );
	    }
	    break;

	case 'v':
	    {
	        ZrefY += 1.0;
            var.set( "ZrefY", (double)ZrefY );
	        //logf( (char*)"Zref : %0.2f", (double)Zref );
	    }
	    break;
    default:
		{
		    cout << "Default..." << endl;
		    logf( (char*)"glutKeyboardFuncAlt  key=%c", (char)key );
        }
        break;
    }		
    //logf( (char*)"glutKeyboardFuncCtrl  key=%d", (int)key );
}
//--------------------------------------------------------------------------------------------------------------------
//
//Symboles de fonction  ASCII Touche CTRL
//	décimal 	hexa. 	clavier 	terme anglais 	terme français
// NULL 0	  	00 	Ctrl+@ 	Null 	Nul
// SOH 	1  		01 	Ctrl+A 	Start of heading 	Début d'entête
// STX 	2  		02 	Ctrl+B 	Start of text 	Début de texte
// ETX 	3  		03 	Ctrl+C 	End of text 	Fin de texte
// EOT 	4  		04 	Ctrl+D 	End of transmit 	Fin de communication
// ENQ 	5  		05 	Ctrl+E 	Enquiry 	Demande
// ACK 	6  		06 	Ctrl+F 	Acknowledge 	Accusé de réception
// BELL 7  		07 	Ctrl+G 	Bell 	Sonnerie
// BS 	8  		08 	Ctrl+H 	Backspace 	Retour arrière
// HT 	9  		09 	Ctrl+I 	Horizontal tab 	Tabulation horizontale
// LF 	10  	0A 	Ctrl+J 	Line feed 	Interligne
// VT 	11  	0B 	Ctrl+K 	Vertical tab 	Tabulation verticale
// FF 	12  	0C 	Ctrl+L 	Form feed 	Page suivante
// CR 	13  	0D 	Ctrl+M 	Carriage return 	Retour en début de ligne
// SO 	14  	0E 	Ctrl+N 	Shitf out 	Hors code
// SI 	15  	0F 	Ctrl+O 	Shift in 	En code
// DLE 	16  	10 	Ctrl+P 	Data line escape 	Echappement en transmission
// DC1 	17  	11 	Ctrl+Q 	Device control 1 	Commande auxiliaire n° 1
// DC2 	18  	12 	Ctrl+R 	Device control 2 	Commande auxiliaire n° 2
// DC3 	19  	13 	Ctrl+S 	Device control 3 	Commande auxiliaire n° 3
// DC4 	20  	14 	Ctrl+T 	Device control 4 	Commande auxiliaire n° 4
// NAK 	21  	15 	Ctrl+U 	Negative acknowledge 	Accusé de réception négatif
// SYN 	22  	16 	Ctrl+V 	Synchronous idle 	Synchronisation
// ETB 	23  	17 	Ctrl+W 	End of transmit block 	Fin de bloc transmis
// CAN 	24  	18 	Ctrl+X 	Cancel 	Annulation
// EM 	25  	19 	Ctrl+Y 	End of medium 	Fin de support
// SUB 	26  	1A 	Ctrl+Z 	Substitute 	Remplacement
// ESC 	27  	1B 	Ctrl+[ 	Escape 	Echappement
// FS 	28  	1C 	Ctrl+\ 	File separator 	Séparateur de fichier
// GS 	29  	1D 	Ctrl+] 	Group separator 	Séparateur de groupe
// RS 	30  	1E 	Ctrl+^ 	Record separator 	Séparateur d'enregistrement
// US 	31  	1F 	Ctrl+_ 	Unit separator 	Séparateur d'unité
// SP 	32  	20 		Space 	Espacement
// DEL 	127  	7F 		Delete 	Effacement
//--------------------------------------------------------------------------------------------------------------------
static void glutKeyboardFunc(unsigned char key, int x, int y) {
    //logf( (char*)"*** glutKeyboardFunc( %d, %d, %d)", (int)key, x, y );
	int modifier = glutGetModifiers();
	
    bFileBrowser = FileBrowser::getInstance().getVisible();
    Camera_mgr&  cam_mgr = Camera_mgr::getInstance();
    
    
        
    
    if (tAlert.size() != 0 )
    {
        if ( bArretUrgence )   
        {
            if (key == 27 )     { 
                arret_urgence();
            }
        }
        bArretUrgence = false;
        alertBoxQuit();
        return;
    }
    else
    if ( PanelConsoleSerial::getInstance().keyboard(key, x, y) )      return;
    else
    if ( bFileBrowser )
    {
        FileBrowser::getInstance().keyboard( key, x, y);
        return;
    }
    else
    if (        cam_mgr.getCurrent() 
            &&  cam_mgr.getCurrent()->getControlVisible()       )
    {
        if ( cam_mgr.getCurrent()->keyboard(key) )      return;
    }
    else
	if (modifier == GLUT_ACTIVE_ALT)
	{
        //logf( (char*)" Touche ALT %c", key );
        glutKeyboardFuncAlt(key,  x,  y);
        return;
	}
    else
	if (modifier == GLUT_ACTIVE_CTRL)
	{
        //logf( (char*)" Touche ALT %c", key );
        glutKeyboardFuncCtrl(key,  x,  y);
        return;
	}
    else
    if ( panelApn && panelApn->keyboard(key, x, y) )      return;

	
	switch(key){ 
	
	// CTRL I
    // touche tab
	case 9:
		//WindowsManager::getInstance().swapVisible();
    	{

            logf( (char*)"Key (TAB) : Image suivante" );
            log_tab(true);
            Captures::getInstance().rotate_capture_plus(false);
            log_tab(true);
        }
		break;
	// CTRL J  LF
	case 10:
		break;
	// CTRL M  CR
	case 13:
        logf( (char*)"Key (CR) : Plein ecran" );
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
    	break;
	// touche escape
	case 27:
	    {
	    logf( (char*)"Key (ESC) : ARRET D'URGENCE !!" );
	    alertBox( "Confirmez l'arret de la monture 'ESC'" );
	    bArretUrgence = true;
	    }
        break;

    case 178:
    	{
            //Captures::getInstance().showIcones();
	        //Captures::getInstance().rotate_capture_plus(true);
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
            Captures::getInstance().rotate_capture_moins(true);
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
        if ( !bModeManuel )
        {
            vec2* pv = Camera_mgr::getInstance().getSuivi();
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
        var.set("vecAD[1].x", vecAD[1].x);
        var.set("vecAD[1].y", vecAD[1].y);
        logf( (char*)"AD[1] (%0.2f,%0.2f)", vecAD[1].x, vecAD[1].y );
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
        if ( !bModeManuel )
        {
            vec2* pv = Camera_mgr::getInstance().getSuivi();
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
        var.set("vecAD[0].x", vecAD[0].x);
        var.set("vecAD[0].y", vecAD[0].y);
        logf( (char*)"AD[0] (%0.2f,%0.2f)", vecAD[0].x, vecAD[0].y );

        char cmd[255];
        sprintf( cmd, "a-1000p" );
        logf( (char*)"Envoi de la commande",  cmd );
        Serial::getInstance().write_string(cmd);
        }
        break;

    case 'b':
        {
            //PanelCourbe::bDebug = !PanelCourbe::bDebug;
		    //logf( (char*)"Key (b) : Bluetooth start" );
		    //BluetoothManager::getInstance().start();
		    Captures::getInstance().afficheInfoFits();
        }
        break;
    
    case 'B':
        {
        	if ( var.existe("bVerboseArduino") )    {
        		bool b = !var.getb("bVerboseArduino");
        		var.set("bVerboseArduino", b);
			    logf( (char*)"bVerboseArduino = %s", BOOL2STR(b) );
        	}
        //logf( (char*)"Key (b) : Bluetooth disconnect" );
        //BluetoothManager::getInstance().disconnect();
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
        if ( !bModeManuel )
        {
            vec2* pv = Camera_mgr::getInstance().getSuivi();
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

        compute_matrix();
        
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
        
        if ( !bModeManuel )
        {
            vec2* pv = Camera_mgr::getInstance().getSuivi();
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

        char cmd[255];
        sprintf( cmd, "d1000p" );
        logf( (char*)"Envoi de la commande",  cmd );
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
        logf( (char*)"Key (f) : File browser affiche une image" );
        bFileBrowser = FileBrowser::getInstance().getVisible();
        FileBrowser::getInstance().setFiltre( "" );
        if ( var.existe("DirFileBrowser") )
        {
            workDirFileBrowser = *var.gets( "DirFileBrowser" );
        }

        FileBrowser::getInstance().change_dir( workDirFileBrowser );
        bFileBrowser = !bFileBrowser;
        FileBrowser::getInstance().setCallBack( &cb_file_browser );
        
        if ( bFileBrowser )         FileBrowser::getInstance().affiche();
        else                        FileBrowser::getInstance().cache();
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
        logf( (char*)"Key (H) : Choisir le repertoire pour enregistre" );
        Camera* pCam = Camera_mgr::getInstance().getCurrent();
        if ( pCam != NULL )
        {
            FileBrowser::getInstance().setCallBack( pCam );
            FileBrowser::getInstance().setExtra( 0 );
            if ( var.existe("DirSurveillance") )    workDirCaptures = *var.gets( "DirSurveillance" );
            FileBrowser::getInstance().change_dir( workDirCaptures );
            FileBrowser::getInstance().setNewline(true);
            FileBrowser::getInstance().affiche();
            //p->enregistre();
        }
        }
        break;

    case 'h':
        {
        logf( (char*)"Key (h) : Enregistre uneimage" );
        Camera* p = Camera_mgr::getInstance().getCurrent();
        if ( p!=NULL )
        {
            p->enregistre();
        }
        }
        break;

    case 'i':
        {
            if ( panelApn == NULL )         panelApn = new PanelApn();
            else                            panelApn->setVisible( !panelApn->getVisible() );
            
            //if ( panelApn->getVisible() )   panelApn->commande_photo();
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
            thread( &commande_stellarium).detach();
        }
		break;
	case 'j':
		{
			var.set("bAffFitsCorrection", !var.getb("bAffFitsCorrection") );
			Capture* p = Captures::getInstance().getCurrentCapture();
			if ( p && !p->isIconized() )	{
				p->onTop();
			}
            //thread( &commande_asi_studio).detach();
        }
		break;

    case 'K':
        {
            //*
            char cmd[] = "C";
            Serial::getInstance().write_string(cmd);
            //*/
            /*
            char cmd[255];
            int ad, dc;

            ad = 1000;
            sprintf( cmd, "a%dp;", ad );
            Serial::getInstance().write_string(cmd, true);

            dc = 1000;
            sprintf( cmd, "d%dp", dc );
            Serial::getInstance().push_cmd(cmd);

            ad = -1000;
            sprintf( cmd, "a%dp;", ad );
            Serial::getInstance().push_cmd(cmd);

            dc = -1000;
            sprintf( cmd, "d%dp", dc );
            Serial::getInstance().push_cmd(cmd);

            ad = -500; dc = -500;
            sprintf( cmd, "a%dp;d%dp", ad, dc );
            Serial::getInstance().push_cmd(cmd);

            ad = 500; dc = 500;
            sprintf( cmd, "a%dp;d%dp", ad, dc );
            Serial::getInstance().push_cmd(cmd);
            */
        }
        break;
    case 'k':
        {
        bSound = !bSound;
        logf( (char*)"Key (k) : Active/desactive le son  %s ", BOOL2STR(bSound) );
        var.set( "bSound", bSound );
        }
        break;
    case 'l':
        {            
        logf( (char*)"Key (l) : Affiche les connexions" );
        Connexion_mgr::getInstance().print_list();
        Camera_mgr::getInstance().print_list();
        Captures::getInstance().print_list();
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
        if ( !bModeManuel )	{
        	bModeManuel = true;
        }


	    logf( (char*)"Key (m) : Autorisation du deplcament par la souris (bouton du milieu) ...");
	    bMouseDeplace = !bMouseDeplace;
	    logf( (char*)"  bMouseDeplace = %s", BOOL2STR(bMouseDeplace) );

	    if ( bMouseDeplace )			bMouseDeplaceVers = false;

        }
        break;

    case 'n':
        {
	        logf( (char*)"Key (n) :Requete GAIA");
        	Capture*  p = Captures::getInstance().getCurrentCapture();
        	if ( p && p->isFits() )	{
        		p->getPreview()->findGaiaDR3();
        	}
        }
        break;

    case 'N':
        {
        logf( (char*)"Key (N) : Mode NUIT");
        bNuit = !bNuit;
        logf( (char*)"  bNuit = %s", BOOL2STR(bNuit) );
        var.set("bNuit", bNuit);

        setColor();
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
                //bOneFrame = true;
            }
            else
            {
                //*
                Camera_mgr::getInstance().sup( pPleiade );
                bPleiade = false;
                pPleiade = NULL;
                //delete pPleiade;
                //*/
            }
        }
    	break;

    case 'O':
        {
        logf( (char*)"Key (O) : Mode manuel (%s)", BOOL2STR(bModeManuel) );
        set_mode(!bModeManuel);
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

    case 'q':
        {
        logf( (char*)"Key (q) : Lance un script python");
        //Py_SetProgramName(argv[0]);  /* optional but recommended */
        vizier_load_stars(string(""), 56.0, 24.0);

        /*
	    char filename[] = "pyemb7.py";
	    FILE* fp;

	    Py_Initialize();

	    fp = _Py_fopen(filename, "r");
	    PyRun_SimpleFile(fp, filename);

	    Py_Finalize();
        
        
        
        Py_Initialize();
        PyRun_SimpleString("from time import time,ctime\n"
                         "print( 'Hello Word' )\n");
        Py_Finalize();
        */
        }
        // ./find_gaia_dr2.py -r 200 ngc4535
        break;

    case 'Q':
        {
            thread( &commande_polaris).detach();
        }
        break;
    case 'r' :
        {
            logf( (char*)"Key (Q) : Ouvre un fichier de suivi (.guid)");
            FileBrowser& fb = FileBrowser::getInstance();
            
            fb.setCallBack(&cb_cguidage);
            fb.setFiltre( ".guid" );
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
    
                //Captures::getInstance().deleteAllStars();
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
        logf( (char*)"Key (S) : Suivi on/off" );
            bSuivi = !bSuivi;
            var.set( "bSuivi", bSuivi );
            change_joy( xSuivi, ySuivi );
            logf( (char*)"  bSuivi (%s)", BOOL2STR(bSuivi) );
        }
        break;

	case 't':
		{
    		fTimeCorrection *= 0.9f;
    		fTimeCpt = 0.0;
            logf( (char*)"Augmente de temps : %0.2f", fTimeCorrection );
        }
		break;
	case 'T':
		{
    		fTimeCorrection /= 0.9f;
    		fTimeCpt = 0.0;
            logf( (char*)"Diminue de temps : %0.2f", fTimeCorrection );
        }
		break;

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

                xSuivi = x;
                ySuivi = y;

                var.set("xSuivi", xSuivi );
                var.set("ySuivi", ySuivi );

                panelCourbe->get_vOrigine().x = x;
                panelCourbe->get_vOrigine().y = y;
                panelCourbe->get_vOrigine().z = 0.0;
                
                change_joy( x, y );

                logf( (char*)"initialise vOrigine(click) : (%d,%d)", x, y);
            }
            else
            {
                vec2* pv = Camera_mgr::getInstance().getSuivi();
                if ( pv != NULL )
                {
                    xSuivi = pv->x;
                    ySuivi = pv->y;

                    change_joy( xSuivi, ySuivi );

                    var.set("xSuivi", xSuivi );
                    var.set("ySuivi", ySuivi );
                }
                panelCourbe->get_vOrigine().x = xSuivi;
                panelCourbe->get_vOrigine().y = ySuivi;
                panelCourbe->get_vOrigine().z = 0.0;
                
                
                logf( (char*)"initialise vOrigine(suivi) : (%0.2f,%0.2f)", xSuivi, ySuivi);
            }
            bCentrageSuivi = false;
        }
        break;
    case 'w' :
        {
            //BluetoothManager::getInstance().centre_joystick();
            bCentrageSuivi = !bCentrageSuivi;
            WindowsManager& wm = WindowsManager::getInstance();
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
            ++iDisplayfft %= 4;

            if ( iDisplayfft&0x1 )          bDisplayfftX = true;        
            else                            bDisplayfftX = false;
            if ( iDisplayfft&0x2 )          bDisplayfftY = true;
            else                            bDisplayfftY = false;

            var.set("bDisplayfftX", bDisplayfftX);
            var.set("bDisplayfftY", bDisplayfftY);

            logf( (char*)"Key (x) : Affiche/Cache la FFT (%d)", iDisplayfft );
        }
        break;

    case 'X':
        {
            ++iDisplayCourbe %= 4;

            if ( iDisplayCourbe&0x1 )       bDisplayCourbeX = true;        
            else                            bDisplayCourbeX = false;
            if ( iDisplayCourbe&0x2 )       bDisplayCourbeY = true;
            else                            bDisplayCourbeY = false;

            var.set("bDisplayCourbeX", bDisplayCourbeX);
            var.set("bDisplayCourbeY", bDisplayCourbeY);

            logf( (char*)"Key (x) : Affiche/Cache la courbe (%d)", iDisplayfft );
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
        bCorrection = !bCorrection; 
        logf( (char*)"Key (Y) : Asservissement de monture %s", BOOL2STR(bCorrection) );
        set_asservissement();
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
        if ( key != 14 )
            logf((char*)"key: %d", key);
        }
        break;
	}
	
	//Camera_mgr::getInstance().keyboard( key );
    onTop();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void glutKeyboardUpFunc(unsigned char key, int x, int y)	{
	WindowsManager::getInstance().keyboardUpFunc( key, x, y);
    //WindowsManager::getInstance().onBottom(panelPreView);
    onTop();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void glutSpecialFunc(int key, int x, int y)	{

    Captures::getInstance().glutSpecialFunc(key, x, y);
    
    
    switch( key)
    {
    case GLUT_KEY_F1:
        {
        bPanelHelp = !bPanelHelp;
        var.set("bPanelHelp", bPanelHelp);
        panelHelp->setVisible(bPanelHelp);
        if ( bPanelHelp )       WindowsManager::getInstance().onTop(panelHelp);
        log( (char*)"Toggle panelHelp !!!" );
        }
        break;
	case GLUT_KEY_F2:
        {
        Camera_mgr::getInstance().togglePanel();
        log( (char*)"Toggle panelCamera !!!" );
        }
        break;
    case GLUT_KEY_F3:
        {
        bPanelResultat = !bPanelResultat;
        var.set("bPanelResultat", bPanelResultat);
        panelResultat->setVisible(bPanelResultat);
        if ( bPanelResultat )       WindowsManager::getInstance().onTop(panelResultat);
        logf( (char*)"Toggle panelResultat !!! %d", (int)bPanelResultat );
        }
        break;
    case GLUT_KEY_F4:
        {
        bPanelCourbe = !bPanelCourbe;
        var.set("bPanelCourbe", bPanelCourbe);
        panelCourbe->setVisible(bPanelCourbe);
        //( pButtonCourbe,  bPanelCourbe,       "courbe" );
        set_courbe();
        
        if ( bPanelCourbe )       WindowsManager::getInstance().onTop(panelCourbe);
        log( (char*)"Toggle panelCourbe !!!" );
        }
        break;
    case GLUT_KEY_F5:
        {
        bPanelStdOut = !bPanelStdOut;
        var.set("bPanelStdOut", bPanelStdOut);
        panelStdOut->setVisible(bPanelStdOut);
        if ( bPanelStdOut )       WindowsManager::getInstance().onTop(panelStdOut);
        //log( (char*)"Toggle panelStdOut !!!" );
        }
        break;
    case GLUT_KEY_F6:
        {
        bPanelSerial = !bPanelSerial;
        var.set("bPanelSerial", bPanelSerial);
        PanelConsoleSerial::getInstance().setVisible(bPanelSerial);
        logf( (char*)"Toggle serial (%s)", BOOL2STR(bPanelSerial) );
        }
        break;
    case GLUT_KEY_F7:
        {
        logf( (char*)"Key (n) : Affiche/cache les capture");
        bAffIconeCapture = !bAffIconeCapture;
        logf( (char*)"  bAffIconeCapture = %s", BOOL2STR(bAffIconeCapture) );
        Captures::getInstance().switchAffIcones();
        }
        break;
    case GLUT_KEY_F8:
        {
            if ( panelApn == NULL )         panelApn = new PanelApn();
            else
                panelApn->setVisible( !panelApn->getVisible() );
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
        Captures::getInstance().ajoute();
        }
        break;
    case GLUT_KEY_F12:
        {
        Captures::getInstance().supprime();
        }
        break;
        
    case GLUT_KEY_SHIFT_L:
    case GLUT_KEY_SHIFT_R:
    case GLUT_KEY_CTRL_L:
    case GLUT_KEY_CTRL_R:
    case GLUT_KEY_ALT_L:
    case GLUT_KEY_ALT_R:
        break;
    default:	
	    {
        //logf( (char*)"glutSpecialFunc %d", key );
        logf( (char*)"glutSpecialFunc (%d) \'%c\'", (int)key, key );

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
static void glutSpecialUpFunc(int key, int x, int y)	{
	WindowsManager::getInstance().keyboardSpecialUpFunc( key, x, y);
    //WindowsManager::getInstance().onBottom(panelPreView);
    onTop();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void glutMouseFunc(int button, int state, int x, int y)	{
    //logf( (char*)"main::glutMouseFunc()");
    log_tab(true);
    
    mouse.x = x;
    mouse.y = y;

    WindowsManager& wm  = WindowsManager::getInstance();
    Camera_mgr&     mgr = Camera_mgr::getInstance(); 

	wm.mouseFunc(button, state, x, y);
    //WindowsManager::getInstance().onBottom(panelPreView);
    PanelWindow*    pPreviewCam = NULL;
    Panel *         pCapture = NULL;
    Panel*          pMouseOver = NULL;

    if ( panelStatus->isMouseOver(x, y) )
    {
        //logf( (char*)"Souris sur la barre de status" );
        goto endglutMouseFunc;
        return;
    }
    
    if ( mgr.getCurrent() != NULL  &&  mgr.getCurrent()->getPanelPreview() != NULL )
    {
        pPreviewCam = mgr.getCurrent()->getPanelPreview();
        pCapture    = wm.getCapture();
        pMouseOver  = wm.findPanelMouseOver(x, y);
    }
    
	if ( bMouseDeplace && button == GLUT_MIDDLE_BUTTON && state == 0 )
	{
		bMouseDeplaceVers = true;
        getSuiviParameter();

        mgr.onBottom();
        
	    int X = x;
	    int Y = y;
	    
	    mgr.screen2tex(X,Y);
	    
	    //xClick = X;
	    //yClick = Y;

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

        mgr.onBottom();
        
	    int X = x;
	    int Y = y;
	    
	    mgr.screen2tex(X,Y);
	    
	    xClick = X;
	    yClick = Y;

        vDepl[1].x = xClick;
        vDepl[1].y = yClick;
        vDepl[1].z = 0.0;
        logf( (char*)"vDepl[1](%0.2f, %0.2f)",  vDepl[1].x, vDepl[1].y );

	    logf( (char*)"state = 1" );

        vec3 v = vDepl[1] - vDepl[0];
	    logf( (char*)"  delta (%0.2f, %0.2f)", v.x, v.y );
	    
        vTr = mChange * v;
        int ad = (int) (vTr.x * -1000.0);
        int dc = (int) (vTr.y * 1000.0);
        char cmd[255];
        sprintf( cmd, "a%dp;d%dp", ad, dc );
        logf( (char*)"Envoi de la commande : \"%s\"",  cmd );

        Serial::getInstance().write_string(cmd);
	}

	//if ( bPause && button == 0 && state == 0 )	{
    if ( bModeManuel && button == GLUT_LEFT_BUTTON && state == 0 )	{
        getSuiviParameter();

        mgr.onBottom();
        
	    int X = x;
	    int Y = y;
	    
	    mgr.screen2tex(X,Y);
	    
	    xClick = X;
	    yClick = Y;

	    logf( (char*)"Click Down Click(%d, %d)  vCamera (%dx%d)" , xClick, yClick, mgr.getRB()->w.load(), mgr.getRB()->h.load() );
	    
	} 
    else
	if ( !bModeManuel && button == 0 && state == 0 && pMouseOver == pPreviewCam )	{

	    logf( (char*)"Suivi=(%0.2f,%0.2f)" , xSuivi, ySuivi );
	    //logf( (char*)"    l=%d rgb=%d,%d,%d" , r,g,b,  l );

	} 

    Camera_mgr::getInstance().onBottom();
    onTop();

endglutMouseFunc:
    log_tab(false);
    //logf( (char*)"main::glutMouseFunc()");
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void glutMotionFunc(int x, int y)	{	
    mouse.x = x;
    mouse.y = y;
	WindowsManager::getInstance().motionFunc(x, y);
    //WindowsManager::getInstance().onBottom(panelPreView);
    onTop();

	//if ( bMouseDeplace  )        logf( (char*)" motion func (%d, %d)", x, y );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void glutPassiveMotionFunc(int x, int y)	{
    mouse.x = x;
    mouse.y = y;
	WindowsManager::getInstance().passiveMotionFunc(x, y);
    //WindowsManager::getInstance().onBottom(panelPreView);
    onTop();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void setColor()	
{
    logf( (char*)"main::setColor()" );

    unsigned long color;
    if (bNuit)                  color = 0xFFFF0000;
    else                        color = COLOR_WHITE;

    PanelConsoleSerial::getInstance().getConsole()->setColor(color);

    panelStdOut->setColor(color);
    panelHelp->setColor(color);
    panelCourbe->setColor(color);
    panelStatus->setColor(color);
    panelResultat->setColor(color);
    Captures::getInstance().setColor(color);
    Camera_mgr::getInstance().setColor(color);
    FileBrowser::getInstance().setColor(color);
    PanelConsoleSerial::getInstance().setColor(color);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void onTop()	
{
    WindowsManager& wm = WindowsManager::getInstance();
    Captures::getInstance().onTop();
    wm.onTop(panelStdOut);
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

    if ( var.existe("xPanelHelp") )         x  = var.geti( "xPanelHelp");
    if ( var.existe("yPanelHelp") )         y  = var.geti( "yPanelHelp");

    panelHelp->setPos( x,  y );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void resizeCourbe(int width, int height)	{
    int dx = width - 20;
    int dy = 600;
    int x = 10;
    int y = height - 10 - 20 - dy;

    if ( var.existe("xPanelCourbe") )         x  = var.geti( "xPanelCourbe");
    if ( var.existe("yPanelCourbe") )         y  = var.geti( "yPanelCourbe");
    if ( var.existe("dxPanelCourbe") )        dx = var.geti("dxPanelCourbe");
    if ( var.existe("dyPanelCourbe") )        dy = var.geti("dyPanelCourbe");


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
static void CreateCourbe()	{
    panelCourbe = new PanelCourbe();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void CreateResultat()	{
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

	pRef   = new PanelText( (char*)"Reference ",  PanelText::NORMAL_FONT, x0, y0+16*l++  );
	pEtoile = new PanelText( (char*)"Etoile ",      PanelText::NORMAL_FONT, x0, y0+16*l++  );
	pEcart = new PanelText( (char*)"Ecart ",      PanelText::NORMAL_FONT, x0, y0+16*l++  );
	pRef->setTabSize(80);
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
static void addString( string s )
{
    if ( s.size() == 0 )       { y_help += 15; return; }
    PanelText* p = new PanelText( (char*)s.c_str(),  		PanelText::NORMAL_FONT, x_help, y_help );
    //PanelText* p = new PanelText( (char*)s.c_str(),  		PanelText::UBUNTU_B, x_help, y_help );
    p->setTabSize(40);
	panelHelp->add( p );
	y_help += 15;
}	
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void addString2( string s )
{
    if ( s.size() == 0 )       { y_help += 15; return; }
    PanelText* p = new PanelText( (char*)s.c_str(),  		PanelText::NORMAL_FONT, x_help+400, y_help );
    p->setTabSize(40);
	panelHelp->add( p );
	y_help += 15;
}	
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void CreateHelp()	
{
    int X = width - 50;
    int Y = 50;
    int DX = 400;
    int DY = 700;
    
    panelHelp = new PanelWindow();
	panelHelp->setExtraString( (char*)"PanelHelp");
    panelHelp ->setDisplayGL(displayGLnuit_cb);
	panelHelp->setPosAndSize( X, Y, DX, DY);
	panelHelp->setVisible(bPanelHelp);
	//panelHelp->setBackground( (char*)"/home/rene/programmes/opengl/video/images/background.tga");
	panelHelp->setBackground( (char*)"images/background.tga");
    
	int y = 10;
	int dy = 15;
	int l=50;

    //----------------------------------------------------------------------------------------------------------------
	for( int i=0; i<t_sHelp1.size(); i++ )
	{
    	addString( t_sHelp1[i] );
	}
    //----------------------------------------------------------------------------------------------------------------
    y_help = 0;
	for( int i=0; i<t_sHelp2.size(); i++ )
	{
    	addString2( t_sHelp2[i] );
	}
    //----------------------------------------------------------------------------------------------------------------

    
	WindowsManager::getInstance().add(panelHelp);
	
	DY = ++l*dy;
	
    if ( var.existe("xPanelHelp") )         X  = var.geti( "xPanelHelp");
    if ( var.existe("yPanelHelp") )         Y  = var.geti( "yPanelHelp");
    if ( var.existe("dxPanelHelp") )        DX = var.geti("dxPanelHelp");
    if ( var.existe("dyPanelHelp") )        DY = var.geti("dyPanelHelp");

    panelHelp->setSize(DX, DY);
    //panelHelp->loadSkin((char*)"fen-2");
    resizeHelp(width, height);

    //X = width - 20 - DX;
    //Y = 20 + 20 ;
	panelHelp->setPos(X, Y);


    logf((char*)"** CreateHelp()  panelHelp  %d,%d %dx%d", X, Y, DX, DY);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#include "status.cpp"

static void CreateStatus()	{
	WindowsManager& wm = WindowsManager::getInstance();
	wm.setScreenSize( width, height );

	int x=0;
	int dx=width;
	int dy=20;
	int y=height-dy;
	char s[255];

	panelStatus = new PanelSimple();
	panelStatus->setExtraString( (char*)"PanelStatus");
	panelStatus->setPosAndSize( x, y, dx, dy );

	//pStatus = new PanelText( (char*)"",		PanelText::NORMAL_FONT, 10, 2 );
	//panelStatus->add( pStatus );
    //if ( bPause )   pStatus->changeText((char*)"Pause" );
    //else            pStatus->changeText((char*)"-----" );

    logf((char*)"** CreateStatus()  panelSatuts  %d", width);
    pFPS = new PanelText( (char*)"0",		            PanelText::NORMAL_FONT, width-100, 2 );
	panelStatus->add( pFPS );

    pHertz = new PanelText( (char*)"0",		            PanelText::NORMAL_FONT, width-150, 2 );
	panelStatus->add( pHertz );

    pArduino = new PanelText( (char*)"Arduino",		    PanelText::NORMAL_FONT, width-280, 2 );
    pArduino->setColor(COLOR_GREY);
	panelStatus->add( pArduino );

    pCoordSuivi = new PanelText( (char*)"(---, ---)",   PanelText::NORMAL_FONT, width-560, 2 );
	panelStatus->add( pCoordSuivi );

    pSuivi = new PanelText( (char*)"Suivi",		        PanelText::NORMAL_FONT, width-440, 2 );
	panelStatus->add( pSuivi );

    pStellarium = new PanelText( (char*)"Stellarium",   PanelText::NORMAL_FONT, width-230, 2 );
    pStellarium->setColor(COLOR_GREY);
	panelStatus->add( pStellarium );

    pPas = new PanelText( (char*)"pas:",		        PanelText::NORMAL_FONT, width-370, 2 );
    pPas->setColor(COLOR_GREY);
	panelStatus->add( pPas );

    pDeplacement = new PanelText( (char*)"Depl",		PanelText::NORMAL_FONT, width-405, 2 );
    pDeplacement->setColor(COLOR_GREY);
	panelStatus->add( pDeplacement );

    pAD = new PanelText( (char*)"AD:",		            PanelText::NORMAL_FONT, 60, 2 );
    change_ad_status( fpos_ad );
	panelStatus->add( pAD );
    
    pDC = new PanelText( (char*)"DC:",		            PanelText::NORMAL_FONT, 200, 2 );
    change_dc_status( fpos_dc );
	panelStatus->add( pDC );

    pAsservi = new PanelText( (char*)"GUID",		    PanelText::NORMAL_FONT, 705, 2 );
	panelStatus->add( pAsservi );




	string sErr = "Status !!!";

 	wm.add( panelStatus );
 	panelStatus->setBackground((char*)"images/background.tga");
 	
 	create_windows_button();


    logf((char*)"** CreateStatus()  panelSatuts  %d,%d %dx%d", x, y, dx, dy);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void CreateStdOut()	{
	panelStdOut = new PanelStdOut();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void CreateAllWindows()	{
    //CreatePreview();
    //CreateControl();
    CreateHelp();
    CreateResultat();
    CreateStatus();
    CreateCourbe();
    CreateStdOut();
    panelApn = NULL;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void changeDec( bool b )
{
    pButtonDec->setVal( !b );
    if ( b != bDec )
    {
        logf( (char*)"Declinaison : %d", (int)b );
        //inverse_texture( pButtonDec, b, "dec" );
        pButtonDec->setVal( !b );
        bDec = b;
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void changeAsc( bool b )
{
    pButtonAsc->setVal( !b );
    if ( b != bAsc )
    {
        logf( (char*)"Asc Droite : %d", (int)b );
        //inverse_texture( pButtonAsc, b, "asc" );
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
        //inverse_texture( pButtonAsc, b, "asc" );
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
        //inverse_texture( pButtonAsc, b, "asc" );
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
void log( char* chaine )
{
    string aff = sTab + string(chaine);
    if ( panelStdOut && bStdOut )          panelStdOut->affiche( (char*)aff.c_str() );
    
    printf( "log : %s\n", aff.c_str() );
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
void parse_size( string s )
{
    std::size_t found = s.find("x");
    int l = s.size();
    
    if (found!=std::string::npos)
    {
        std::cout << "Period found at: " << found << '\n';

        char buff[100];
        sprintf( buff, "%s", s.c_str() );
        buff[found] = 0;
        buff[l] = 0;
        
        char *sWidth = buff;
        char *sHeight = buff + found +1;
        
        width  = atoi(sWidth);
        height = atoi(sHeight);
        
        std::cout<<"Parse "<< sWidth <<"x"<< sHeight << '\n';
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void parse_option( int argc, char**argv )
{
    bool bOK = false;
    /*
    for(int i=0; i<20; i++ )    {
        if (camera.isDevice(i))    {
            cout <<"Camera sur /dev/video"<< i << endl;
            bOK = true;
            break;
        }
    }
    */
    
    //if (!bOK)           cout <<"Pas de camera !!!" << endl;

    char        cSize[100];

    for (;;) {
            int idx;
            int c;

            c = getopt_long(argc, argv, short_options, long_options, &idx);

            if (-1 == c)
                    break;

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
                    exclude.push_back( dev_name );
                    break;

            case 'h':
                    usage(stdout, argc, argv);
                    exit(EXIT_SUCCESS);

            case 'f':
                    bFull = true;
                    width  = widthScreen;
                    height = heightScreen;
                    break;

            case 's':
                    strncpy( cSize, optarg, sizeof(cSize) );

                    //string      sSize=string((const char*)cSize);
                    printf( "%s\n", cSize );
                    parse_size(cSize);
                    break;
            case 'p':
                {
                pPleiade = new Pleiade();
                //bOneFrame = true;
                Camera_mgr::getInstance().add( pPleiade );
                bPleiade = true;
                }
                break;
    
            /*
            case 'm':
                    io = IO_METHOD_MMAP;
                    break;

            case 'r':
                    io = IO_METHOD_READ;
                    break;

            case 'u':
                    io = IO_METHOD_USERPTR;
                    break;

            case 'o':
                    out_buf++;
                    break;


            case 'c':
                    errno = 0;
                    frame_count = strtol(optarg, NULL, 0);
                    if (errno)
                            errno_exit(optarg);
                    break;
            */
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
    Display *display;
    Screen *screen;

    // open a display
    display = XOpenDisplay(NULL);

    // return the number of available screens
    int count_screens = ScreenCount(display);

    printf("Total count screens: %d\n", count_screens);


    for (int i = 0; i < count_screens; ++i) {
        screen = ScreenOfDisplay(display, i);
        printf("Screen %d: %dX%d\n", i + 1, screen->width, screen->height);
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

    cout << "Nb crtc   : " << scr->ncrtc << endl;         
    cout << "Nb Output : " << scr->noutput << endl;         
    cout << "N  mode   : " << scr->nmode << endl;    
    
    for( int i=0; i<scr->ncrtc; i++ )
    {
        crtc_info = XRRGetCrtcInfo (dpy, scr, scr->crtcs[i]);
        cout << i <<" - " << crtc_info->width <<"x"<< crtc_info->height << endl;
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
void init_var()
{
    //VarManager& var = VarManager::getInstance();

    var.set( "bModeManuel", bModeManuel );
    var.set( "bSuivi", bSuivi );
    var.set( "bPanelControl", bPanelControl );
    var.set( "bPanelHelp", bPanelHelp );
    var.set( "bPanelResultat", bPanelResultat );
    var.set( "bPanelCourbe", bPanelCourbe );
    var.set( "bPanelStdOut", bPanelStdOut );
    var.set( "bPanelSerial", bPanelSerial );
    var.set( "bAfficheVec", bAfficheVec);
    var.set( "bCorrection", bCorrection);

    var.set( "err", panelCourbe->get_err() );

    var.set("bPause", bPause);
    var.set("bFull", bFull);
    var.set("bNuit", bNuit);

    var.set("courbe1", panelCourbe->get_courbe1());
    var.set("delta_courbe1", panelCourbe->get_delta_courbe1());
    var.set("courbe2", panelCourbe->get_courbe2());
    var.set("delta_courbe2", panelCourbe->get_delta_courbe2());

    var.set("vecAD[0].x", vecAD[0].x);
    var.set("vecAD[0].y", vecAD[0].y);
    var.set("vecAD[1].x", vecAD[1].x);
    var.set("vecAD[1].y", vecAD[1].y);

    var.set("vecDC[0].x", vecDC[0].x);
    var.set("vecDC[0].y", vecDC[0].y);
    var.set("vecDC[1].x", vecDC[1].x);
    var.set("vecDC[1].y", vecDC[1].y);

    var.set("vOrigine.x", panelCourbe->get_vOrigine().x);
    var.set("vOrigine.y", panelCourbe->get_vOrigine().y);

    var.set("xSuivi", xSuivi);
    var.set("ySuivi", ySuivi);

    if ( !var.existe("fLimitCorrection") )      var.set( "fLimitCorrection", (double)80.0);
    
    /*
    var.set("xPanelStdOut",  panelStdOut->getX() );
    var.set("yPanelStdOut",  panelStdOut->getY() );
    var.set("dxPanelStdOut", panelStdOut->getDX() );
    var.set("dyPanelStdOut", panelStdOut->getDY() );
    */
    //if ( panelStdOut->getX() == 0 )    bAlert = true;

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void charge_var()
{
    var.charge();

    if ( var.existe("bModeManuel") )        bModeManuel         = var.getb( "bModeManuel" );
    //bSuivi              = var.getb( "bSuivi" );

    bPanelCourbe        = var.getb( "bPanelCourbe" );
    if ( panelCourbe )      panelCourbe->setVisible(bPanelCourbe);
    
    bPanelHelp          = var.getb( "bPanelHelp" );
    if (panelHelp)      panelHelp->setVisible( bPanelHelp );
    
    bPanelResultat      = var.getb( "bPanelResultat" );
    if (panelResultat)  panelResultat->setVisible( bPanelResultat );
    
    bPanelStdOut        = var.getb( "bPanelStdOut" );
    if (panelStdOut)   panelStdOut->setVisible( bPanelStdOut );
    
    
    bPanelSerial        = var.getb( "bPanelSerial" );
    PanelConsoleSerial::getInstance().setVisible( bPanelSerial );
    
    bAfficheVec         = var.getb("bAfficheVec");

    bPause              = var.getb("bPause");
    bFull               = var.getb("bFull");
    if ( bFull )        glutFullScreen();

    bCorrection         = var.getb("bCorrection");
    bNuit               = var.getb("bNuit");

    if ( panelCourbe == NULL )          logf( (char*)"[ERREUR] Erreur panelCourbe NULL ..." );
    /*
    panelCourbe->set_err( var.getf("err") );
    //update_err();
    
    panelCourbe->set_courbe1( var.getf("courbe1"));
    panelCourbe->set_delta_courbe1( var.getf("delta_courbe1"));
    panelCourbe->set_courbe2( var.getf("courbe2"));
    panelCourbe->set_delta_courbe2( var.getf("delta_courbe2"));

    panelCourbe->get_vOrigine().x          = var.getf("vOrigine.x");
    panelCourbe->get_vOrigine().y          = var.getf("vOrigine.y");
    panelCourbe->get_vOrigine().z          = 0.0;
    */
    vecAD[0].x          = var.getf("vecAD[0].x");
    vecAD[0].y          = var.getf("vecAD[0].y");
    vecAD[0].z          = 0.0;

    vecAD[1].x          = var.getf("vecAD[1].x");
    vecAD[1].y          = var.getf("vecAD[1].y");
    vecAD[1].z          = 0.0;

    vecDC[0].x          = var.getf("vecDC[0].x");
    vecDC[0].y          = var.getf("vecDC[0].y");
    vecDC[0].z          = 0.0;

    vecDC[1].x          = var.getf("vecDC[1].x");
    vecDC[1].y          = var.getf("vecDC[1].y");
    vecDC[1].z          = 0.0;
    
    compute_matrix();
    logf( (char*)"vecAD[0] (%02f,%0.2f)", vecAD[0].x, vecAD[0].y );
    logf( (char*)"vecAD[1] (%02f,%0.2f)", vecAD[1].x, vecAD[1].y );
    logf( (char*)"vecDC[0] (%02f,%0.2f)", vecDC[0].x, vecDC[0].y );
    logf( (char*)"vecDC[1] (%02f,%0.2f)", vecDC[1].x, vecDC[1].y );

    logf( (char*)"MATRIX %02f", mChange.mat[0] );


    xSuivi              = var.getf("xSuivi");
    ySuivi              = var.getf("ySuivi");
    
    //xSuivi              = panelCourbe->get_vOrigine().x;
    //ySuivi              = panelCourbe->get_vOrigine().y;

	Camera_mgr&  cam_mgr = Camera_mgr::getInstance();
	cam_mgr.active();

    if ( var.existe("bSimu") )              bSimu           = var.getb("bSimu");
    if ( var.existe("bAffSuivi") )          bAffSuivi       = var.getb("bAffSuivi");
    if ( var.existe("bAffCentre") )         bAffCentre      = var.getb("bAffCentre");
    if ( var.existe("bSound") )             bSound          = var.getb("bSound");
    if ( var.existe("bInverseCouleur"))     bInverseCouleur = var.getb("bInverseCouleur");
    if ( var.existe("fLimitCorrection"))    fLimitCorrection = var.getf("fLimitCorrection");
    if ( var.existe("filtre"))              filtre          = var.geti("filtre");
    if ( var.existe("fpos_ad"))             fpos_ad         = var.getf("fpos_ad");
    if ( var.existe("fpos_dc"))             fpos_dc         = var.getf("fpos_dc");
    if ( var.existe("Xref"))                Xref            = var.getf("Xref");
    if ( var.existe("Yref"))                Yref            = var.getf("Yref");
    if ( var.existe("ZrefX"))               ZrefX           = var.getf("ZrefX");
    if ( var.existe("ZrefY"))               ZrefY           = var.getf("ZrefY");
    if ( var.existe("Wref"))                Wref            = var.getf("Wref");
    if ( var.existe("bAffCatalog"))         bAffCatalog     = var.getb("bAffCatalog");
   	if ( !var.existe("bVerboseArduino") )   var.set("bVerboseArduino", false);
    if ( !var.existe("bAffFitsCorrection")) var.set("bAffFitsCorrection", true );
   
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void exit_handler()
{
    //BluetoothManager::getInstance().disconnect();
    Serveur_mgr::getInstance().close_all();
    cout <<"exit_handler()"<< endl;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
    //var.setSauve();
    //init_var();
    atexit(exit_handler);
    //parse_option_size(argc, argv);
    
    //vCameraSize.x = 1280;
    //vCameraSize.y = 720;

    //vCameraSize.x = 1920;
    //vCameraSize.y = 1080;
    
    xCam = 0;
    yCam = 0;
    dxCam = 1920;
    dyCam = 1080;

    getX11Screen();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

    //width = getScreenDX() - 100;
    //height = getScreenDY() - 40;
    width = getScreenDX();
    height = getScreenDY();

    
    cout <<"Screen size "<< width <<"x"<< height << endl;

	//xPos = 1200 + (getScreenDX()-width)/2;
	xPos = 0.0;//(getScreenDX()-width)/2;
	yPos = 0.0;//(getScreenDY()-height)/2;

	glutInitWindowPosition(xPos, yPos);
	glutInitWindowSize( width, height );

	if (glutCreateWindow("Astro Pilot") == 0){ 
		return 1;
	}

	//glutFullScreen();

	glutReshapeFunc(reshapeGL);
	glutDisplayFunc(displayGL);
	glutIdleFunc(idleGL);

	
	cout << "Ecran   wxh " << width << "x" << height << endl;
	cout << "Fenetre x,y wxh "<< xPos <<","<< yPos <<" " << width << "x" << height << endl;

	glutKeyboardFunc(glutKeyboardFunc);
	glutKeyboardUpFunc(glutKeyboardUpFunc);
	glutSpecialFunc(glutSpecialFunc);
	glutSpecialUpFunc(glutSpecialUpFunc);
	glutMouseFunc(glutMouseFunc);
	glutMotionFunc(glutMotionFunc);
	glutPassiveMotionFunc(glutPassiveMotionFunc);

    if ( bFull )        glutFullScreen();

	initGL(argc, argv);
    glewInit();
    
    charge_var();
    
    CreateAllWindows();
    set_asservissement();

    panelCourbe->get_vOrigine().x = xSuivi;
    panelCourbe->get_vOrigine().y = ySuivi;
    panelCourbe->get_vOrigine().z = 0.0;
    change_joy( xSuivi, ySuivi );
    
    parse_option(argc, argv);
    
    
    logf ((char*)"############## START MANAGER ###################");
    
    Camera_mgr& cam_mgr = Camera_mgr::getInstance();

    cam_mgr.reOrder();
    //cam_mgr.getCurrent();
    //cam_mgr.active();

    Connexion_mgr::getInstance().start();
    PanelConsoleSerial::getInstance();
    PanelConsoleSerial::getInstance().setVisible( bPanelSerial );
    Serveur_mgr::getInstance().start_init();
    Serveur_mgr::getInstance().start_deplacement();

    init_var();
    var.setSauve();
    
    if ( var.getb("bNuit") )    panelStdOut->setColor( 0xffff0000 );
    else                        panelStdOut->setColor( COLOR_WHITE );
    
    double gris = 0.2;
    glClearColor( gris, gris, gris,1.0);
    
    // Pre-Charge la texture pour eviter un bug
    WindowsManager::getInstance().loadResourceImage( "images/file.png" );
    WindowsManager::getInstance().loadResourceImage( "images/dir.png" );
    FileBrowser::getInstance();
    //BluetoothManager::getInstance();
    WindowsManager::genereMipMap( false );

    setColor();
    
    compute_matrix();
    bCorrection = false;
    
	vizier.charge();
	// -----------------------------------         EN ROUTE    
    glutMainLoop();
	// -----------------------------------         BYE BYE


	return 0;
}



#endif
