#ifndef MAIN_CPP
#define MAIN_CPP


#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

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
//#include "star.h"

//#define DEBUG 1
#define SIZEPT  20
//#define AXE_X   (300.0/4.0)
//#define AXE_Y   (3.0*300.0/4.0)
float   xStartAxe = 50.0;

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
VarManager& var = VarManager::getInstance();
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
string              currentDirectory;
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
//PanelConsole *      pConsoleSerial;
PanelWindow *       panelHelp;
PanelWindow *       panelResultat;
PanelWindow *       panelCourbe;
PanelWindow *       panelStdOutW;

//PanelSimple *       panelPreView;
PanelSimple *       panelStatus;

PanelScrollText*    panelStdOut;

PanelText*          pCamFilename;
PanelText*          R;
PanelText*          G;
PanelText*          B;
PanelText*          L;
PanelText*          SP;
PanelText*          pArduino;
PanelText*          pStellarium;
PanelText*          pDeplacement;
PanelText*          pAD;
PanelText*          pDC;
PanelText*          pMode;
PanelText*          pAsservi;
PanelText*          pXMax;
PanelText*          pXMin;
PanelText*          pYMax;
PanelText*          pYMin;

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
bool                bAutorisationSuivi = false;
bool                bNuit   = false;
bool                bDebug  = false;
bool                bQuit;


bool                bPanelControl  = true;
bool                bPanelHelp     = false;
bool                bPanelResultat = false;
bool                bPanelCourbe   = false;
bool                bPanelStdOut   = false;
bool                bPanelSerial   = false;
bool                bAfficheVec    = false;
bool                bMouseDeplace  = false;
bool                bRestauration  = false;
bool                bFileBrowser   = false;
bool                bStellarium    = false;
bool                bPleiade       = false;
bool                bSauve         = false;
bool                bOneFrame      = false;

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
float               offset_x;
float               offset_y;
float               delta_courbe1 = 1.0;
float               delta_courbe2 = 1.0;
float               courbe1 = 1.0;
float               courbe2 = 1.0;
int                 decal_resultat = 0;
vec4                colorTraces[] = 
                        {
                        vec4(1.0,0.0,0.0,1.0), vec4(0.0,1.0,0.0,1.0), vec4(0.0,0.0,1.0,1.0),
                        vec4(1.0,1.0,0.0,1.0), vec4(1.0,0.0,1.0,1.0), vec4(0.0,1.0,1.0,1.0),
                        vec4(0.5,0.0,0.0,1.0), vec4(0.0,0.5,0.0,1.0), vec4(0.0,0.0,0.5,1.0),
                        vec4(0.5,0.5,0.0,1.0), vec4(0.5,0.0,0.5,1.0), vec4(0.0,0.5,0.5,1.0)
                        };
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
vec3                vOrigine;
vec3                vTr;
bool                bCorrection = false;
float               fTimeCorrection = 3.0;
float               fTimeCpt = 0.0;
float               err = 2.0;
#define err         err
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
vector<AlertBox*>   tAlert;
bool                bAlert = false;
string              sAlert;

bool                bFindStar = false;
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
void photo()
{
    string filename = " -o /home/rene/Documents/astronomie/logiciel/script/image/atmp/2000-01-01/k200d/test";
    string iso = " -i 100";
    string stime = " -t 1";
    string frames = " -F 2";
    string timeout = " --timeout=3";
    string focus = " -f";
    string command = "/home/rene/Documents/astronomie/logiciel/k200d/pktriggercord-0.84.04/pktriggercord-cli";
    //command = command + filename + iso + stime + timeout;
    command = command + focus + timeout + filename;
    logf( (char*) command.c_str() );
    
    int ret = system( (char*) command.c_str() );
    if ( ret != 0 )
    {
        string mes = "Erreur de commande Pentax Valeur retourne : " +  to_string(ret);
        alertBox(mes);
        logf( (char*)mes.c_str() );
    }
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
void change_file(string dirname, string filename)
{
    Captures::getInstance().change_file(dirname, filename);
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
void glVecAD()
{
    float gris = 0.0;
    if ( var.getb("bNuit") )        glColor4f( 0.5, 0.0, 0.0, 1.0 );
    else                            glColor4f( 1.0, gris, gris, 1.0 );
    
    glBegin(GL_LINES);
        int x = vecAD[0].x;
        int y = vecAD[0].y;
        tex2screen(x, y);
        glVertex2i( x, y );

        x = vecAD[1].x;
        y = vecAD[1].y;
        tex2screen(x, y);
        glVertex2i( x, y );

        glVertex2i( vecAD[1].x, vecAD[1].y );
    glEnd();        
}
void glVecDC()
{
    float gris = 0.0;
    if ( var.getb("bNuit") )        glColor4f( 0.5, 0.0, 0.0, 1.0 );
    else                            glColor4f( gris, 1.0, gris, 1.0 );
    
    glBegin(GL_LINES);
        /*
        vec3 v = vecAD[1] - vecAD[0];
        
        int x = vecAD[0].x;
        int y = vecAD[0].y;
        tex2screen(x, y);
        glVertex2i( x, y );

        x = vecAD[0].x - v.y;
        y = vecAD[0].y + v.x;
        tex2screen(x, y);
        glVertex2i( x, y );
        */
        int x = vecDC[0].x;
        int y = vecDC[0].y;
        tex2screen(x, y);
        glVertex2i( x, y );

        x = vecDC[1].x;
        y = vecDC[1].y;
        tex2screen(x, y);
        glVertex2i( x, y );

        glVertex2i( vecDC[1].x, vecDC[1].y );

    glEnd();        
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void glCercle(int x, int y, int rayon)
{
	glBegin(GL_LINE_LOOP);

        for( float i=0; i<=360.0; i+=1.0 )
        {
            float fx = (float)x+ (float)rayon*cos(DEG2RAD(i));
            float fy = (float)y+ (float)rayon*sin(DEG2RAD(i));
            glVertex2i(fx,fy);
        }
        
    glEnd();        
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void glCarre( int x,  int y,  int dx,  int dy )
{
	glBegin(GL_LINES);
        x = x-dx;
        y = y-dy;
        
        glVertex2i(x,y);                glVertex2i(x+2*dx,y);
        glVertex2i(x+2*dx,y);           glVertex2i(x+2*dx,y+2*dy);
        glVertex2i(x+2*dx,y+2*dy);      glVertex2i(x,y+2*dy);
        glVertex2i(x,y+2*dy);           glVertex2i(x,y);

    glEnd();        
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void glCroix( int x,  int y,  int dx,  int dy )
{
	glBegin(GL_LINES);

	    glVertex2i(x, y-dy);         glVertex2i(x, y+dy);
	    glVertex2i(x-dx, y);         glVertex2i(x+dx, y);

    glEnd();        
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void displayGLTrace(void)
{
    if ( !bAffTrace )              return;

    int nbv = t_vTrace.size();
    
    glBegin(GL_LINES);
        
    int m = sizeof(colorTraces) / 16;
    //logf( (char*)"Modulo : %d", m );

    for( int j=0; j<nbv; j++ )
    {
        
        glColor4fv( (GLfloat*)&colorTraces[j%m] );
            
        vector<vec2> *   trace = t_vTrace[j];
        int nb = (*trace).size();
        if ( nb == 1 )                  continue;
    
        for ( int i=0; i<nb-1; i++ )
        {
	        int x;
	        int y;
	        
	        x = round((*trace)[i].x);
	        y = round((*trace)[i].y);
	        tex2screen(x,y);
            glVertex2i(x,y);

	        x = round((*trace)[i+1].x);
	        y = round((*trace)[i+1].y);
	        tex2screen(x,y);
            glVertex2i(x,y);
        }
    }    
    glEnd();
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
void displayGLCamera_cb(void)
{
    if ( !bAutorisationSuivi )
    {
        if ( var.getb("bNuit") )        glColor4f( 1.0, 0.0, 0.0, 0.2 );
	    else                            glColor4f( 0.0, 1.0, 0.0, 0.2 );
	    
	    int x = xClick;
	    int y = yClick;
	    
	    tex2screen(x,y);

	    glCroix(x, y, 50, 50);
        glCercle(x, y, 25);

    }

    if ( bAfficheVec)
    {
        glVecAD();
        glVecDC();
    }

    float gris = 0.3;
    if ( bNuit )        glColor4f( gris,  0.0,  0.0, 1.0 );
    else                glColor4f( 0.0,   1.0,  0.0, 0.2 );    


    displayGLTrace();

    if ( var.getb("bNuit") )        glColor4f( 1.0, 0.0, 0.0, 1.0 );
    else                            glColor4f( 1.0, 1.0, 1.0, 1.0 );
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void glEchelleAxe( int AXE, int SIZE, float max, float min, PanelText* pMax, PanelText* pMin )
{
    float gris = 0.3;
    vec4 color, colorLimit, colorAxe;
    
    if ( var.getb("bNuit") )
    {
        color       = vec4( 0.15, 0.0, 0.0, 1.0 );    
        colorLimit  = vec4( 0.8, 0.0, 0.0, 1.0 );    
        colorAxe    = vec4( 1.0, 0.0, 0.0, 1.0 );
    }
    else                            
    {
        gris = 0.15;
        color       = vec4( gris, gris, gris, 1.0 );
        colorLimit  = vec4( 1.0, 0.0, 1.0, 1.0 );
        colorAxe    = vec4( 1.0, 1.0, 1.0, 1.0 );
    }
    
    glBegin(GL_LINES);

        //
        // graduation horizontale
        //
        glColor4fv( (GLfloat*)&color );
        int pas = delta_courbe1;
        float fPas = delta_courbe1;
        while( fPas < 10.0 )     fPas += courbe1;

        for( float i=0; i<SIZE/2; i+=fPas )
        {
            int x = xStartAxe;
            int y = AXE + i;
            panelCourbe->x2Screen(x);
            panelCourbe->y2Screen(y);
            glVertex2i( x, y );
            x += panelCourbe->getPosDX();
            glVertex2i( x, y );

            x = xStartAxe;
            y = AXE - i;
            panelCourbe->x2Screen(x);
            panelCourbe->y2Screen(y);
            glVertex2i( x, y );
            x += panelCourbe->getPosDX();
            glVertex2i( x, y );
        }
        //
        // graduation verticale
        //
        fPas = courbe1;
        while( fPas < 10.0 )     fPas += courbe1;
        for( float i=xStartAxe; i<2000; i+=fPas )
        {
            int x = i;
            int y = AXE;
            panelCourbe->x2Screen(x);
            panelCourbe->y2Screen(y);
            y -= SIZE/2;
            glVertex2i( x, y );
            
            y += SIZE;
            glVertex2i( x, y );
        }
        //
        // axe horizontale centrale
        //
        glColor4fv( (GLfloat*)&colorAxe );
        int y = AXE;
        int x = xStartAxe;

        panelCourbe->x2Screen(x);
        panelCourbe->y2Screen(y);

        glVertex2i( x, y );
        x += panelCourbe->getPosDX();
        glVertex2i( x, y );
        //
        // Limite horizontale
        // Min et Max
        //
        glColor4fv( (GLfloat*)&colorLimit );

        y = (float)(delta_courbe1*(min) + AXE);
        if ( pMax != NULL )     pMax->setPos( 5, y-7 );
        panelCourbe->y2Screen(y);
        glVertex2i( xStartAxe, y );
        glVertex2i( x, y );

        y = (float)(delta_courbe1*(max) + AXE);
        if ( pMin != NULL )     pMin->setPos( 5, y-7 );
        panelCourbe->y2Screen(y);
        glVertex2i( xStartAxe, y );
        glVertex2i( x, y );

        glColor4fv( (GLfloat*)&colorAxe );
    glEnd();        
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void glEchelle()
{
    int  AXE_X = (float)panelCourbe->getDY()/4.0;
    int  AXE_Y = (3.0*(float)panelCourbe->getDY()/4.0);
    
    int dy_axe = (float)panelCourbe->getDY() / 2.0;
    
    glEchelleAxe( AXE_X, dy_axe, (float)+err, (float)-err, pXMax, pXMin );
    glEchelleAxe( AXE_Y, dy_axe, (float)+err, (float)-err, pYMax, pYMin );

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void displayResultat_cb(void)
{
    
    int DY = panelCourbe->getY();
    int n = t_vResultat.size();
    
    int  AXE_X = (float)panelCourbe->getDY()/4.0;
    int  AXE_Y   (3.0*(float)panelCourbe->getDY()/4.0);


    if ( bCorrection || bRestauration )
    {
        offset_x = vOrigine.x;
        offset_y = vOrigine.y;
    }
        offset_x = vOrigine.x;
        offset_y = vOrigine.y;

    glEchelle();
    
    if ( decal_resultat >= t_vResultat.size() )     return;
    
    if ( n != 0  )
    {
        if ( var.getb("bNuit") )        glColor4f( 1.0, 0.0, 0.0, 1.0 );
        else                            glColor4f( 0.0, 0.0, 1.0, 1.0 );
        
        glBegin(GL_LINE_STRIP);
        int deb = decal_resultat;
        int fin = deb + 2000.0/courbe1;
        
        if ( fin >= t_vResultat.size() )        fin = t_vResultat.size();
        if ( deb <= 0 )                          deb = 0;
        //int fin = t_vResultat.size()-decal_resultat;
        //logf( 
        //for( int i=decal_resultat; i<(t_vResultat.size()-decal_resultat); i++ )
        for( int i=deb; i<fin; i++ )
        {
            float y = t_vResultat[n-i-1].x - offset_x;
            y = (float)(delta_courbe1*(y) + AXE_X);
            int x = (i-deb)*courbe1 + xStartAxe;

            int Y = y;
            panelCourbe->x2Screen(x);
            panelCourbe->y2Screen(Y);


            glVertex2i( x, Y );
            //logf( "%d %0.2f %0.2f y=%0.2f", n-i-1, t_vResultat[i].x, offset_x, y );
        }
        glEnd();        

        
        if ( var.getb("bNuit") )        glColor4f( 1.0, 0.0, 0.0, 1.0 );
        else                            glColor4f( 1.0, 1.0, 0.0, 1.0 );
        glBegin(GL_LINE_STRIP);
        //for( int i=decal_resultat; i<(t_vResultat.size()-decal_resultat); i++ )
        for( int i=deb; i<fin; i++ )
        {
            float y = t_vResultat[n-i-1].y - offset_y;
            y = (float)(delta_courbe2*(y) + AXE_Y);
            int x = (i-deb)*courbe2 + xStartAxe;

            int Y = y;
            panelCourbe->x2Screen(x);
            panelCourbe->y2Screen(Y);

            glVertex2i( x, Y );
        }
        glEnd();   
        
    }     
    
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
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void screen2tex( int& x, int& y )
{
    x = (float)(x-xCam) * rw;
    y = (float)(y-yCam) * rh;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void tex2screen( int& x, int& y )
{
    x = (float)x / rw + xCam;
    y = (float)y / rh + yCam;
}
#include <malloc.h>
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
float getSkyPoint_colorR(int offset)
{
    float r;

    try
    {
        r = ptr[offset+0]; 
    }
    catch ( const std::exception& e )
    {
        std::cout << e.what() << std::endl;
        return -1;
    }
    return r;
}
float getSkyPoint_colorG(int offset)
{
    float r;

    try
    {
        r = ptr[offset+1]; 
    }
    catch ( const std::exception& e )
    {
        std::cout << e.what() << std::endl;
        return -1;
    }
    return r;
}
float getSkyPoint_colorB(int offset)
{
    float r;

    try
    {
        r = ptr[offset+2]; 
    }
    catch ( const std::exception& e )
    {
        std::cout << e.what() << std::endl;
        return -1;
    }
    return r;
}
float getSkyPoint_colorL(int offset)
{
    float r;
    float g;
    float b;

    try
    {
        r = ptr[offset+0]; 
        g = ptr[offset+1]; 
        b = ptr[offset+2]; 
    }
    catch ( const std::exception& e )
    {
        std::cout << e.what() << std::endl;
        return -1;
    }
    return 0.33 * r + 0.5 * g  + 0.16 * b;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void updatePanelResultat(float x, float y, float mag)
{
    int xPanel = x;
    int yPanel = y;
    
    //mag = -1.0;

    tex2screen(xPanel,yPanel);
    panelResultat->setPos(xPanel+20 , yPanel+20);

    
    char sSkyPoint[255];
    char sR[255];
    char sG[255];
    char sB[255];
    char sL[255];
    sprintf( sSkyPoint, "(%0.2f,%0.2f) / (%0.2f,%0.2f)  mag=%0.2f", x, y, vOrigine.x, vOrigine.y, mag );
    
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


}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void reshapeGL(int newWidth, int newHeight)
{
    logf((char*) "-------- reshapeGL(%d, %d)", newWidth, newHeight);

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
void change_ad(float ad)
{
    ad = DEG2RAD(ad);
    struct hms HMS;
    rad2hms( HMS, ad);
    
    char    buff[255];
    sprintf( buff, "AsDr : %02d:%02d:%0.2f", (int)HMS.h, (int)HMS.m, HMS.s );
    
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
    sprintf( buff, "Decl : %02d:%02d:%0.2f", (int)DMS.d, (int)DMS.m, DMS.s );
    
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
void sauve(void)
{
    string filename = "/home/rene/.astropilot/sauvegarde.txt";
    logf( (char*)"Sauvegarde des valeurs dans '%s'", (char*)filename.c_str() );
    
    std::ofstream fichier;
    fichier.open(filename, std::ios_base::app);

    if ( !fichier ) 
    {
        logf( (char*)"[ERROR]impossble d'ouvrir : '%s'", (char*)filename.c_str() );
    }

    for(int i=0; i<t_vSauve.size(); i++)
    {
        fichier << "( " << t_vSauve[i].x << " , " <<  t_vSauve[i].y << " ) / ";
        fichier << "( " << vOrigine.x << " , " <<  vOrigine.y << " )\n";
    }

    fichier.close();
    
    t_vSauve.clear();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void charge_fichier(void)
{
    //string filename = "/home/rene/.astropilot/svg.txt";
    //string filename = "/home/rene/.astropilot/m57.txt";
    string filename = "/home/rene/.astropilot/sauvegarde.txt";
    logf( (char*)"Chargement des valeurs dans '%s'", (char*)filename.c_str() );
    
    std::ifstream fichier;
    fichier.open(filename, std::ios_base::app);

    if ( !fichier ) 
    {
        logf( (char*)"[ERROR]impossble d'ouvrir : '%s'", (char*)filename.c_str() );
    }

    
    vOrigine.x = 0.0;
    vOrigine.y = 0.0;
    
    t_vResultat.clear();
    

    string line;
    int nbLigne = 0;
    
    
    while ( getline (fichier, line) )
    {
        //cout << line << '\n';
        float rx, ry, ox, oy;

        //logf( (char*) line.c_str() );
        sscanf( line.c_str(), "( %f , %f ) / ( %f , %f )", &rx, &ry, &ox, &oy );

        vOrigine.x = ox;
        vOrigine.y = oy;

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
    fichier.open(filename, std::ios_base::app);

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
void suivi(void)
{
    if ( Camera_mgr::getInstance().haveNewFrame() )
    {
        Camera_mgr::getInstance().haveUseFrame(false);
    }
    else
        return;
    
    /*
    //Camera_mgr::getInstance().suivi();
    //change_background_pleiade();
    getSuiviParameter();   
    
    //char   sSkyPoint[100];
    
    struct sky_point point;
    point.xAverage = 0.0;
    point.yAverage = 0.0;
    point.ponderation = 0.0;
    
    //printf( "1-Suivi x=%0.2f, y=%0.2f\n", xSuivi, ySuivi);

    //getSkyPoint(&point, (int)xSuivi-xCam, (int)ySuivi-yCam, SIZEPT);
    getSkyPoint(&point, (int)xSuivi, (int)ySuivi, SIZEPT);
    
    float xx;
    float yy;

    if ( point.ponderation > 0.1 ) {

        xx = point.xAverage / point.ponderation;
        yy = point.yAverage / point.ponderation;
        
        //float mag = -(log(point.ponderation ) / log(2.0)) + 17.0;
        //float mag = point.ponderation;

        //sprintf( sSkyPoint, "(%0.2f,%0.2f) / (%0.2f,%0.2f)  mag=%0.2f", xx, yy, vOrigine.x, vOrigine.y, pond2mag(point.ponderation) );
        //SP->changeText(sSkyPoint);
        //panelResultat->setVisible(true);

        xSuivi = xx;
        ySuivi = yy;
        //printf( "2-Suivi x=%0.2f, y=%0.2f\n", xSuivi, ySuivi);
    }
    else 
    {
        logf( (char*)"Perte suivi !!!" );
        rechercheSkyPoint( xSuivi, ySuivi);
    }
    */


    vec2*       pV = Camera_mgr::getInstance().getSuivi();
    if ( pV == NULL )
    {
        //logf( (char*)"Suivi NULL");
        return;
    }
    else
    {
        //logf( (char*)"Suivi (%0.2f, %0.2f)", pV->x, pV->y ); 

        float xx = pV->x;
        float yy = pV->y;

        xSuivi = xx;
        ySuivi = yy;
        

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
        
        updatePanelResultat( xSuivi, ySuivi, 0.0 );
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void getSuiviParameter(void)
{
    Camera_mgr&  cam_mgr = Camera_mgr::getInstance();
    //cam_mgr.active();

    xCam = cam_mgr.get_xCam();
    yCam = cam_mgr.get_yCam();
    dxCam = cam_mgr.get_dxCam();
    dyCam = cam_mgr.get_dyCam();

    vCameraSize.x = 1920.0;
    vCameraSize.y = 1080;
    vCameraSize = cam_mgr.get_vCameraSize();

    //rw = (float)vCameraSize.x/(float)panelPreView->getDX();
    //rh = (float)vCameraSize.y/(float)panelPreView->getDY();
    rw = (float)vCameraSize.x/(float)dxCam;
    rh = (float)vCameraSize.y/(float)dyCam;

    /* 
    logf( (char*)"-------------------------------------" );
    logf( (char*)"width=%d height=%d" , width, height );
    logf( (char*)"xCam=%d yCam=%d dxCam=%d dyCam=%d", xCam, yCam, dxCam, dyCam );
    logf( (char*)"rw=%0.2f rh=%0.2f" , rw, rh );
    */
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
    //-----------------------------------------------------------------------
    // Gestion de la sauvegarde des coordonnees
    // ecran des differentes fenetres
    //-----------------------------------------------------------------------
    if ( panelStdOutW->getHaveMove() )
    {
        panelStdOutW->resetHaveMove();

        var.set("xPanelStdOut",  panelStdOutW->getX() );
        var.set("yPanelStdOut",  panelStdOutW->getY() );
        var.set("dxPanelStdOut", panelStdOutW->getDX() );
        var.set("dyPanelStdOut", panelStdOutW->getDY() );
        
        //if ( panelStdOutW->getX() != 0 )    alertBox("xPanelStdOut != 0");
    }

    if ( panelHelp->getHaveMove() )
    {
        panelHelp->resetHaveMove();

        var.set("xPanelHelp",  panelHelp->getX() );
        var.set("yPanelHelp",  panelHelp->getY() );
        var.set("dxPanelHelp", panelHelp->getDX() );
        var.set("dyPanelHelp", panelHelp->getDY() );
        
        //if ( panelStdOutW->getX() != 0 )    alertBox("xPanelStdOut != 0");
    }

    if ( panelCourbe->getHaveMove() )
    {
        panelCourbe->resetHaveMove();

        var.set("xPanelCourbe",  panelCourbe->getX() );
        var.set("yPanelCourbe",  panelCourbe->getY() );
        var.set("dxPanelCourbe", panelCourbe->getDX() );
        var.set("dyPanelCourbe", panelCourbe->getDY() );
        
        //if ( panelStdOutW->getX() != 0 )    alertBox("xPanelStdOut != 0");
    }
    //-----------------------------------------------------------------------
    // Mise a jour des buffers de la camera
    // et des pointeurs vers la texture background
    //-----------------------------------------------------------------------
    if (!bPause || bOneFrame )    {
        Camera_mgr::getInstance().change_background_camera();
        if (bOneFrame)      { updatePanelPause(true); }
        bOneFrame = false;
    }
    Camera_mgr::getInstance().update();
    
    Captures::getInstance().update();
    //-----------------------------------------------------------------------
    // Lance le suivi des etoiles
    //-----------------------------------------------------------------------
    if (bAutorisationSuivi)
    {
        suivi();
    }
    //-----------------------------------------------------------------------
    // Geqtion du suivi des eroiles
    //-----------------------------------------------------------------------
    if (!bAutorisationSuivi)
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
            trace->push_back( vec2(xSuivi, ySuivi) );
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
    
    WindowsManager::getInstance().idleGL( elapsedTime );
    
	if ( elapsedTime != -1 )
	{
	    fTimer += elapsedTime;
	    if ( fTimer >= 10.0 )
	    {
	        fTimer -= 10.0;
            
            char cmd[255];
            sprintf( cmd, "g" );
            Serial::getInstance().write_string(cmd);
	    }
	    //logf( (char*)"%0.2f", fTimer );
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
	    if ( bCorrection && fTimeCpt > 0.0 && bAutorisationSuivi)
	    {
            fTimeCpt -= fTimeCorrection;
            if ( fTimeCpt > 0.0 )    fTimeCpt = 0.0;//-= fTimeCorrection;

	        float dx = xSuivi - vOrigine.x;
	        float dy = ySuivi - vOrigine.y;
	        if ( fabs(dx) > err || fabs(dy) > err )
	        {
	            vec3 w = vec3( xSuivi, ySuivi, 0.0);
	            vec3 v = vOrigine - w;

                vec3 res = mChange * v;
                int ad = (int) (res.x * -1000.0);
                int dc = (int) (res.y * 1000.0);
                char cmd[255];
                sprintf( cmd, "a%dp;d%dp", ad, dc );
                logf( (char*)" main.cpp (Asservissement=>) '%s'",  cmd );
                Serial::getInstance().write_string(cmd);
                //fTimeCpt += 1.0;
	        }
	    }
	}
    //------------------------------------------------------
    //------------------------------------------------------
    WindowsManager::getInstance().onTop(panelStatus);
    PanelConsoleSerial::getInstance().idleGL();
    WindowsManager::getInstance().idleGL( elapsedTime );
    

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
    Panel*  pFocus = WindowsManager::getInstance().getFocus();

    if ( pFocus == panelHelp)       pFocus->setVisible(!pFocus->getVisible());
    //if ( pFocus == panelControl)    pFocus->setVisible(!pFocus->getVisible());
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
            logf( (char*)"-------------- Touche CTRL+TAB" );
	        Camera* pCurrent = cam_mgr.getCurrent();
	        cam_mgr.active();

            getSuiviParameter();
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
    
    if (tAlert.size() != 0 )
    {
        if ( bQuit )   
        {
            if (key == 27 )     { quit(); return; }
        }
        bQuit = false;
        alertBoxQuit();
        return;
    }
    
    if ( PanelConsoleSerial::getInstance().keyboard(key, x, y) )      return;;

    if ( bFileBrowser )
    {
        FileBrowser::getInstance().keyboard( key, x, y);
        return;
    }

    Camera_mgr&  cam_mgr = Camera_mgr::getInstance();
    if (        cam_mgr.getCurrent() 
            &&  cam_mgr.getCurrent()->getControlVisible()       )
    {
        if ( cam_mgr.getCurrent()->keyboard(key) )      return;
    }
    /*
	if (modifier == GLUT_ACTIVE_CTRL)
	{
        logf( (char*)" Touche CTRL %d", (int)key );
        glutKeyboardFuncCtrl(key,  x,  y);
        return;
	}
	*/
	if (modifier == GLUT_ACTIVE_ALT)
	{
        logf( (char*)" Touche ALT %c", key );
        glutKeyboardFuncCtrl(key,  x,  y);
        return;
	}

	
	switch(key){ 
	
	// CTRL q
	case 17:
	    {
	    quit();
	    }
        break;

	// touche escape
	case 27:
	    {
	    alertBox( "Confirmez la sortie du programme 'ESC'" );
	    bQuit = true;
	    }
        break;


    case 4:
		{
            logf( (char*)"case 'ctrl+D'" );
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
    
    case 16:
		{
            logf( (char*)"case 'ctrl+P'" );
        }
        break;
    
    case 6:
		{
            logf( (char*)"case 'ctrl+F'" );
            /*
            if ( isMouseOverCapture()  )
            {
                int n = current_capture;
                if ( n != -1 )
                {
                    captures[n]->fullscreen();
                }
            }
            */
            Captures::getInstance().fullscreen();
        }
        break;
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    // touche tab
	case 9:
		//WindowsManager::getInstance().swapVisible();
		{
        	if (modifier == GLUT_ACTIVE_CTRL)
        	{
	            logf( (char*)"-------------- Touche CTRL+TAB" );
		        Camera_mgr&  cam_mgr = Camera_mgr::getInstance();
		        cam_mgr.active();

                getSuiviParameter();
                break;
        	}
        	else if (modifier == GLUT_ACTIVE_SHIFT)
        	{
	            logf( (char*)"-------------- Touche SHIFT+TAB" );
    	        Captures::getInstance().rotate_capture_moins(false);
                break;
        	}
        	else if (modifier == GLUT_ACTIVE_ALT)
        	{
	            logf( (char*)"-------------- Touche ALT+TAB" );
                break;
        	}

            logf( (char*)"-------------- Touche TAB" );
	        Captures::getInstance().rotate_capture_plus(false);
        }
		break;
	case 10:
		break;
	case 13:
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
    case 'q':
    	{
	        Captures::getInstance().rotate_capture_plus(true);
    	}
	    break;
    
    case 'Q':
    	{
	        Captures::getInstance().rotate_capture_moins(true);
    	}
	    break;
    
	case 't':
		{;
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
    case 'o':
        {
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
    	bAutorisationSuivi = !bAutorisationSuivi;
        var.set("bAutorisationSuivi", bAutorisationSuivi);

        if (bAutorisationSuivi)         pMode->changeText((char*)"Mode suivi");
        else                            pMode->changeText((char*)"Mode souris");
        }
    	break;

    case 'l':
        {            
        Connexion_mgr::getInstance().print_list();
        Camera_mgr::getInstance().print_list();
        Captures::getInstance().print_list();
        }
        break;

    case 'L':
        {            
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
             }
        }
            
        }
        break;

    case 'f':  // '-'
        {
        bFileBrowser = FileBrowser::getInstance().getVisible();
        bFileBrowser = !bFileBrowser;
        
        if ( bFileBrowser )         FileBrowser::getInstance().affiche();
        else                        FileBrowser::getInstance().cache();
        }
        break;


    case 'v':  // '-'
        {
        bSauve = !bSauve;
        if ( bSauve )       logf( (char*)"Sauvegarde !!!" );
        else                logf( (char*)"Arret sauvegarde !!!" );
        }
        break;
    case 'V':  // '-'
        {
        if (!bAutorisationSuivi)
        {
            int x = xClick;
            int y = yClick;
            tex2screen(x,y);

            vOrigine.x = x;
            vOrigine.y = y;
            vOrigine.z = 0.0;
        }
        else
        {
            vOrigine.x = xSuivi;
            vOrigine.y = ySuivi;
            vOrigine.z = 0.0;
        }
        }
        break;
    case 'p':  // '-'
        {
        updatePanelPause(!bPause);
        }
        break;
    case 'P':  // '-'
        {
        //photo();
        bOneFrame = true;
        }
        break;

/*
    case '0':
        {
        Camera_mgr::getInstance().sup("video1");
        log( (char*)"Sup camera !!!" );
        }
        break;
*/
    case 'A':
        {
        if ( bAutorisationSuivi )
        {
            vecAD[1].x = xSuivi;
            vecAD[1].y = ySuivi;
            vecAD[1].z = 0.0;
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
        if ( bAutorisationSuivi )
        {
            vecAD[0].x = xSuivi;
            vecAD[0].y = ySuivi;
            vecAD[0].z = 0.0;
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

    case 'c':
        {
            bRecTrace = !bRecTrace;
            if ( bRecTrace )
            {
                logf( (char*)"Sauvegarde ...");
                t_vTrace.push_back( new (vector<vec2>)() );
            }
        }
        break;
    case 'C':
        {
            bAffTrace = !bAffTrace;
        }
        break;
    case 'e':
        {
            int i = t_vTrace.size();
            if ( i == 0 )       break;
            
            vector<vec2>* trace = t_vTrace[i-1];
            trace->clear();
            delete trace;
            t_vTrace.pop_back();
            trace = 0;
        }
        break;
    case 'E':
        {
            sauve_traces();
        }
        break;
    case 'z':
        {
            charge_traces();
        }
        break;
    case 'Z':
        {
            bFindStar = !bFindStar; 
        }
        break;
    case 's':
        {
            logf( (char*)"case 's'" );
            
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
            bSuivi = !bSuivi;
            var.set( "bSuivi", bSuivi );
        }
        break;
    case 'D':
        {
        if ( bAutorisationSuivi )
        {
            vecDC[1].x = xSuivi;
            vecDC[1].y = ySuivi;
            vecDC[1].z = 0.0;
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
        
        bMouseDeplace = true;
        }
        break;
    case 'd':
        {
        
        if ( bAutorisationSuivi )
        {
            vecDC[0].x = xSuivi;
            vecDC[0].y = ySuivi;
            vecDC[0].z = 0.0;
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

    case 'M':
        {
        compute_matrix();
        }
        break;
    case 'm':
        {
        bMouseDeplace = !bMouseDeplace;
        if ( bMouseDeplace )
        {
            logf( (char*)"Autorisation du deplcament par la souris (bouton du milieu) ...");
        }
	    if (bMouseDeplace)              pDeplacement->changeText((char*)"Deplacement");
        else                            pDeplacement->changeText((char*)"----");
        }
        break;
    case 'y':
        {
        bAfficheVec = !bAfficheVec;
        var.set("bAfficheVec", bAfficheVec);
        }
        break;
    case 'U':
        {
        courbe1 *= 0.8;
        var.set("courbe1", courbe1);
        courbe2 *= 0.8;
        var.set("courbe2", courbe2);
        }
        break;
    case 'u':
        {
        courbe1 /= 0.8;
        var.set("courbe1", courbe1);
        courbe2 /= 0.8;
        var.set("courbe2", courbe2);
        }
        break;
    case 'J':
        {
        delta_courbe1 *= 0.8;
        var.set("delta_courbe1", delta_courbe1);
        delta_courbe2 *= 0.8;
        var.set("delta_courbe2", delta_courbe2);
        }
        break;
    case 'j':
        {
        delta_courbe1 /= 0.8;
        var.set("delta_courbe1", delta_courbe1);
        delta_courbe2 /= 0.8;
        var.set("delta_courbe2", delta_courbe2);
        }
        break;
    case 'k':
        {
            decal_resultat ++;
            if ( decal_resultat >= t_vResultat.size() )            decal_resultat --;
        }
        break;
    case 'K':
        {
            decal_resultat --;
            if ( decal_resultat < 0 )            decal_resultat = 0;
        }
        break;
    case 'i':
        {
            decal_resultat += 100;
            if ( decal_resultat >= t_vResultat.size() )            decal_resultat -= 100;
        }
        break;
    case 'I':
        {
            decal_resultat -= 100;
            if ( decal_resultat < 0 )            decal_resultat = 0;
        }
        break;
    case 'h':
        {
        Camera* p = Camera_mgr::getInstance().getCurrent();
        if ( p!=NULL )
        {
            p->enregistre();
    	    logf( (char*)"Enregistre  une photo de la camera courante !!" );
        }
        }
        break;
    case 'Y':
        {
        bCorrection = !bCorrection; 
        var.set( "bCorrection", bCorrection);

        fTimeCpt = 0.0; 

        /*
        vOrigine.x = xSuivi;
        vOrigine.y = ySuivi;
        vOrigine.z = 0.0;
        */
        var.set("vOrigine.x", vOrigine.x);
        var.set("vOrigine.y", vOrigine.y);

        if ( bCorrection )      pAsservi->changeText((char*)"Asservissemnent");
        else                    pAsservi->changeText((char*)"");
        }
        break;
    case 'N':
        {
        bNuit = !bNuit;
        var.set("bNuit", bNuit);
        
        if (bNuit)  PanelConsoleSerial::getInstance().getConsole()->setColor(0xFFFF0000);
        else        PanelConsoleSerial::getInstance().getConsole()->setColor(0xFFFFFFFF);

        if (bNuit)  panelStdOut->setColor(0xFFFF0000);
        else        panelStdOut->setColor(0xFFFFFFFF);

        //if (bNuit)  panelHelp->setColor(0xFFFF0000);
        //else        panelHelp->setColor(0xFFFFFFFF);
        }
        break;
    case 'r' :
        {
        bRestauration = !bRestauration;
        if ( bRestauration )    charge_fichier();
        }
        break;
    case 'R' :
        {
        bAlert = !bAlert;
        alertBox( "TEST ALERT BOX" );
        }
        break;
    default:
        {
        logf((char*)"key: %d", key);
        }
        break;
	}
	
	//Camera_mgr::getInstance().keyboard( key );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void glutKeyboardUpFunc(unsigned char key, int x, int y)	{
	WindowsManager::getInstance().keyboardUpFunc( key, x, y);
    //WindowsManager::getInstance().onBottom(panelPreView);
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
        if ( bPanelCourbe )       WindowsManager::getInstance().onTop(panelCourbe);
        log( (char*)"Toggle panelCourbe !!!" );
        }
        break;
    case GLUT_KEY_F5:
        {
        bPanelStdOut = !bPanelStdOut;
        var.set("bPanelStdOut", bPanelStdOut);
        panelStdOutW->setVisible(bPanelStdOut);
        if ( bPanelStdOut )       WindowsManager::getInstance().onTop(panelStdOutW);
        log( (char*)"Toggle panelStdOut !!!" );
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
    case GLUT_KEY_F11:
        {
        Captures::getInstance().ajoute();
        }
        break;
    case GLUT_KEY_F10:
        {
        bDebug = !bDebug;
        WindowsManager::getInstance().debug(bDebug);
        }
        break;
    case GLUT_KEY_F12:
        {
        Captures::getInstance().supprime();
        }
        break;
	default:	
	    {
        logf( (char*)"glutSpecialFunc %d", key );

		}
		break;
		
    }



	WindowsManager::getInstance().keyboardSpecialFunc( key, x, y);
    //WindowsManager::getInstance().onBottom(panelPreView);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void glutSpecialUpFunc(int key, int x, int y)	{
	WindowsManager::getInstance().keyboardSpecialUpFunc( key, x, y);
    //WindowsManager::getInstance().onBottom(panelPreView);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void glutMouseFunc(int button, int state, int x, int y)	{
    mouse.x = x;
    mouse.y = y;
    WindowsManager& wm = WindowsManager::getInstance();
	wm.mouseFunc(button, state, x, y);
    //WindowsManager::getInstance().onBottom(panelPreView);
    if ( panelStatus->isMouseOver(x, y) )
    {
        logf( (char*)"Souris sur la barre de status" );
        return;
    }

    PanelWindow*    pPreviewCam = NULL;
    Panel *         pFocus = NULL;
    Panel*          pMouseOver = NULL;

    Camera_mgr& mgr = Camera_mgr::getInstance(); 
    if ( mgr.getCurrent() != NULL  &&  mgr.getCurrent()->getPanelPreview() != NULL )
    {
        pPreviewCam = mgr.getCurrent()->getPanelPreview();
        pFocus      = wm.getFocus();
        pMouseOver  = wm.findPanelMouseOver(x, y);
    }
    
	if ( bMouseDeplace && button == GLUT_MIDDLE_BUTTON && state == 0 )
	{
        getSuiviParameter();

        Camera_mgr::getInstance().onBottom();
        
	    int X = x;
	    int Y = y;
	    
	    screen2tex(X,Y);
	    
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

        Camera_mgr::getInstance().onBottom();
        
	    int X = x;
	    int Y = y;
	    
	    screen2tex(X,Y);
	    
	    xClick = X;
	    yClick = Y;

        vDepl[1].x = xClick;
        vDepl[1].y = yClick;
        vDepl[1].z = 0.0;
        logf( (char*)"vDepl[1](%0.2f, %0.2f)",  vDepl[1].x, vDepl[1].y );

	    logf( (char*)"state = 1" );

        vec3 v = vDepl[1] - vDepl[0];
        vTr = mChange * v;
        int ad = (int) (vTr.x * -1000.0);
        int dc = (int) (vTr.y * 1000.0);
        char cmd[255];
        sprintf( cmd, "a%dp;d%dp", ad, dc );
        //logf( (char*)"Envoi de la commande",  cmd );
        Serial::getInstance().write_string(cmd);
	}

	//if ( bPause && button == 0 && state == 0 )	{
    if ( !bAutorisationSuivi && button == 0 && state == 0 )	{
        getSuiviParameter();

        Camera_mgr::getInstance().onBottom();
        
	    int X = x;
	    int Y = y;
	    
	    screen2tex(X,Y);
	    
	    xClick = X;
	    yClick = Y;

	    logf( (char*)"Click x=%d y=%d  X=%d Y=%d vCameraSize.x=%d" , x, y, X, Y, vCameraSize.x );
	    
	} 
    else
	if ( bAutorisationSuivi && button == 0 && state == 0 && pMouseOver == pPreviewCam )	{

	    logf( (char*)"xSuivi=%0.2f ySuivi=%0.2f   " , xSuivi, ySuivi );
	    //logf( (char*)"    l=%d rgb=%d,%d,%d" , r,g,b,  l );

	} 

    Camera_mgr::getInstance().onBottom();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void glutMotionFunc(int x, int y)	{	
    mouse.x = x;
    mouse.y = y;
	WindowsManager::getInstance().motionFunc(x, y);
    //WindowsManager::getInstance().onBottom(panelPreView);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void glutPassiveMotionFunc(int x, int y)	{
    mouse.x = x;
    mouse.y = y;
	WindowsManager::getInstance().passiveMotionFunc(x, y);
    //WindowsManager::getInstance().onBottom(panelPreView);
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
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void update_err()
{
    char s[20];
    sprintf( s, "+%0.2f", err );
    pXMax->changeText( (char*)s );
    pYMax->changeText( (char*)s );

    sprintf( s, "-%0.2f", err );
    pXMin->changeText( (char*)s );
    pYMin->changeText( (char*)s );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void CreateCourbe()	{
	WindowsManager& wm = WindowsManager::getInstance();

    panelCourbe = new PanelWindow();
    panelCourbe->setDisplayGL(displayGLnuit_cb);
    resizeCourbe( width, height );

    wm.add(panelCourbe);
    
    panelCourbe->setVisible(bPanelCourbe);
 	panelCourbe->setBackground((char*)"images/background.tga");
    panelCourbe->setDisplayGL( displayResultat_cb );

    pXMax = new PanelText( (char*)"+err",		PanelText::NORMAL_FONT, 5, 50 );
	panelCourbe->add( pXMax );

    pXMin = new PanelText( (char*)"-err",		PanelText::NORMAL_FONT, 5, 60 );
	panelCourbe->add( pXMin );

    pYMax = new PanelText( (char*)"+err",		PanelText::NORMAL_FONT, 5, 70 );
	panelCourbe->add( pYMax );

    pYMin = new PanelText( (char*)"-err",		PanelText::NORMAL_FONT, 5, 80 );
	panelCourbe->add( pYMin );

    update_err();
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
	panelHelp->add( new PanelText( (char*)s.c_str(),  		PanelText::NORMAL_FONT, x_help, y_help ) );
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

	
	addString( "   --- Touche de fonction  DU LOGICIEL ---" );
	addString( "Brightness      B/b" );
	addString( "Contrast        C/c" );
	addString( "Saturation      S/s" );
	addString( "Hue             H/h" );
	addString( "Gamma           G/g" );
	addString( "Sharpness       Z/z" );
	addString( "Exposure        E/e" );
	addString( "White balance   W/w" );
	
	addString( "" );
	addString( "ctrl+TAB: camera suivante" );
	addString( "o: Ouvre/Ferme la fenetre pleiades");
	addString( "p: Pause de l'affichage de pleiades");
	addString( "" );
	addString( "f:   Ouvrir un fichier image");
	addString( "TAB: Change l'affichage des fichiers" );
	addString( "q:   Toutes les images sont affichees en icones");
	addString( "7:   Charge la prochaine image");
	addString( "8:   Efface la derniere image");

	addString( "");
	addString( "a/A: Vecteur en ascension droite");
	addString( "  a: Debut");
	addString( "  A: Fin");
	addString( "d/D: Vecteur en declinaison");
	addString( "  d: Debut");
	addString( "  D: Fin");
	addString( "M  : Calcul la matrice de transformation");
	addString( "y  : Affiche les vecteurs");
	addString( "m  : Deplacement à la souris");
	addString( "O  : Mode souris / mode suivi");

	addString( "");
	addString( "c: Lance/arrete l\'enregistrement de trace");
	addString( "C: Affiche les traces");
	addString( "e: Efface la derniere trace");
	addString( "E: Sauvegarde les traces");
	addString( "z: Charge les traces");
	addString( "");
	addString( "h: Enregistre une image de la camera courante");
	addString( "u/U: Echelle en x sur les courbes");
	addString( "j/J: Echelle en y sur les courbes");
	addString( "k/K: Decalage sur les courbes de 1");
	addString( "i/I: Decalage sur les courbes de 100");

	addString( "r: Rappel des mesures de decalage en x,y du fichier beltegeuse.txt");
	addString( "R: Test alert BOX");
	addString( "l: List les ports /dev/ +  les controles ");
	addString( "L: List les variables");
	
	addString( "");
	addString( "S: Stop le suivi");
	addString( "t/T: change le temps de correction");
	addString( "Y: Lance l' asservissement");
	addString( "V: Initialise les coordonnees de suivi");
	addString( "v: Sauvegarde des coordonnees dans le fichier .astropilot/sauvegarde.text");
	addString( "ESC: --- SORTIE DU LOGICIEL ---" );

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

    pStellarium = new PanelText( (char*)"----",		PanelText::NORMAL_FONT, width-230, 2 );
	panelStatus->add( pStellarium );

    pAD = new PanelText( (char*)"AsDr :",		PanelText::NORMAL_FONT, 80, 2 );
	panelStatus->add( pAD );
    pDC = new PanelText( (char*)"Decl :",		PanelText::NORMAL_FONT, 200, 2 );
	panelStatus->add( pDC );

    pMode = new PanelText( (char*)" ",		PanelText::NORMAL_FONT, 350, 2 );
	panelStatus->add( pMode );
 
    pAsservi = new PanelText( (char*)" ",		PanelText::NORMAL_FONT, 650, 2 );
	panelStatus->add( pAsservi );

    pDeplacement = new PanelText( (char*)" ",		PanelText::NORMAL_FONT, width-410, 2 );
	panelStatus->add( pDeplacement );


	if (bMouseDeplace)              pDeplacement->changeText((char*)"Deplacement");
    else                            pDeplacement->changeText((char*)"----");

	if (bAutorisationSuivi)         pMode->changeText((char*)"Mode suivi");
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
	WindowsManager& wm = WindowsManager::getInstance();

	int x=10;
	int y=50;
	int dx=350;
	int dy=400;

    x = var.geti("xPanelStdOut");
    y = var.geti("yPanelStdOut");
    dx = var.geti("dxPanelStdOut");
    dy = var.geti("dyPanelStdOut");

    if ( x<= 0 )        x = 10;
    if ( dx<= 100 )     dx = 350;
    if ( y<= 0 )        y = 10;
    if ( dy<= 100 )     dy = 400;

	panelStdOutW = new PanelWindow();
    panelStdOutW->setDisplayGL(displayGLnuit_cb);

	panelStdOut = new PanelScrollText(dy/13,50);
	panelStdOut->setPrompt(string(">"));

	panelStdOutW->setPosAndSize( x, y, dx, dy );
 	panelStdOutW->setVisible(bPanelStdOut);

	panelStdOut->setPosAndSize( 0, 0, dx, dy );


 	wm.add( panelStdOutW );
 	panelStdOutW->add( panelStdOut );

 	panelStdOut->setBackground((char*)"images/background.tga");
 	panelStdOut->setTabSize(20);

    string st = string("Bonjour\n");
       
    logf((char*)"** CreateStdOut()  panelStdOut  %d,%d %dx%d", x, y, dx, dy);
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
void log( char* chaine )
{
    if ( panelStdOut )          panelStdOut->affiche( chaine );
    
    printf( "log : %s\n", chaine);
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
    
    if (!bOK)           cout <<"Pas de camera !!!" << endl;

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
        printf("\tScreen %d: %dX%d\n", i + 1, screen->width, screen->height);
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

    var.set( "bAutorisationSuivi", bAutorisationSuivi );
    var.set( "bSuivi", bSuivi );
    var.set( "bPanelControl", bPanelControl );
    var.set( "bPanelHelp", bPanelHelp );
    var.set( "bPanelResultat", bPanelResultat );
    var.set( "bPanelCourbe", bPanelCourbe );
    var.set( "bPanelStdOut", bPanelStdOut );
    var.set( "bPanelSerial", bPanelSerial );
    var.set( "bAfficheVec", bAfficheVec);
    var.set( "bCorrection", bCorrection);

    var.set( "err", err );

    var.set("bPause", bPause);
    var.set("bFull", bFull);
    var.set("bNuit", bNuit);

    var.set("courbe1", courbe1);
    var.set("delta_courbe1", delta_courbe1);
    var.set("courbe2", courbe2);
    var.set("delta_courbe2", delta_courbe2);

    var.set("vecAD[0].x", vecAD[0].x);
    var.set("vecAD[0].y", vecAD[0].y);
    var.set("vecAD[1].x", vecAD[1].x);
    var.set("vecAD[1].y", vecAD[1].y);

    var.set("vecDC[0].x", vecDC[0].x);
    var.set("vecDC[0].y", vecDC[0].y);
    var.set("vecDC[1].x", vecDC[1].x);
    var.set("vecDC[1].y", vecDC[1].y);

    var.set("vOrigine.x", vOrigine.x);
    var.set("vOrigine.y", vOrigine.y);

    var.set("xSuivi", xSuivi);
    var.set("ySuivi", ySuivi);
    
    /*
    var.set("xPanelStdOut",  panelStdOutW->getX() );
    var.set("yPanelStdOut",  panelStdOutW->getY() );
    var.set("dxPanelStdOut", panelStdOutW->getDX() );
    var.set("dyPanelStdOut", panelStdOutW->getDY() );
    */
    //if ( panelStdOutW->getX() == 0 )    bAlert = true;

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void charge_var()
{
    var.charge();

    bAutorisationSuivi  = var.getb( "bAutorisationSuivi" );
    //bSuivi              = var.getb( "bSuivi" );

    bPanelCourbe        = var.getb( "bPanelCourbe" );
    if ( panelCourbe )      panelCourbe->setVisible(bPanelCourbe);
    
    bPanelHelp          = var.getb( "bPanelHelp" );
    if (panelHelp)      panelHelp->setVisible( bPanelHelp );
    
    bPanelResultat      = var.getb( "bPanelResultat" );
    if (panelResultat)  panelResultat->setVisible( bPanelResultat );
    
    bPanelStdOut        = var.getb( "bPanelStdOut" );
    if (panelStdOutW)   panelStdOutW->setVisible( bPanelStdOut );
    
    
    bPanelSerial        = var.getb( "bPanelSerial" );
    PanelConsoleSerial::getInstance().setVisible( bPanelSerial );
    
    bAfficheVec         = var.getb("bAfficheVec");

    bPause              = var.getb("bPause");
    bFull               = var.getb("bFull");
    if ( bFull )        glutFullScreen();

    bCorrection         = var.getb("bCorrection");
    bNuit               = var.getb("bNuit");


    err                 = var.getf("err");
    //update_err();
    
    courbe1             = var.getf("courbe1");
    delta_courbe1       = var.getf("delta_courbe1");
    courbe2             = var.getf("courbe2");
    delta_courbe2       = var.getf("delta_courbe2");

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

    vOrigine.x          = var.getf("vOrigine.x");
    vOrigine.y          = var.getf("vOrigine.y");
    vOrigine.z          = 0.0;

    xSuivi              = var.getf("xSuivi");
    ySuivi              = var.getf("ySuivi");
    
    xSuivi              = vOrigine.x;
    ySuivi              = vOrigine.y;

	Camera_mgr&  cam_mgr = Camera_mgr::getInstance();
	cam_mgr.active();

    getSuiviParameter();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void exit_handler()
{
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

    parse_option(argc, argv);
    
    
    logf ((char*)"############## START MANAGER ###################");
    
    Camera_mgr::getInstance().reOrder();
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

    WindowsManager::genereMipMap( false );
    
    compute_matrix();
    glutMainLoop();


	return 0;
}



#endif
