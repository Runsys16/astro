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

#include <WindowsManager.h>
#include "button_callback.h"

#include "surveillance.h"
#include "Mathlib.h"
#include "panel_courbe.h"


#define BOOL2STR(b) b?(char*)"true":(char*)"false"


using namespace std;

struct hms
{
    float   h;
    float   m;
    float   s;
};

struct dms
{
    float   d;
    float   m;
    float   s;
};

struct etoile
{
    struct hms  ad;
    struct dms  dec;
};

struct sky_point
{
    float   xAverage;
    float   yAverage;
    float   ponderation;
    float   x;
    float   y;
    bool    found;
};

struct readBackground
{
    unsigned int            w;
    unsigned int            h;
    unsigned int            d;
    GLubyte*                ptr;
};
typedef struct readBackground       rb_t;

#ifndef MAIN_CPP
    extern bool                     bNuit;
    extern bool                     bFindStar;
    extern bool                     bOneFrame;
    extern bool                     bPause;
    extern bool                     bPanelCourbe;
    extern bool                     bSimu;
    extern bool                     bMouseDeplace;
    extern bool                     bAffCentre;
    extern bool                     bAffSuivi;
    extern bool                     bAfficheVec;
    extern bool                     bPanelStdOut;
    extern bool                     bSound;

    extern int                      width;
    extern int                      height;
    extern vector<vec2>             t_vResultat;
    extern bool                     bModeManuel;

    extern float                    xSuivi;
    extern float                    ySuivi;
        
    extern int                      xClick;
    extern int                      yClick;
        
    extern vector<vector<vec2> * >  t_vTrace;
    extern bool                     bAffTrace;
    extern bool                     bRecTrace;
    extern vec4                     colorTraces[];

    extern bool                     bInverseCouleur;
    
    #ifndef PANELCOURBE_CPP
    extern PanelCourbe*             panelCourbe;
    #endif
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
void                change_file( string, string );

void                alertBox( string mes );
void                alertBoxQuit();

int                 getScreenDX();
int                 getScreenDY();

int                 getWidth();
int                 getHeight();

vector<string>&     getExclude();
float               hms2rad( struct hms& );
float               dms2rad( struct dms& );

void                rad2hms( struct hms&, float );
void                rad2dms( struct dms&, float );

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

float               getSkyPoint_colorR(int offset);
float               getSkyPoint_colorG(int offset);
float               getSkyPoint_colorB(int offset);
float               getSkyPoint_colorL(int offset);
void                updatePanelResultat(float x, float y, float mag);
void                getSkyPointLine(struct sky_point* point, int x, int y, int size);
float               getLumMax(int offset, float max );
float               getLum(int offset );
void                findSkyPoint(struct sky_point* point, int X, int Y, int size);
void                rechercheSkyPoint( int x, int y);

void                getSkyPoint(struct sky_point*, int, int, int);





void                change_fov( void );
static void         displayGL(void);
static void         reshapeGL(int newWidth, int newHeight);

void                reset_camera(void);
void                write_image(void);

void                change_hertz(float);
void                change_arduino(bool);
void                change_joy(int, int);
void                change_ad(float);
void                change_dc(float);
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
void set_mode(void);
void inverse_texture(PanelButton *, bool, string);
void set_courbe();

void idleStatus();

#endif
