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

//#define DEBUG 1
#define SIZEPT  20
#define AXE_X   (300.0/4.0)
#define AXE_Y   (3.0*300.0/4.0)


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

int                 wImg;
int                 hImg;
int                 nPlanesImg;

ivec2               vCameraSize;
GLubyte*             ptr;
vector<string>      exclude;

int                 _r[256];
int                 _g[256];
int                 _b[256];

float               xSuivi;
float               ySuivi;
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

#define             GET_OFFSET(x,y,width)   (3*y*width+x)
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
vector<vec2>        t_vResultat;
vector<vec2>        t_vSauve;
float               offset_x;
float               offset_y;
float               delta_courbe1 = 1.0;
float               delta_courbe2 = 1.0;
float               courbe1 = 1.0;
float               courbe2 = 1.0;
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

vector<Capture*>    captures;
int                 current_capture = -1;
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
float fTimer = 0.0;
bool bAsc = true;
bool bDec = true;
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
void photo()
{
    string filename = " -o /home/rene/Documents/astronomie/logiciel/k200d/test";
    string iso = " -i 100";
    string time = " -t 1";
    string frames = " -F 2";
    string timeout = " --timeout=3";
    string focus = " -f";
    string command = "/home/rene/Documents/astronomie/logiciel/k200d/pktriggercord-0.84.04/pktriggercord-cli";
    //command = command + filename + iso + time + timeout;
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
void change_file( string dirname, string filename )
{
    captures.push_back( new Capture(dirname, filename) );
    logf ( (char*)"main.c::change_file()  Chargement du fichier %s%s", dirname.c_str(), filename.c_str() );
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
    int dx = glutGet(GLUT_SCREEN_WIDTH);
    if (dx==3200)            dx = 1920;
    return dx;
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
    int dy = glutGet(GLUT_SCREEN_HEIGHT);
    return dy;
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
void displayGL_cb(void)
{
    if ( bSuivi && bAutorisationSuivi )   {
        if ( var.getb("bNuit") )        glColor4f( 1.0, 0.0, 0.0, 0.2 );
	    else                            glColor4f( 0.0, 1.0, 0.0, 0.2 );
	    
	    int x = round(xSuivi+0.5);
	    int y = round(ySuivi+0.5);
	    
	    tex2screen(x,y);

	    glCroix(x, y, 50, 50);
        glCarre(x, y, SIZEPT, SIZEPT);
            
    }
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


    if ( var.getb("bNuit") )        glColor4f( 1.0, 0.0, 0.0, 1.0 );
    else                            glColor4f( 1.0, 1.0, 1.0, 1.0 );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void glEchelle()
{
    float gris = 0.3;
    if ( var.getb("bNuit") )        glColor4f( 0.5, 0.0, 0.0, 1.0 );    
    else                            glColor4f( gris, gris, gris, 1.0 );
    
    glBegin(GL_LINES);
        int y = AXE_X;
        int x = 0;

        panelCourbe->x2Screen(x);
        panelCourbe->y2Screen(y);

        glVertex2i( x, y );
        
        x += panelCourbe->getPosDX();

        glVertex2i( x, y );
     
        y = AXE_Y;
        x = 0;

        panelCourbe->x2Screen(x);
        panelCourbe->y2Screen(y);

        glVertex2i( x, y );
        
        x = panelCourbe->getDX() + x;
        panelCourbe->x2Screen(x);

        glVertex2i( x, y );

        float dxMax = err;
        y = (float)(delta_courbe1*(dxMax) + AXE_X);
        pXMax->setPos( 5, y );
        panelCourbe->y2Screen(y);
        glVertex2i( 0, y );
        glVertex2i( x, y );

        float dxMin = -err;
        y = (float)(delta_courbe1*(dxMin) + AXE_X);
        pXMin->setPos( 5, y -15 );
        panelCourbe->y2Screen(y);
        glVertex2i( 0, y );
        glVertex2i( x, y );

        float dyMax = err;
        y = (float)(delta_courbe2*(dyMax) + AXE_Y);
        pYMax->setPos( 5, y );
        panelCourbe->y2Screen(y);
        glVertex2i( 0, y );
        glVertex2i( x, y );

        float dyMin = -err;
        y = (float)(delta_courbe2*(dyMin) + AXE_Y);
        pYMin->setPos( 5, y -15 );
        panelCourbe->y2Screen(y);
        glVertex2i( 0, y );
        glVertex2i( x, y );

        for( int i=0; i<2000; i+=60)
        {
            int x = i;
            int y = 0;
            panelCourbe->x2Screen(x);
            panelCourbe->y2Screen(y);
            glVertex2i( x, y );
            
            y += panelCourbe->getPosDX();
            glVertex2i( x, y );
            
        }
    glEnd();        
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void displayCourbeGL_cb(void)
{
    int DY = panelCourbe->getY();
    int n = t_vResultat.size();
    
    if ( bCorrection || bRestauration )
    {
        offset_x = vOrigine.x;
        offset_y = vOrigine.y;
    }
        offset_x = vOrigine.x;
        offset_y = vOrigine.y;

    glEchelle();
    
    if ( n != 0  )
    {
        if ( var.getb("bNuit") )        glColor4f( 1.0, 0.0, 0.0, 1.0 );
        else                            glColor4f( 0.0, 0.0, 1.0, 1.0 );
        
        glBegin(GL_LINE_STRIP);
        for( int i=0; i<t_vResultat.size(); i++ )
        {
            int y = (float)(delta_courbe1*(t_vResultat[i].x-offset_x) + AXE_X);
            int x = (n-i)*courbe1;

            panelCourbe->x2Screen(x);
            panelCourbe->y2Screen(y);

            glVertex2i( x, y );
            //printf( "%d %0.2f %0.2f y=%0.2f\n", i, t_vResultat[i].x, offset_x, y );
        }
        glEnd();        

        
        if ( var.getb("bNuit") )        glColor4f( 1.0, 0.0, 0.0, 1.0 );
        else                            glColor4f( 1.0, 1.0, 0.0, 1.0 );
        glBegin(GL_LINE_STRIP);
        for( int i=0; i<t_vResultat.size(); i++ )
        {
            int y = (float)(delta_courbe2*(t_vResultat[i].y-offset_y) + AXE_Y);
            int x = (n-i)*courbe2;

            panelCourbe->x2Screen(x);
            panelCourbe->y2Screen(y);

            glVertex2i( x, y );
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

    tex2screen(xPanel,yPanel);
    panelResultat->setPos(xPanel+20 , yPanel+20);

    int offset = getOffset( x ,y ,vCameraSize.x);

    
    char sSkyPoint[255];
    char sR[255];
    char sG[255];
    char sB[255];
    char sL[255];
    sprintf( sSkyPoint, "(%0.2f,%0.2f) / (%0.2f,%0.2f)  mag=%0.2f", x, y, vOrigine.x, vOrigine.y, pond2mag(mag) );
    SP->changeText(sSkyPoint);
    
    sprintf( sR, "r=%03d", (int)getSkyPoint_colorR(offset) );
    sprintf( sG, "g=%03d", (int)getSkyPoint_colorG(offset) );
    sprintf( sB, "b=%03d", (int)getSkyPoint_colorB(offset) );
    sprintf( sL, "l=%03d", (int)getSkyPoint_colorL(offset) );

    R->changeText(sR);
    G->changeText(sG);
    B->changeText(sB);
    L->changeText(sL);
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void getSkyPointLine(struct sky_point* point, int x, int y, int size)
{
    int min = x - size;
    int max = x + size;
    
    //int offset = GET_OFFSET(0,y,vCameraSize.x);
    
    //printf( "getPointLine x%d y%d\n", x, y);
    ptr = Camera_mgr::getInstance().getPtr();
    size_t size_ptr = malloc_usable_size ( (void*) ptr);
    
    for( x=min; x<max; x++) 
    {
        int offset = getOffset(x,y,vCameraSize.x);
        if ( offset+4 > size_ptr )
        {
            logf( (char*)" lg : %ld  offset : %ld", (unsigned long) size_ptr, (unsigned long) offset );
            return;
        }
        
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
            return;
        }

        float l = 0.33 * r + 0.5 * g  + 0.16 * b;
        if ( l<20.0 )       l = 0.0;
        
        //if (bPrintLum)      printf( "%-3d-", (int)l);
        
        point->xAverage    += l * (float)x;
        point->yAverage    += l * (float)y;
        point->ponderation += l;
    }  

    //if (bPrintLum)      printf( "\n" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
float getLumMax(int offset, float max )
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

    float l = 0.33 * r + 0.5 * g  + 0.16 * b;
    if ( l<max )       l = 0.0;

    return l;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
float getLum(int offset )
{
    return getLumMax( offset, 20.0 );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void findSkyPoint(struct sky_point* point, int X, int Y, int size)
{
    int x, y;

    y = X - size;
        
    ptr = Camera_mgr::getInstance().getPtr();
    size_t size_ptr = malloc_usable_size ( (void*) ptr);
    //-------------------------------------------------------------------------    
    y = Y - size;
    for( int i =-size; i<size; i++ ) 
    {
        x = X+i;
        int offset = getOffset( x, y, vCameraSize.x);

        if ( offset+4 > size_ptr )        {
            logf( (char*)" lg : %ld  offset : %ld", (unsigned long) size_ptr, (unsigned long) offset );
            return;
        }
        
        float l = getLum( offset) ;
        
        if (l!=0.0) {  point->found = true; point->x = x; point->y = y; return; }
    }  
    //-------------------------------------------------------------------------    
    y = Y + size;
    for( int i =-size; i<size; i++ ) 
    {
        x = X+i;
        int offset = getOffset( x, y, vCameraSize.x);

        if ( offset+4 > size_ptr )        {
            logf( (char*)" lg : %ld  offset : %ld", (unsigned long) size_ptr, (unsigned long) offset );
            return;
        }
        
        float l = getLum( offset) ;
        
        if (l!=0.0) { point->found = true; point->x = x; point->y = y; return; }
    }  
    //-------------------------------------------------------------------------    
    //-------------------------------------------------------------------------    
    x = X - size;
    for( int i =-size; i<size; i++ ) 
    {
        y = Y + i;
        int offset = getOffset( x, y,vCameraSize.x);

        if ( offset+4 > size_ptr )        {
            logf( (char*)" lg : %ld  offset : %ld", (unsigned long) size_ptr, (unsigned long) offset );
            return;
        }
        
        float l = getLum( offset) ;
        
        if (l!=0.0) { point->found = true; point->x = x; point->y = y; return; }
    }  
    //-------------------------------------------------------------------------    
    x = X + size;
    for( int i =-size; i<size; i++ ) 
    {
        y = Y + i;
        int offset = getOffset( x, y,vCameraSize.x);

        if ( offset+4 > size_ptr )        {
            logf( (char*)" lg : %ld  offset : %ld", (unsigned long) size_ptr, (unsigned long) offset );
            return;
        }
        
        float l = getLum( offset) ;
        
        if (l!=0.0) { point->found = true; point->x = x; point->y = y; return; }
    } 
    point->found = false;  
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void getSkyPoint(struct sky_point* point, int x, int y, int size)
{
    //int X = (float)(x-xCam) * rw;
    //int Y = (float)(y-yCam) * rh;
    int min = y - size;
    int max = y + size;
    
    //printf( "getPoint\n");
    for(int Y=min; Y<max; Y ++)        getSkyPointLine(point, x, Y, size);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void change_fov( void )
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	GLdouble ratio = (GLdouble)width / (GLdouble)height;
	
	gluPerspective((GLdouble)fov, (GLdouble)ratio, (GLdouble)0.1, (GLdouble) 1000.0);
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

	change_fov();
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
void charge_fichier(void)
{
    //string filename = "/home/rene/.astropilot/svg.text";
    string filename = "/home/rene/.astropilot/sauvegarde.text";
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
    
    
    while ( getline (fichier, line) )
    {
        //cout << line << '\n';
        float rx, ry, ox, oy;

        logf( (char*) line.c_str() );
        sscanf( line.c_str(), "( %f , %f ) / ( %f , %f )", &rx, &ry, &ox, &oy );

        vOrigine.x = ox;
        vOrigine.y = oy;

        t_vResultat.push_back( vec2(rx,ry) );
    }    
    int n = t_vResultat.size();
    logf( (char*)"%0.2f, %0.2f", t_vResultat[n-1].x, t_vResultat[n-1].y );
    
    fichier.close();
    
    t_vSauve.clear();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void sauve(void)
{
    string filename = "/home/rene/.astropilot/sauvegarde.text";
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
void suivi(void)
{
    if ( Camera_mgr::getInstance().haveNewFrame() )
    {
        Camera_mgr::getInstance().haveUseFrame(false);
    }
    else
        return;
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

    if ( t_vResultat.size()>20000)      t_vResultat.clear();

    vec2 v;
    v.x = xx;
    v.y = yy;
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
    
    
    //printf( "3-Suivi x=%0.2f, y=%0.2f\n", xSuivi, ySuivi);
    //int xPanel = xSuivi;
    //int yPanel = ySuivi;
    //tex2screen(xPanel,yPanel);
    //panelResultat->setPos(xPanel+20 , yPanel+20);
    updatePanelResultat( xSuivi, ySuivi, point.ponderation );
    
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
	float time = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

    char sFPS[] = "fps 0000";
    sprintf( sFPS,"fps %d", *Timer::getInstance().getvFPSCounter() );
    pFPS->changeText((char*)sFPS);
    
	timer.Idle();

    if ( bAlert )
    {
        bAlert = false;
        AlertBox* p = new AlertBox( sAlert );
        tAlert.push_back( p );
        p->onTop();
    }
    
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


    if (!bPause)    {
        Camera_mgr::getInstance().change_background_camera();
    }

    if (bAutorisationSuivi & bSuivi)    suivi();

    if (!bAutorisationSuivi)
    {
        /*
        char   s[100];

        int x = xClick;
        int y = yClick;
        tex2screen(x,y);

        //sprintf( s, "x=%d, y=%d", x, y);
        sprintf( s, "(%0.2f,%0.2f) / (%0.2f,%0.2f)", (float)x, (float)y, vOrigine.x, vOrigine.y );
        SP->changeText(s);
        //panelResultat->setVisible(true);
        panelResultat->setPos(x+20 , y+20);
        */
        updatePanelResultat( xClick, yClick, 0 );
    }

	
    float elapsedTime = -1;
	if ( prevTime < 0 )	{
		prevTime = time;
	}
	else 	{
		elapsedTime = time - prevTime;
		prevTime = time;
	}
    WindowsManager::getInstance().idleGL( elapsedTime );
	if ( elapsedTime != -1 )
	{
	    fTimer += elapsedTime;
	    if ( fTimer >= 5.0 )
	    {
	        fTimer -= 5.0;
            
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
                //logf( (char*)"**** Asservissement -> '%s'",  cmd );
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
static void rotate_capture(bool bIcones)
{
    int n = captures.size();

    if ( n == 0 )           return;
       
    WindowsManager& wm = WindowsManager::getInstance();
    
    current_capture = ++current_capture % n;
    int dx, dy, dxi, dyi;
    float ratio = (float)width/(float)height;
    
    dxi = width / 8;
    dyi = height  / 6;

    dx = width - dxi;
    dy = height; 

    int j = current_capture;
    int DY;

    if ( bIcones )         n++;

    if ( n>1 )      DY = height / (n-1);    
    else            DY = height / (n);    

    if ( bIcones )         n--;

    int y=10;

    for (int i=0; i<n; i++ )
    {
        Capture* p = captures[j];
        
        if ( j==current_capture )
        {
            if ( !bIcones )
            {
                p->resize(10,10,dx-20,dy-20);
                p->onTop();
            }
            else
            {
                p->resize(dx+10,y+10,dxi-20,dyi-20);
                p->onTop();
                y += DY;
            }
        }
        else
        {
            p->resize(dx+10,y+10,dxi-20,dyi-20);
            p->onTop();
            y += DY;
        }
        j = ++j % n;
    } 
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void glutKeyboardFunc(unsigned char key, int x, int y) {
    //std::cout << (int)key << std::endl;
    //logf( (char*)"*** KEYBOARD GL ***" );
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



	
	switch(key){ 
	
	// touche escape
	case 27:
	    {
	    alertBox( "Confirmez la sortie du programme 'ESC'" );
	    bQuit = true;
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
	            static int cursor;
	            cursor = ++cursor % 20;
	            logf( (char*)"-------------- Touche SHIFT+TAB %04X", cursor );
	            glutSetCursor(cursor);
                break;
        	}
        	else if (modifier == GLUT_ACTIVE_ALT)
        	{
	            logf( (char*)"-------------- Touche ALT+TAB" );
                break;
        	}

	        rotate_capture(false);
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
		    glutPositionWindow( (getScreenDX()-width)/2, (getScreenDY()-height)/2  );
		    glutReshapeWindow( width, height );
            log( (char*)"NormalScreen !!!" );
		}
    	break;
    case 'q':
    	{
	    rotate_capture(true);
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
        bPause = !bPause;
        var.set("bPause", bPause);

        if ( bPause )   pStatus->changeText((char*)"Pause" );
        else            pStatus->changeText((char*)"----" );
        }
        break;
    case 'P':  // '-'
        {
        photo();
        }
        break;

    case '0':
        {
        Camera_mgr::getInstance().sup("video1");
        log( (char*)"Sup camera !!!" );
        }
        break;
    case '1':
        {
        Camera_mgr::getInstance().togglePanel();
        log( (char*)"Toggle panelCamera !!!" );
        }
        break;
    case '2':
        {
        bPanelHelp = !bPanelHelp;
        var.set("bPanelHelp", bPanelHelp);
        panelHelp->setVisible(bPanelHelp);
        WindowsManager::getInstance().onTop(panelHelp);
        log( (char*)"Toggle panelHelp !!!" );
        }
        break;
    case '3':
        {
        bPanelResultat = !bPanelResultat;
        var.set("bPanelResultat", bPanelResultat);
        panelResultat->setVisible(bPanelResultat);
        WindowsManager::getInstance().onTop(panelResultat);
        log( (char*)"Toggle panelResultat !!!" );
        }
        break;
    case '4':
        {
        bPanelCourbe = !bPanelCourbe;
        var.set("bPanelCourbe", bPanelCourbe);
        panelCourbe->setVisible(bPanelCourbe);
        WindowsManager::getInstance().onTop(panelCourbe);
        log( (char*)"Toggle panelCourbe !!!" );
        }
        break;
    case '5':
        {
        bPanelStdOut = !bPanelStdOut;
        var.set("bPanelStdOut", bPanelStdOut);
        panelStdOutW->setVisible(bPanelStdOut);
        WindowsManager::getInstance().onTop(panelStdOutW);
        log( (char*)"Toggle panelStdOut !!!" );
        }
        break;
    case '6':
        {
        bPanelSerial = !bPanelSerial;
        var.set("bPanelSerial", bPanelSerial);
        PanelWindow* p = PanelConsoleSerial::getInstance().getWindow();
        p->setVisible( bPanelSerial );
        WindowsManager::getInstance().onTop(p);
        log( (char*)"Toggle serial !!!" );
        }
        break;
    case '7':
        {
        captures.push_back( new Capture() );
        current_capture = captures.size() - 1;
        }
        break;
    case '8':
        {
        int n = captures.size();
        if ( n!= 0 )
        {
            Capture* p = captures[n-1];
            delete p;
            captures.pop_back();
        }
        else
            current_capture = -1;
        }
        break;
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
    case 'u':
        {
        courbe1 *= 0.8;
        var.set("courbe1", courbe1);
        }
        break;
    case 'U':
        {
        courbe1 /= 0.8;
        var.set("courbe1", courbe1);
        }
        break;
    case 'j':
        {
        delta_courbe1 *= 0.8;
        var.set("delta_courbe1", delta_courbe1);
        }
        break;
    case 'J':
        {
        delta_courbe1 /= 0.8;
        var.set("delta_courbe1", delta_courbe1);
        }
        break;
    case 'i':
        {
        courbe2 *= 0.8;
        var.set("courbe2", courbe2);
        }
        break;
    case 'I':
        {
        courbe2 /= 0.8;
        var.set("courbe2", courbe2);
        }
        break;
    case 'k':
        {
        delta_courbe2 *= 0.8;
        var.set("delta_courbe2", delta_courbe2);
        }
        break;
    case 'K':
        {
        delta_courbe2 /= 0.8;
        var.set("delta_courbe2", delta_courbe2);
        }
        break;
    case 'h':
        {
        Camera* p = Camera_mgr::getInstance().getCurrent();
        if ( p!=NULL )
        {
            //p->enregistre();
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
	
	Camera_mgr::getInstance().keyboard( key );
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

    int n = current_capture;
    if ( n == -1 )          return;


    Capture&  c = *captures[n];
    
    
    switch( key)
    {
	// right
	case 102:	
	    {

	    logf( (char*)"Touche right !!" );
        float m = c.getCentX() + 10.0;
        c.setCentX( m );

        logf( (char*)"Echelle=%0.2f  dx=%0.2f dy=%0.2f", c.getEchelle(), c.getCentX(), c.getCentY() );

		}
		break;
	// left
	case 100:	
	    {

        float m = c.getCentX() - 10.0;
        c.setCentX( m );

        logf( (char*)"Echelle=%0.2f  dx=%0.2f dy=%0.2f", c.getEchelle(), c.getCentX(), c.getCentY() );

	    }
		break;
	// up
	case 101:	
	    {

        float m = c.getCentY() - 10.0;
        c.setCentY( m );

        logf( (char*)"Echelle=%0.2f  dx=%0.2f dy=%0.2f", c.getEchelle(), c.getCentX(), c.getCentY() );

		}
		break;
	// down
	case 103:	
	    {

        float m = c.getCentY() + 10.0;
        c.setCentY( m );

        float e = c.getEchelle();

        logf( (char*)"Echelle=%0.2f  dx=%0.2f dy=%0.2f", c.getEchelle(), c.getCentX(), c.getCentY() );

		}	
		break;
	// pgup
	case 104:	
	    {

        float e = c.getEchelle() + 0.2;
        c.setEchelle(e);
        
        logf( (char*)"Echelle=%0.2f  dx=%0.2f dy=%0.2f", c.getEchelle(), c.getCentX(), c.getCentY() );

		}
		break;
	// pgdown
	case 105:	
	    {

        float e = c.getEchelle() - 0.2;
        c.setEchelle(e);
        
        logf( (char*)"Echelle=%0.2f  dx=%0.2f dy=%0.2f", c.getEchelle(), c.getCentX(), c.getCentY() );

        }
		break;
	// home
	case 106:	
	    {

        c.setEchelle( 1.0 );
        c.setCentX( 0.0 );
        c.setCentY( 0.0 );

        logf( (char*)"Echelle=%0.2f  dx=%0.2f dy=%0.2f", c.getEchelle(), c.getCentX(), c.getCentY() );

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
	WindowsManager::getInstance().mouseFunc(button, state, x, y);
    //WindowsManager::getInstance().onBottom(panelPreView);
    if ( panelStatus->isMouseOver(x, y) )
    {
        logf( (char*)"Souris sur la barre de status" );
        return;
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
	if ( bAutorisationSuivi && button == 0 && state == 0 )	{

        getSuiviParameter();

        Camera_mgr::getInstance().onBottom();
        
	    int X = x;
	    int Y = y;
	    
	    screen2tex(X,Y);
	    logf( (char*)"---------------------- Click (%d, %d)  (%d, %d) vCameraSize.x=%d" , x, y, X, Y, vCameraSize.x );
	    
        ptr = Camera_mgr::getInstance().getPtr();
        int N = getOffset(X, Y, vCameraSize.x);


        size_t size_ptr = malloc_usable_size ( (void*) ptr);
        if ( N+4 > size_ptr )
        {
            logf( (char*)" lg : %ld  offset : %ld", (unsigned long) size_ptr, (unsigned long) N );
            return;
        }


	    int r = ptr[N];
	    int g = ptr[N+1];
	    int b = ptr[N+2];

        float fL = 0.33 * (float)r + 0.5 * (float)g  + 0.16 * (float)b;
        int l = fL;

	    char sR[10];
	    char sG[10];
	    char sB[10];
	    char sL[100];
	    char skyPoint[100];
	    
	    sprintf( sR, "r=%03d", r );
	    sprintf( sG, "g=%03d", g );
	    sprintf( sB, "b=%03d", b );
	    sprintf( sL, "l=%03d", l );
	    

    
        struct sky_point point;
        point.xAverage = 0.0;
        point.yAverage = 0.0;
        point.ponderation = 0.0;
        
        bPrintLum = true;
        
        getSkyPoint(&point, X, Y, SIZEPT);
        
        for( int i=0; i<500; i++ )
        {
            findSkyPoint(&point, X, Y, i);
            if ( point.found )
            {
                int xx = point.x;
                int yy = point.y;
                

                point.xAverage = 0.0;
                point.yAverage = 0.0;
                point.ponderation = 0.0;

                bPrintLum = true;
                
                getSkyPoint(&point, xx, yy, SIZEPT);

                logf( (char*)"Iteration : %d  x=%0.2f, y=%0.2f", i, (float)point.x, (float)point.y );

                break;
            }
        }
        
        
        bPrintLum = false;
        
        if ( point.ponderation > 0.1 ) {

            float xx = point.xAverage / point.ponderation;
            float yy = point.yAverage / point.ponderation;
            
            sprintf( skyPoint, "x=%0.2f, y=%0.2f, mag=%0.2f", xx, yy, pond2mag(point.ponderation) );
	        SP->changeText(skyPoint);
	        //panelResultat->setVisible(true);

	        //printf( "%s", skyPoint);
	        
	        if ( bAutorisationSuivi)
	        {
	            bSuivi = true;
                var.set( "bSuivi", bSuivi );
            }
            	            
	        xSuivi = xx;
	        ySuivi = yy;
	        offset_x = xx;
	        offset_y = yy;

            updatePanelResultat( xSuivi, ySuivi, point.ponderation );
	        //t_vResultat.clear();
        }
        else {
            sprintf( skyPoint, "Rien");
	        SP->changeText(skyPoint);
	        bSuivi = false;
            var.set( "bSuivi", bSuivi );
        }

	    logf( (char*)"xSuivi=%0.2f ySuivi=%0.2f   " , xSuivi, ySuivi );
	    logf( (char*)"    l=%d rgb=%d,%d,%d" , r,g,b,  l );

	} 

    Camera_mgr::getInstance().onBottom();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void glutMotionFunc(int x, int y)	{	
	WindowsManager::getInstance().motionFunc(x, y);
    //WindowsManager::getInstance().onBottom(panelPreView);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void glutPassiveMotionFunc(int x, int y)	{
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

    panelHelp->setPos( x,  y );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void resizeCourbe(int width, int height)	{
    int dx = width - 20;
    int dy = 300;
    int x = 10;
    int y = height - 10 - 20 - dy;

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
    resizeCourbe( width, height );

    wm.add(panelCourbe);
    
    panelCourbe->setVisible(bPanelCourbe);
 	panelCourbe->setBackground((char*)"images/background.tga");
    panelCourbe->setDisplayGL( displayCourbeGL_cb );

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
    
    if ( var.existe("xPanelHelp") )
        X  = var.geti( "xPanelHelp");

    if ( var.existe("yPanelHelp") )
        Y  = var.geti( "yPanelHelp");

    if ( var.existe("dxPanelHelp") )
        DX = var.geti("dxPanelHelp");

    if ( var.existe("dyPanelHelp") )
        DY = var.geti("dyPanelHelp");

    panelHelp = new PanelWindow();
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
	addString( "h: Enregistre une image de la camera courante");
	addString( "u/U: Echelle en x sur la courbe 1");
	addString( "i/I: Echelle en x sur la courbe 2");
	addString( "j/J: Echelle en y sur la courbe 1");
	addString( "k/K: Echelle en y sur la courbe 2");
	addString( "r: Rappel des mesures de decalage en x,y du fichier beltegeuse.txt");
	addString( "R: Test alert BOX");
	addString( "l: List les ports /dev/ +  les controles ");
	addString( "L: List les variables");
	
	addString( "");
	addString( "t/T: change le temps de correction");
	addString( "Y: Lance l' asservissement");
	addString( "V: Initialise les coordonnees de suivi");
	addString( "v: Sauvegarde des coordonnees dans le fichier .astropilot/sauvegarde.text");
	addString( "ESC: --- SORTIE DU LOGICIEL ---" );

	WindowsManager::getInstance().add(panelHelp);
	
    panelHelp->setSize( DX, ++l*dy);
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
    pButtonSui->setVal( !b );
    if ( b != bAsc )
    {
        logf( (char*)"Suivi : %d", (int)b );
        //inverse_texture( pButtonAsc, b, "asc" );
        bAsc = b;
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

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

    //width = getScreenDX() - 100;
    //height = getScreenDY() - 40;
    width = getScreenDX();
    height = getScreenDY();
    
    cout <<"Screen size "<< getScreenDX() <<"x"<< getScreenDY() << endl;

	//xPos = 1200 + (getScreenDX()-width)/2;
	xPos = (getScreenDX()-width)/2;
	yPos = (getScreenDY()-height)/2;

	glutInitWindowPosition(xPos, yPos);
	glutInitWindowSize( width, height );

	if (glutCreateWindow("Astro Pilot") == 0){ 
		return 1;
	}

	//glutFullScreen();

	glutReshapeFunc(reshapeGL);
	glutDisplayFunc(displayGL);
	glutIdleFunc(idleGL);

	
	cout << "Ecran   wxh " << getScreenDX() << "x" << getScreenDY() << endl;
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

    getX11Screen();
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
    
    compute_matrix();
    glutMainLoop();


	return 0;
}


