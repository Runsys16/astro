#ifndef MAIN_CPP
#define MAIN_CPP

#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>


#include "Python.h"



#include "main.h"
#include "v4l2.h"
#include "camera.h"
#include "camera_mgr.h"
#include "connexion_mgr.h"
#include "Mathlib.h"
#include "timer.h"
#include "pleiade.h"
#include "panel_console_serial.h"
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
#include <GL/freeglut_ext.h>

//#define DEBUG 1
#define SIZEPT  20
//#define AXE_X   (300.0/4.0)
//#define AXE_Y   (3.0*300.0/4.0)
//float   xStartAxe = 50.0;

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
PanelText*          R;
PanelText*          G;
PanelText*          B;
PanelText*          L;
PanelText*          SP;
PanelText*          pJoyXY;
PanelText*          pArduino;
PanelText*          pStellarium;
PanelText*          pDeplacement;
PanelText*          pAD;
PanelText*          pDC;
PanelText*          pMode;
PanelText*          pAsservi;
/*
PanelText*          pXMax;
PanelText*          pXMin;
PanelText*          pYMax;
PanelText*          pYMin;
*/

PanelText*          pHertz;
PanelText*          pFPS;
PanelText *         pStatus;

Pleiade*            pPleiade;

float               ac;
float               dc;

//Device_cam          camera      = Device_cam();

char                background[]="frame-0.raw";
float               prevTime    = -1.0f;

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
bool                bQuit;
bool                bAffIconeCapture = true;;


bool                bPanelControl  = true;
bool                bPanelHelp     = false;
bool                bPanelResultat = false;
bool                bPanelCourbe   = false;
bool                bPanelStdOut   = false;
bool                bPanelSerial   = false;
bool                bAfficheVec    = false;
bool                bMouseDeplace  = false;
bool                bFileBrowser   = false;
bool                bStellarium    = false;
bool                bPleiade       = false;
bool                bSauve         = false;
bool                bOneFrame      = false;
bool                bStdOut        = false;
bool                bSimu          = false;
bool                bAffCentre     = false;
bool                bAffSuivi      = true;
bool                bSound         = true;
bool                bInverseCouleur= false;

int                 wImg;
int                 hImg;
int                 nPlanesImg;

ivec2               vCameraSize;
GLubyte*            ptr;
vector<string>      exclude;

int                 _r[256];
int                 _g[256];
int                 _b[256];

float               xSuivi;
float               ySuivi;
//vector<Star*>       v_tStars;
float               fTimeMili;
//--------------------------------------------------------------------------------------------------------------------
//              Ratio Witdh Height
//--------------------------------------------------------------------------------------------------------------------
double              rw;
double              rh;

int                 xCam;
int                 yCam;
int                 dxCam;
int                 dyCam;

float               zoom;

ivec2               mouse;

#define             GET_OFFSET(x,y,width)   (3*y*width+x)
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
vector<vec2>        t_vResultat;
vector<vec2>        t_vSauve;
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

float               pas = 4000;
float               cAD;
ivec2               calibreMove[2];
vec2                vRef;
vec3                vecAD[2];
vec3                vecDC[2];
vec3                vDepl[2];
mat3                mChange;
vec3                vOri;
vec3                vTr;
bool                bCorrection = false;
float               fTimeCorrection = 3.0;
float               fTimeCpt = 0.0;
float               fLimitCorrection = 80.0;
//float               err = 2.0;
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
float fTimer = 0.0;
bool bAsc = true;
bool bDec = true;
bool bSui = false;
bool bJoy = false;
bool bRet = false;
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
        string mes = "Pyhton polaris : " +  to_string(ret);
        alertBox(mes);
        logf( (char*)mes.c_str() );
    }
    
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

        charge_guidage( string(str) );
        if ( panelCourbe != NULL )  
        {
            panelCourbe->get_vOrigine().x = vOri.x;
            panelCourbe->get_vOrigine().y = vOri.y;
        }
    }
    
    workDirSauveCourbe = FileBrowser::getInstance().getWorkingDir();
    var.set( "DirSauveCourbe", workDirSauveCourbe );
    FileBrowser::getInstance().setFiltre( "" );
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
            logf( (char*)"  Sauve guidage %s", (char*)s.c_str() );
            //charge_guidage( string(str) );
            workDirSauveCourbe = FileBrowser::getInstance().getWorkingDir();
            var.set( "DirSauveCourbe", workDirSauveCourbe );
            FileBrowser::getInstance().setFiltre( "" );
            
            bSauve = true;
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
    logf( (char*)"callback_enregistre( %s, %d, \"%s\" )", BOOL2STR(b), ii, (char*)str );

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
void updatePanelPause()
{
    if ( bPause )   pStatus->changeText((char*)"Pause" );
    else            pStatus->changeText((char*)"----" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void updatePanelPause(bool b)
{
    if ( b != bPause)            
    {
        bPause = b; 
        updatePanelPause();
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
float pond2mag( float ponderation )
{
    return  -(log( ponderation ) / log(2.0)) + 17.0;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
float hms2rad(struct hms& HMS)
{
    return (HMS.h + HMS.m/60.0 + HMS.s/3600.0) / 12.0*M_PI;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
float dms2rad(struct dms& DMS)
{
    return (DMS.d + DMS.m/60.0 + DMS.s/3600.0) / 180.0*M_PI;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void rad2hms(struct hms& HMS, float r)
{
    float sign = 1.0;
    if ( r<0.0 )
    {
        sign = -1.0;
        r *=-1.0;
    }
    float h = r / M_PI * 12.0;
    int H = h;
    HMS.h = H;
    
    float m = (h-H) * 60.0;
    //if ( h < 0.0 )  m *= -1.0;
    int M = m;
    HMS.m = M;
    
    float s = (m-M) * 60.0;
    HMS.s = s;
    
    HMS.h *= sign;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void rad2dms(struct dms& DMS, float r)
{
    float sign = 1.0;
    if ( r<0.0 )
    {
        sign = -1.0;
        r *=-1.0;
    }

    float d = r / M_PI * 180.0;
    int D = d;
    DMS.d = D;
    
    float m = (d-D) * 60.0;
    //if ( d < 0.0 )  m *= -1.0;
    int M = m;
    DMS.m = M;
    
    float s = (m-M) * 60.0;
    DMS.s = s;

    DMS.d *= sign;
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
void updatePanelResultat(float x, float y, float mag)
{
    int xPanel = x;
    int yPanel = y;
    
    //mag = -1.0;

    Camera_mgr& mgr = Camera_mgr::getInstance();
    mgr.tex2screen(xPanel,yPanel);
    panelResultat->setPos(xPanel+20 , yPanel+20);

    
    char sSkyPoint[255];
    char sR[255];
    char sG[255];
    char sB[255];
    char sL[255];
    vec3& v = panelCourbe->get_vOrigine();
    sprintf( sSkyPoint, "(%0.2f,%0.2f) / (%0.2f,%0.2f)  mag=%0.2f", x, y, v.x, v.y, mag );
    
    //logf( (char*)"updatePanelResultat(%d, %d) => %s", x, y, (char*)sSkyPoint );
    
    SP->changeText(sSkyPoint);
    /* 
    sprintf( sR, "r=%03d", (int)getSkyPoint_colorR(offset) );
    sprintf( sG, "g=%03d", (int)getSkyPoint_colorG(offset) );
    sprintf( sB, "b=%03d", (int)getSkyPoint_colorB(offset) );
    sprintf( sL, "l=%03d", (int)getSkyPoint_colorL(offset) );

    R->changeText(sR);
    G->changeText(sG);
    B->changeText(sB);
    L->changeText(sL);
    */
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
void change_hertz(float hz)
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
    if ( b )    pArduino->changeText((char*)"Arduino");
    else        pArduino->changeText((char*)"----");
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void change_joy(int x, int y)
{
    static char sJoyXY[255];
    sprintf( sJoyXY, "Suivi(%d, %d) %s", x, y, BOOL2STR(bSuivi) );
    pJoyXY->changeText((char*)sJoyXY );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void change_joy(float x, float y)
{
    static char sJoyXY[255];
    sprintf( sJoyXY, "Suivi(%0.2f, %0.2f) %s", x, y, BOOL2STR(bSuivi) );
    pJoyXY->changeText((char*)sJoyXY );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void change_ad(float ad)
{
    ad = DEG2RAD(ad);
    struct hms HMS;
    rad2hms( HMS, ad);
    
    char    buff[255];
    sprintf( buff, "AsDr : %02dh%02dm%0.2fs", (int)HMS.h, (int)HMS.m, HMS.s );
    
    pAD->changeText( buff );

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void change_dc(float dc)
{
    dc = DEG2RAD(dc);
    struct dms DMS;
    rad2dms( DMS, dc);
    
    char    buff[255];
    sprintf( buff, "Decl : %02dd%02dm%0.2fs", (int)DMS.d, (int)DMS.m, DMS.s );
    
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
void sauve()
{
    //string filename = "/home/rene/.astropilot/sauvegarde.txt";
    logf( (char*)"Sauvegarde des valeurs dans '%s'", (char*)filenameSauve.c_str() );
    
    std::ofstream fichier;
 
    fichier.open(filenameSauve, std::ios_base::app);

    if ( !fichier ) 
    {
        logf( (char*)"[ERROR]impossble d'ouvrir : '%s'", (char*)filenameSauve.c_str() );
    }

    for(int i=0; i<t_vSauve.size(); i++)
    {
        fichier << "( " << t_vSauve[i].x << " , " <<  t_vSauve[i].y << " ) / ";
        fichier << "( " << panelCourbe->get_vOrigine().x << " , " <<  panelCourbe->get_vOrigine().y << " )\n";
    }

    fichier.close();
    
    t_vSauve.clear();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void charge_guidage(string filename)
{
    //string filename = "/home/rene/.astropilot/svg.txt";
    //string filename = "/home/rene/.astropilot/m57.txt";
    //string filename = "/home/rene/.astropilot/sauvegarde.txt";
    logf( (char*)"Chargement des valeurs dans '%s'", (char*)filename.c_str() );
    
    std::ifstream fichier;
    fichier.open(filename, std::ios_base::app);

    if ( !fichier ) 
    {
        logf( (char*)"[ERROR]impossble d'ouvrir : '%s'", (char*)filename.c_str() );
    }

    
    //panelCourbe->get_vOrigine().x = 0.0;
    //panelCourbe->get_vOrigine().y = 0.0;
    
    t_vResultat.clear();
    

    string line;
    int nbLigne = 0;
    
    
    while ( getline (fichier, line) )
    {
        //cout << line << '\n';
        float rx, ry, ox, oy;

        //logf( (char*) line.c_str() );
        sscanf( line.c_str(), "( %f , %f ) / ( %f , %f )", &rx, &ry, &ox, &oy );

        vOri.x = ox;
        vOri.y = oy;

        t_vResultat.push_back( vec2(rx,ry) );
        nbLigne++;
    }    
    int n = t_vResultat.size();
    logf( (char*)"Lecture de %d donnees dans %d lignes", n, nbLigne );
    
    fichier.close();
    
    t_vSauve.clear();
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
    float x, y;
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

        sscanf( line.c_str(), "( %f , %f )", &x, &y );
        //logf ( (char*)" v(%f,%f)", x, y );
        v.x = x;
        v.y = y;

        trace->push_back( vec2(x,y) );
        nbl++;
    }
    
    fichier.close();
    logf( (char*)"Lecture de %d lignes %d", nbl );
    
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
    

    vec2*       pV = Camera_mgr::getInstance().getSuivi();
    
    if ( bSuivi && pV != NULL )
    {
        //logf( (char*)"Suivi (%0.2f, %0.2f)", pV->x, pV->y ); 

        float xx = pV->x;
        float yy = pV->y;

        //xSuivi = xx;
        //ySuivi = yy;
        

        if ( t_vResultat.size()>20000)      t_vResultat.clear();

        vec2 v = vec2(xx, yy);

        if ( t_vResultat.size() > 2000 )
        {
            t_vResultat.erase ( t_vResultat.begin()+0);
        }
        t_vResultat.push_back(v);

        if ( bSauve )
        {
            if ( t_vSauve.size() > 200 )       sauve();
        }
        else 
        {
            if ( t_vSauve.size() > 200 )       t_vSauve.clear();
        }

        t_vSauve.push_back(v);
        
        updatePanelResultat( xx, yy, 0.0 );
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void getSuiviParameter(void)
{
    Camera_mgr&  cam_mgr = Camera_mgr::getInstance();
    //cam_mgr.active();

    panelCourbe->set_xCam( cam_mgr.get_xCam() );
    panelCourbe->set_yCam( cam_mgr.get_yCam() );
    panelCourbe->set_dxCam( cam_mgr.get_dxCam() );
    panelCourbe->set_dyCam( cam_mgr.get_dyCam() );

    vCameraSize.x = 1920.0;
    vCameraSize.y = 1080;
    vCameraSize = cam_mgr.get_vCameraSize();

    rw = (float)vCameraSize.x/(float)panelCourbe->get_dxCam();
    rh = (float)vCameraSize.y/(float)panelCourbe->get_dyCam();

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void idleGL(void)
{
    //logf( (char*)"*** IDLE GL ***" );
    Timer&          timer = Timer::getInstance();
	fTimeMili = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

    char sFPS[] = "fps 0000";
    sprintf( sFPS,"fps %d", *Timer::getInstance().getvFPSCounter() );
    pFPS->changeText((char*)sFPS);
    
	timer.Idle();

    idleStatus();

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
    // Geqtion du suivi des eroiles
    //-----------------------------------------------------------------------
    if (bSuivi)
    {
        updatePanelResultat( xClick, yClick, 0 );
    }
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
	
    float elapsedTime = -1;
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
	    fTimer += elapsedTime;
	    if ( fTimer >= 10.0 )
	    {
	        fTimer -= 10.0;
            
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
	    if ( bCorrection && fTimeCpt > 0.0 )
	    {
            fTimeCpt -= fTimeCorrection;
            if ( fTimeCpt > 0.0 )    fTimeCpt = 0.0;//-= fTimeCorrection;

            vec2* pv = Camera_mgr::getInstance().getSuivi();
            if ( pv != NULL )
            {
	            //float dx = xSuivi - panelCourbe->get_vOrigine().x;
	            //float dy = ySuivi - panelCourbe->get_vOrigine().y;
	            float dx = xSuivi - pv->x;
	            float dy = ySuivi - pv->y;
                //logf( (char*)" Asservissement delta(%0.2f, %0.2f)", dx, dy );
	            
	            if ( fabs(dx) > panelCourbe->get_err() || fabs(dy) > panelCourbe->get_err() )
	            {
	                vec3 w = vec3( xSuivi, ySuivi, 0.0);
	                vec3 v = w - vec3( pv->x, pv->y, 0.0);
	                float l = v.length();

	                if ( l > fLimitCorrection )
	                    logf( (char*)"[WARNING]Suivi l=%0.2f", l ); 

                    if ( !bMouseDeplace && Serial::getInstance().getFree() ) {

                        vec3 res = mChange * v;
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
static void quit(void)
{
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
    // touche tab
	case 9:
		//WindowsManager::getInstance().swapVisible();
		{
		    /*
            logf( (char*)"-------------- Touche CTRL+TAB" );
    	    logf( (char*)"Key (TAB) : ByeBye !!" );
	        Camera* pCurrent = cam_mgr.getCurrent();
	        cam_mgr.active();

            getSuiviParameter();
            */
        }
		break;
    default:
		{
		    cout << "Default..." << endl;
        }
        break;
    }		
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void glutKeyboardFunc(unsigned char key, int x, int y) {
    //logf( (char*)"*** glutKeyboardFunc( %d, %d, %d)", (int)key, x, y );
	int modifier = glutGetModifiers();
	
    bFileBrowser = FileBrowser::getInstance().getVisible();
    Camera_mgr&  cam_mgr = Camera_mgr::getInstance();
    
        
    
    if (tAlert.size() != 0 )
    {
        if ( bQuit )   
        {
            if (key == 27 )     { 
                //quit();
                logf((char*)"**[ARRET D'URGENCE]**" );
                bSuivi = false;
                var.set( "bSuivi", bSuivi );
                change_joy( xSuivi, ySuivi );

                char cmd[]="n";

                Serial::getInstance().write_string(cmd);
                Serial::getInstance().reset();

                bCorrection = false;
            }
        }
        bQuit = false;
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
        logf( (char*)" Touche ALT %c", key );
        glutKeyboardFuncCtrl(key,  x,  y);
        return;
	}
    else
    if ( panelApn && panelApn->keyboard(key, x, y) )      return;

	
	switch(key){ 
	
	// CTRL q
	case 17:
	    {
	    logf( (char*)"Key (ctrl+Q) : ByeBye !!" );
	    quit();
	    }
        break;

	// touche escape
	case 27:
	    {
	    logf( (char*)"Key (ESC) : ByeBye !!" );
	    alertBox( "Confirmez l'arret de la monture 'ESC'" );
	    bQuit = true;
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
	// CTRL P
    case 16:
		{
	    logf( (char*)"Key (ctrl+p) : Noop" );
        }
        break;
	// CTRL F
    case 6:
		{
            logf( (char*)"Key (ctrl+f) : FullScreen" );
            Captures::getInstance().fullscreen();
        }
        break;
	// CTRL I
    // touche tab
	case 9:
		//WindowsManager::getInstance().swapVisible();
		{
		
        //logf( (char*)"Key (TAB) : Image suivante/(shift)precedente" );
    	if (modifier == GLUT_ACTIVE_CTRL)
    	{
            logf( (char*)"Key (ctrl+TAB) : Camera suivante" );
	        Camera_mgr&  cam_mgr = Camera_mgr::getInstance();
	        cam_mgr.active();

            getSuiviParameter();
            break;
    	}
    	else if (modifier == GLUT_ACTIVE_SHIFT)
    	{
            logf( (char*)"Key (Shift+TAB) : Image precedente" );
            //Captures::getInstance().showIcones();
	        Captures::getInstance().rotate_capture_moins(false);
            break;
    	}
    	else if (modifier == GLUT_ACTIVE_ALT)
    	{
            logf( (char*)"-------------- Touche ALT+TAB" );
            break;
    	}
    	else
    	{

            logf( (char*)"Key (TAB) : Image suivante" );
            Captures::getInstance().rotate_capture_plus(false);
        }
                
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
            Captures::getInstance().rotate_capture_moins(true);
    	}
	    break;
    
    case 126:
    	{
           // Captures::getInstance().showIcones();
            logf( (char*)"Key (126) : Image precedente" );
	        Captures::getInstance().rotate_capture_moins(true);
    	}
	    break;
    
    case 'A':
        logf( (char*)"Key (A) : Pt1 Ascension droite" );
        {
        if ( !bModeManuel )
        {
            vec2* pv = Camera_mgr::getInstance().getSuivi();
            if ( pv != NULL )
            {
                float x   = pv->x;
                float y   = pv->y;

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
        logf( (char*)"Key (a) : Pt0 Ascension droite" );
        if ( !bModeManuel )
        {
            vec2* pv = Camera_mgr::getInstance().getSuivi();
            if ( pv != NULL )
            {
                float x   = pv->x;
                float y   = pv->y;

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
        //logf( (char*)"Envoi de la commande",  cmd );
        Serial::getInstance().write_string(cmd);
        }
        break;

    case 'b':
        {
        logf( (char*)"Key (b) : Bluetooth start" );
        //BluetoothManager::getInstance().start();
        }
        break;
    
    case 'B':
        {
        logf( (char*)"Key (b) : Bluetooth disconnect" );
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
        logf( (char*)"Key (D) : Pt1 Declinaison" );
        if ( !bModeManuel )
        {
            vec2* pv = Camera_mgr::getInstance().getSuivi();
            if ( pv != NULL )
            {
                float x   = pv->x;
                float y   = pv->y;

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
        logf( (char*)"Key (d) : Pt0 Declinaison" );
        
        if ( !bModeManuel )
        {
            vec2* pv = Camera_mgr::getInstance().getSuivi();
            if ( pv != NULL )
            {
                float x   = pv->x;
                float y   = pv->y;

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
        //logf( (char*)"Envoi de la commande",  cmd );
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
    case 'G':
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
            else
                panelApn->setVisible( !panelApn->getVisible() );
        }
        break;

    case 'I':
        {
        bInverseCouleur = !bInverseCouleur;
        logf( (char*)"Key (I) : Inversion de couleur on/off  %s ", BOOL2STR(bInverseCouleur) );
        var.set( "bInverseCouleur", bInverseCouleur );
        }
        break;
	case 'j':
		{
    		fLimitCorrection *= 0.9f;
            logf( (char*)"Key (j) : Diminue la limite de correction : %0.2f", fLimitCorrection );
            var.set("fLimitCorrection", (float)fLimitCorrection);
        }
		break;
	case 'J':
		{
    		fLimitCorrection /= 0.9f;
            logf( (char*)"Key (J) : Augmente la limite de correction : %0.2f", fLimitCorrection );
            var.set("fLimitCorrection", (float)fLimitCorrection);
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
                float f = var.getf(p->first);
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
        logf( (char*)"Key (m) : Autorisation du deplcament par la souris (bouton du milieu) ...");
        bMouseDeplace = !bMouseDeplace;
        logf( (char*)"  bMouseDeplace = %s", BOOL2STR(bMouseDeplace) );

	    if (bMouseDeplace)              pDeplacement->changeText((char*)"Deplacement");
        else                            pDeplacement->changeText((char*)"----");
        }
        break;

    case 'n':
        {
        logf( (char*)"Key (n) : Affiche/cache les capture");
        bAffIconeCapture = !bAffIconeCapture;
        logf( (char*)"  bAffIconeCapture = %s", BOOL2STR(bAffIconeCapture) );
        Captures::getInstance().switchAffIcones();
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
    	bModeManuel = !bModeManuel;
        logf( (char*)"Key (O) : Mode manuel (%s)", BOOL2STR(bModeManuel) );
        set_mode();
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
        bOneFrame = true;
        }
        break;

    case 'q':
        {
        logf( (char*)"Key (q) : Lance un script python");
        //Py_SetProgramName(argv[0]);  /* optional but recommended */
        Py_Initialize();
        PyRun_SimpleString("from time import time,ctime\n"
                         "print( 'Hello Word' )\n");
        Py_Finalize();
        }
        break;

    case 'Q':
        {
            thread( &commande_polaris).detach();
        }
        break;
    case 'r' :
        {
        logf( (char*)"Key (Q) : Ouvre un fichier de suivi (.guid)");

            FileBrowser::getInstance().setCallBack(&cb_cguidage);
            FileBrowser::getInstance().setFiltre( ".guid" );
            if ( var.existe("DirSauveCourbe") )     workDirSauveCourbe = *var.gets( "DirSauveCourbe" );
            FileBrowser::getInstance().change_dir( workDirSauveCourbe );
            FileBrowser::getInstance().affiche();
        }
        break;

    case 'R' :
        {
        logf( (char*)"Key (R) : Reset les datas de suivi" );
        t_vResultat.clear();
        t_vSauve.clear();
        var.set( "FileResultat", string("---"));
        }
        break;

    case 's':
        {
        logf( (char*)"Key (s) : Trouve toutes les etoies" );
            
            if ( Captures::getInstance().isMouseOverCapture(x, y)  )
            {
                Captures::getInstance().findAllStar();
            }
            else
            {
                if ( Camera_mgr::getInstance().getCurrent() != NULL )
                    Camera_mgr::getInstance().findAllStars();
            }
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
            //panelCourbe->get_vOrigine().x = xSuivi;
            //panelCourbe->get_vOrigine().y = ySuivi;
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
        }
        break;
    case 'w' :
        {
        //BluetoothManager::getInstance().centre_joystick();
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
        Camera_mgr::getInstance().togglePanel();
        log( (char*)"Toggle panelCamera !!!" );
        }
        break;
    case GLUT_KEY_F2:
        {
        bPanelHelp = !bPanelHelp;
        var.set("bPanelHelp", bPanelHelp);
        panelHelp->setVisible(bPanelHelp);
        if ( bPanelHelp )       WindowsManager::getInstance().onTop(panelHelp);
        log( (char*)"Toggle panelHelp !!!" );
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
        PanelWindow* p = PanelConsoleSerial::getInstance().getWindow();
        p->setVisible( bPanelSerial );
        if ( bPanelSerial )       WindowsManager::getInstance().onTop(p);
        log( (char*)"Toggle serial !!!" );
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



	WindowsManager::getInstance().keyboardSpecialFunc( key, x, y);
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
    mouse.x = x;
    mouse.y = y;

    WindowsManager& wm  = WindowsManager::getInstance();
    Camera_mgr&     mgr = Camera_mgr::getInstance(); 

	wm.mouseFunc(button, state, x, y);
    //WindowsManager::getInstance().onBottom(panelPreView);
    if ( panelStatus->isMouseOver(x, y) )
    {
        //logf( (char*)"Souris sur la barre de status" );
        return;
    }

    PanelWindow*    pPreviewCam = NULL;
    Panel *         pCapture = NULL;
    Panel*          pMouseOver = NULL;

    if ( mgr.getCurrent() != NULL  &&  mgr.getCurrent()->getPanelPreview() != NULL )
    {
        pPreviewCam = mgr.getCurrent()->getPanelPreview();
        pCapture      = wm.getCapture();
        pMouseOver  = wm.findPanelMouseOver(x, y);
    }
    
	if ( bMouseDeplace && button == GLUT_MIDDLE_BUTTON && state == 0 )
	{
        getSuiviParameter();

        mgr.onBottom();
        
	    int X = x;
	    int Y = y;
	    
	    mgr.screen2tex(X,Y);
	    
	    xClick = X;
	    yClick = Y;

        vDepl[0].x = xClick;
        vDepl[0].y = yClick;
        vDepl[0].z = 0.0;
        logf( (char*)"vDepl[0](%0.2f, %0.2f)", vDepl[0].x, vDepl[0].y );

	    logf( (char*)"state = 0" );
	}
	if ( bMouseDeplace && button == GLUT_MIDDLE_BUTTON && state == 1 )
	{
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
        logf( (char*)"Envoi de la commande",  cmd );

        Serial::getInstance().write_string(cmd);
	}

	//if ( bPause && button == 0 && state == 0 )	{
    if ( bModeManuel && button == 0 && state == 0 )	{
        getSuiviParameter();

        mgr.onBottom();
        
	    int X = x;
	    int Y = y;
	    
	    mgr.screen2tex(X,Y);
	    
	    xClick = X;
	    yClick = Y;

	    logf( (char*)"Click x=%d y=%d  X=%d Y=%d vCameraSize.x=%d" , x, y, X, Y, vCameraSize.x );
	    
	} 
    else
	if ( !bModeManuel && button == 0 && state == 0 && pMouseOver == pPreviewCam )	{

	    logf( (char*)"xSuivi=%0.2f ySuivi=%0.2f   " , xSuivi, ySuivi );
	    //logf( (char*)"    l=%d rgb=%d,%d,%d" , r,g,b,  l );

	} 

    Camera_mgr::getInstance().onBottom();
    onTop();
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
    long color;
    if (bNuit)                  color = 0xFFFF0000;
    else                        color = 0xFFFFFFFF;

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

	R = new PanelText( (char*)"r=0",  		PanelText::NORMAL_FONT, x0 + dx0*l++, y0  );
	panelResultat->add( R ); 
	G = new PanelText( (char*)"g=0",  		PanelText::NORMAL_FONT, x0 + dx0*l++, y0  );
	panelResultat->add( G ); 
	B = new PanelText( (char*)"b=0",  		PanelText::NORMAL_FONT, x0 + dx0*l++, y0  );
	panelResultat->add( B ); 
	L = new PanelText( (char*)"l=0",  		PanelText::NORMAL_FONT, x0 + dx0*l++, y0  );
	panelResultat->add( L ); 

	SP = new PanelText( (char*)" ",  		PanelText::NORMAL_FONT, x0, y0+16  );
	panelResultat->add( SP ); 

    panelResultat->setSize( x0 + l*dx0+100, dy+16);
    panelResultat->setBackground((char*)"images/background.tga");
    panelResultat->setVisible(bPanelResultat);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int x_help = 0;
int y_help = 0;
static void addString( string s )
{
    if ( s.size() == 0 )       { y_help += 15; return; }
    PanelText* p = new PanelText( (char*)s.c_str(),  		PanelText::NORMAL_FONT, x_help, y_help );
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
    panelHelp->setDisplayGL(displayGLnuit_cb);
	panelHelp->setPosAndSize( X, Y, DX, DY);
	panelHelp->setVisible(bPanelHelp);
	//panelHelp->setBackground( (char*)"/home/rene/programmes/opengl/video/images/background.tga");
	panelHelp->setBackground( (char*)"images/background.tga");
    
	int y = 10;
	int dy = 15;
	int l=50;

	
	addString( "---- CAMERA ----");
	addString( "H: Change le nom d une image de la camera");
	addString( "h: Enregistre une image de la camera courante");

	addString( "    Brightness\t\tB/b" );
	addString( "    Contrast\t\t\tC/c" );
	addString( "    Saturation\t\tS/s" );
	addString( "    Hue\t\t\tH/h" );
	addString( "    Gamma\t\t\tG/g" );
	addString( "    Sharpness\t\tZ/z" );
	addString( "    Exposure\t\t\tE/e" );
	addString( "    White balance\t\tW/w" );
	addString( "    A: enregistre les parametres de la camera");
	addString( "    a: rappelle les parametres de la camera");
	
	addString( "---- MODE NORMAL ----");
	addString( "ctrl+TAB: camera suivante" );
	addString( "   o\t: Ouvre/Ferme la fenetre pleiades");
	addString( "   p\t: Pause de l'affichage de pleiades");
	addString( "   P\t: Image suivante");
	addString( "   f\t: Ouvrir un fichier image");
	addString( "   F\t: Active/Desactive la simu");
	addString( "TAB\t: Change l'affichage des fichiers" );
	addString( "  -\t: Toutes les images sont affichees en icones");
	addString( "F11\t: Charge la prochaine image");
	addString( "F12\t: Efface la derniere image");
	addString( "   r\t: Rappel des mesures de decalage en x,y du fichier beltegeuse.txt");
	addString( "   R\t: Test alert BOX");
	addString( "   l\t: List les ports /dev + les controles ");
	addString( "   L\t: List les variables");
	addString( "   w\t: Centre le joystick");
	addString( "   W\t: Surveille un repertoire");
	addString( "   i\t: Prend une photo sur le pentax");
	addString( "   K\t: Active/desactive le son");
	addString( "   n\t: Affiche/cache les images (F7)");
	addString( "   N\t: Mode nuit on/off");
	addString( "   q\t: Lance un script python");
	addString( "   Q\t: Ouvre une image fits");
	addString( " j/J\t: Change la taille du cercle d'asservissement");

	addString( "---- TRANSFORM MATRIX ----");
	addString( " a/A\t: Vecteur en ascension droite");
	addString( " d/D\t: Vecteur en declinaison");
	addString( "   M\t: Calcul la matrice de transformation");
	addString( "   y\t: Affiche les vecteurs");
	addString( "   m\t: Deplacement à la souris");
	addString( "   O\t: Mode souris / mode suivi");

	addString( "---- TRACES ----");
	addString( "   C\t: Lance/arrete l\'enregistrement de trace");
	addString( "   c\t: Nouvelle trace");
	addString( "   e\t: Affiche les traces (ctrl+tab)");
	addString( "   E\t: Supprime la derniere trace");
	addString( "   z\t: Charge les traces");
	addString( "   Z\t: Sauve les traces");

	addString( "---- SUIVI ----");
	addString( "   S\t: Lance/Stop le suivi");
	addString( " t/T\t: change le temps de correction");
	addString( "   Y\t: Lance l' asservissement");
	addString( "   V\t: Initialise les coordonnees de suivi");
	addString( "   v\t: Sauvegarde des coordonnees de suivi dans un fichier .guid");
    addString( "   U\t: Affichage du centre de la camera on/off");
    addString( "   u\t: Affichage du suivi on/off");
	

	addString( "");
	addString( "ESC\t: --- SORTIE DU LOGICIEL ---" );

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

	panelStatus = new PanelSimple();
	panelStatus->setPosAndSize( x, y, dx, dy );

	pStatus = new PanelText( (char*)"Status",		PanelText::NORMAL_FONT, 10, 2 );
	panelStatus->add( pStatus );
    if ( bPause )   pStatus->changeText((char*)"Pause" );
    else            pStatus->changeText((char*)"-----" );

    pFPS = new PanelText( (char*)"0",		PanelText::NORMAL_FONT, width-100, 2 );
	panelStatus->add( pFPS );

    pHertz = new PanelText( (char*)"0",		PanelText::NORMAL_FONT, width-150, 2 );
	panelStatus->add( pHertz );

    logf((char*)"** CreateStatus()  panelSatuts  %d", width);
    pArduino = new PanelText( (char*)"----",		PanelText::NORMAL_FONT, width-300, 2 );
	panelStatus->add( pArduino );

    pJoyXY = new PanelText( (char*)"Joy(---, ---)",		PanelText::NORMAL_FONT, width-620, 2 );
	panelStatus->add( pJoyXY );

    pStellarium = new PanelText( (char*)"----",		PanelText::NORMAL_FONT, width-230, 2 );
	panelStatus->add( pStellarium );

    pAD = new PanelText( (char*)"AsDr :",		PanelText::NORMAL_FONT, 60, 2 );
	panelStatus->add( pAD );
    pDC = new PanelText( (char*)"Decl :",		PanelText::NORMAL_FONT, 200, 2 );
	panelStatus->add( pDC );

    pMode = new PanelText( (char*)" ",		PanelText::NORMAL_FONT, 350, 2 );
	panelStatus->add( pMode );
 
    pAsservi = new PanelText( (char*)" ",		PanelText::NORMAL_FONT, 850, 2 );
	panelStatus->add( pAsservi );

    pDeplacement = new PanelText( (char*)" ",		PanelText::NORMAL_FONT, width-410, 2 );
	panelStatus->add( pDeplacement );


	if (bMouseDeplace)              pDeplacement->changeText((char*)"Deplacement");
    else                            pDeplacement->changeText((char*)"----");

	if (!bModeManuel)               pMode->changeText((char*)"Mode suivi");
    else                            pMode->changeText((char*)"Mode souris");

    if (bCorrection)                pAsservi->changeText((char*)"Asservissemnent");
    else                            pAsservi->changeText((char*)" ");

    if ( bStellarium )              pStellarium->changeText( (char*)"Stellarium" );
    else                            pStellarium->changeText( (char*)"----" );



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
        sTab = sTab + "   ";
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
    char chaine[255];
    va_list arglist;

    va_start( arglist, fmt );
    vsprintf( chaine, fmt, arglist );
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

    dpy = XOpenDisplay(":0");
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

    var.set( "fLimitCorrection", (float)80.0);
    
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

    if ( var.existe("bSimu") )          bSimu       = var.getb("bSimu");
    if ( var.existe("bAffSuivi") )      bAffSuivi   = var.getb("bAffSuivi");
    if ( var.existe("bAffCentre") )     bAffCentre  = var.getb("bAffCentre");
    if ( var.existe("bSound") )         bSound      = var.getb("bSound");
    if ( var.existe("bInverseCouleur")) bInverseCouleur = var.getb("bInverseCouleur");
    if ( var.existe("fLimitCorrection")) fLimitCorrection = var.getf("fLimitCorrection");
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void exit_handler()
{
    //BluetoothManager::getInstance().disconnect();
    Serveur_mgr::getInstance().close_all();
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
    
    vCameraSize.x = 1280;
    vCameraSize.y = 720;

    vCameraSize.x = 1920;
    vCameraSize.y = 1080;
    
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
    set_mode();
    inverse_texture( pButtonSerial,  bPanelSerial,       "arduino" );
    inverse_texture( pButtonStdOut,  bPanelStdOut,       "" );
    inverse_texture( pButtonHelp,    bPanelHelp,         "help" );
    inverse_texture( pButtonCourbe,  bPanelCourbe,       "courbe" );
    //inverse_texture( pButtonMode,    bModeManuel,        "cible" );

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
    Serveur_mgr::getInstance().start_1();
    Serveur_mgr::getInstance().start_2();

    init_var();
    var.setSauve();
    
    if ( var.getb("bNuit") )    panelStdOut->setColor( 0xffff0000 );
    else                        panelStdOut->setColor( 0xffffffff );
    
    float gris = 0.2;
    glClearColor( gris, gris, gris,1.0);
    
    // Pre-Charge la texture pour eviter un bug
    WindowsManager::getInstance().loadResourceImage( "images/file.png" );
    WindowsManager::getInstance().loadResourceImage( "images/dir.png" );
    FileBrowser::getInstance();
    //BluetoothManager::getInstance();
    WindowsManager::genereMipMap( false );

    setColor();
    
    compute_matrix();
    //bStdOut = true;
    bCorrection = false;
    glutMainLoop();


	return 0;
}



#endif
