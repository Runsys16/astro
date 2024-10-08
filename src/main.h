#ifndef MAIN_H
#define MAIN_H



#include <GL/glew.h>
#include <GL/glut.h>

#include <stdio.h>
#include <stdarg.h>

#include <fstream>
#include <iostream>
#include <string>
#include <getopt.h>
#include <thread>
#include <time.h>
#include <atomic>

#include <WindowsManager.h>
#include "button_callback.h"

#include "surveillance.h"
#include "MathlibD.h"
#include "panel_courbe.h"
//#include "catalog.h"


#define BOOL2STR(b) b?(char*)"true":(char*)"false"

#define COLOR_R(CCC)	            ((CCC&0xff000000)>>24)
#define COLOR_G(CCC)	            ((CCC&0x00ff0000)>>16)
#define COLOR_B(CCC)	            ((CCC&0x0000ff00)>>8)
#define COLOR_A(CCC)	            ((CCC&0x000000ff))
#define COLOR32(RR, GG, BB, AA)	    ((RR<<24)|(GG<<16)|(BB<<8)|(AA))

class Catalog;


using namespace std;

struct hms
{
    double   h;
    double   m;
    double   s;
};

struct dms
{
    double   d;
    double   m;
    double   s;
};

struct etoile
{
    struct hms  ad;
    struct dms  dec;
};

struct sky_point
{
    double   xAverage;
    double   yAverage;
    double   ponderation;
    double   x;
    double   y;
    bool    found;
};

struct readBackground
{
    atomic<unsigned int>    w;
    atomic<unsigned int>    h;
    atomic<unsigned int>    d;
    atomic<GLubyte*>        ptr;
};
typedef struct readBackground       rb_t;

#ifndef MAIN_CPP
    extern bool                     bNuit;
    extern bool                     bFindStar;
    extern atomic<bool>             bOneFrame;
    extern atomic<bool>             bPause;
    extern bool                     bPanelCourbe;
    extern bool                     bSimu;
    extern bool                     bMouseDeplace;
    extern bool                     bMouseDeplaceVers;
    extern bool                     bAffCentre;
    extern bool                     bAffSuivi;
    extern bool                     bAfficheVec;
    extern bool                     bPanelStdOut;
    extern bool                     bSound;
    extern bool                     bCorrection;
    extern bool                     bCentrageSuivi;
    extern bool                     bPanelResultat;

    extern int                      width;
    extern int                      height;
    //extern vector<vec2>             t_vResultat;
    //extern vector<vec2>             t_vSauve;
    extern bool                     bModeManuel;

    extern double                    xSuivi;
    extern double                    ySuivi;
    extern double                    xSuiviSvg;
    extern double                    ySuiviSvg;
        
    extern int                      xClick;
    extern int                      yClick;
        
    extern vector<vector<vec2> * >  t_vTrace;
    extern bool                     bAffTrace;
    extern bool                     bRecTrace;
    extern vcf4                     colorTraces[];

    extern bool                     bInverseCouleur;
    extern double					fLimitCorrection;
    extern double                    fTimeMili;
    
    #ifndef PANELCOURBE_CPP
    extern PanelCourbe*             panelCourbe;
    extern PanelWindow*             panelResultat;
    #endif
    
    extern ivec2					mouse;
    extern vec3                     vecAD[2];
    extern vec3                     vecDC[2];
    extern vec3                     vDepl[2];

    extern double                   pas_sideral;
    extern bool                     bSauve;
    extern string                   filenameSauve;

    extern int                      iDisplayfft;
    extern bool                     bDisplayfftX;
    extern bool                     bDisplayfftY;

    extern int                      iDisplayCourbe;
    extern bool                     bDisplayCourbeX;
    extern bool                     bDisplayCourbeY;

    extern double                    filtre;

    extern double                   Xref;
    extern double                   Yref;
    extern double                   ZrefX;
    extern double                   ZrefY;
    extern double                   Wref;
    extern bool                     bAffCatalog;
    extern Catalog                  vizier;
    
	extern double					hms2rad( struct hms& );
	extern double					dms2rad( struct dms& );

	extern void                		rad2hms( double, struct hms& );
	extern void                		rad2dms( double, struct dms& );
	extern void                		deg2hms( double, struct hms& );
	extern void                		deg2dms( double, struct dms& );

	extern bool						bArduino;
	extern int						iGlutModifier;
#endif

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
class Capture;
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
class CallbackSauveGuidage : public ButtonCallBack
{
public :
    virtual     void    callback( bool, int, char* );
};
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
class CallbackChargeGuidage : public ButtonCallBack
{
public :
    virtual     void    callback( bool, int, char* );
};
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
class CallbackFileBrowser : public ButtonCallBack
{
public :
    virtual     void    callback( bool, int, char* );
};
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
class CallbackFits : public ButtonCallBack
{
public :
    virtual     void    callback( bool, int, char* );
};
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void                vizier_load_stars( string, double, double );
void                vizier_load_stars( string );
void                vizier_load_stars( Catalog*, string, double, double );
void                vizier_load_stars( Catalog*, string );

void 				get_info_arduino();
string              get_basename(string);
unsigned int        get_color(vcf4);
bool                fexists(const char *filename);
bool                isPleiade();

void                callback_enregistre(bool, int, char*);
void                callback_enregistre_cam(char*);
void                callback_charge_cam(char*);

vector<string>      split (string s, string delimiter);

void                captureOnTop(Capture*);

void                ferme_file_browser();
void                updatePanelPause();
void                updatePanelPause(bool);

string              getCurrentDirectory();
void                setCurrentDirectory(string);
void                charge_image( string, string );

void                alertBox( string mes );
void                alertBoxQuit();

int                 getScreenDX();
int                 getScreenDY();

int                 getWidth();
int                 getHeight();

vector<string>&     getExclude();
double               hms2rad( struct hms& );
double               dms2rad( struct dms& );

void                rad2hms( struct hms&, double );
void                rad2dms( struct dms&, double );

void                glVecAD();
void                glVecDC();
void                glCercle(int x, int y, int rayon);
void                glCarre( int x,  int y,  int dx,  int dy );
void                glCroix( int x,  int y,  int dx,  int dy );
void                displayGLCourbe(void);
void                displayGLCamera_cb(void);
void                glEchelle();

void                displayGLTrace();
void                displayGLnuit_cb();
void                displayResultat_cb(void);
void                displayCourbe(void);

int                 getOffset( int x, int y, int width );
void                screen2tex( int& x, int& y );
void                tex2screen( int& x, int& y);

double               getSkyPoint_colorR(int offset);
double               getSkyPoint_colorG(int offset);
double               getSkyPoint_colorB(int offset);
double               getSkyPoint_colorL(int offset);
void                updatePanelResultat(double x, double y, double mag);
void                getSkyPointLine(struct sky_point* point, int x, int y, int size);
double               getLumMax(int offset, double max );
double               getLum(int offset );
void                findSkyPoint(struct sky_point* point, int X, int Y, int size);
void                rechercheSkyPoint( int x, int y);

void                getSkyPoint(struct sky_point*, int, int, int);





void                change_fov( void );
static void         displayGL(void);
static void         reshapeGL(int newWidth, int newHeight);

void                reset_camera(void);
void                write_image(void);

void                change_hertz(double);
void                change_arduino(bool);
void                change_joy(int, int);
void                change_ad_status(double);
void                change_dc_status(double);
void                compute_matrix();

void                sauve(void);
void                charge_guidage(string);
void                sauve_traces(void);
void                charge_traces(void);
void                suivi(void);
void                charge_fits(string);

void                getSuiviParameter();

static void         idleGL(void);
static void         quit(void);
static void         initGL(int argc, char **argv);
static void         rotateVisible();

static void         glutKeyboardFunc(unsigned char key, int x, int y);
static void         glutKeyboardFuncCtrl(unsigned char key, int x, int y);
static void         glutKeyboardFuncAlt(unsigned char key, int x, int y);
static void         glutKeyboardUpFunc(unsigned char key, int x, int y);
static void         glutSpecialFunc(int key, int x, int y);
static void         glutSpecialUpFunc(int key, int x, int y);
static void         glutMouseFunc(int button, int state, int x, int y);
static void         glutMotionFunc(int x, int y);
static void         glutPassiveMotionFunc(int x, int y);

void                onTop();
void                resizeHelp  (int width, int height);
void                resizeControl(int width, int height);
void                resizeCourbe(int width, int height);
void                setColor();

static void         CreateResultat();
static void         createControlID(PanelSimple * p, int x, int y, char* str);
static void         createControlIDbyID(PanelSimple * p, int x, int y, char* str, int id);
static void         CreateHelp();
static void         CreateStatus();
static void         CreateStdOut();
static void         CreateAllWindows();

static void         usage(FILE *fp, int argc, char **argv);

void                displayGL(void);

void                changeDec(bool);
void                changeAsc(bool);
void                changeSui(bool);
void                changeJoy(bool);
bool                getJoy();
void                changeRetourPos(bool);

void                log_tab( bool );
void                log( char *);
void                logf( char *, ...);

void                parse_option( int argc, char**argv );
void                parse_option_size( int argc, char**argv );
int                 main(int argc, char **argv);


//
//  satatus.cpp
//
void set_asservissement(void);
void set_mode(bool);
void inverse_texture(PanelButton *, bool, string);
void set_courbe();

void idleStatus();

#endif
