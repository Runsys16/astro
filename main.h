#ifndef MAIN_H
#define MAIN_H



#include <GL/glew.h>
#include <GL/glut.h>

#include <stdio.h>
#include <stdarg.h>

#include <iostream>
#include <string>
#include <getopt.h>
#include <thread>
#include <time.h>

#include <WindowsManager.h>

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
};

struct readBackground
{
    unsigned int             w;
    unsigned int             h;
    unsigned int             d;
    GLubyte*        ptr;
};


vector<string>&     getExclude();
float               hms2rad( struct hms& );
float               dms2rad( struct dms& );

void                rad2hms( struct hms&, float );
void                rad2dms( struct dms&, float );

void                glCercle(int x, int y, int rayon);
void                glCarre( int x,  int y,  int dx,  int dy );
void                glCroix( int x,  int y,  int dx,  int dy );
void                displayGL_cb(void);
void                glEchelle();
void                displayCourbeGL_cb(void);
int                 getOffset( int x, int y, int width );
void                screen2tex( int& x, int& y );
void                tex2screen( int& x, int& y);
void                getSkyPoint(struct sky_point&, int, int, int);

void                change_fov( void );
static void         displayGL(void);
static void         reshapeGL(int newWidth, int newHeight);

void                reset_camera(void);
void                write_image(void);

void                change_hertz(float);
void                change_arduino(bool);
void                change_ad(float);
void                change_dc(float);

void                getSuiviParameter();

static void         idleGL(void);
static void         quit(void);
static void         initGL(int argc, char **argv);
static void         rotateVisible();

static void         glutKeyboardFunc(unsigned char key, int x, int y);
static void         glutKeyboardUpFunc(unsigned char key, int x, int y);
static void         glutSpecialFunc(int key, int x, int y);
static void         glutSpecialUpFunc(int key, int x, int y);
static void         glutMouseFunc(int button, int state, int x, int y);
static void         glutMotionFunc(int x, int y);
static void         glutPassiveMotionFunc(int x, int y);

void                resizeHelp  (int width, int height);
void                resizeControl(int width, int height);
void                resizeCourbe(int width, int height);

static void         CreateResultat();
static void         createControlID(PanelSimple * p, int x, int y, char* str);
static void         createControlIDbyID(PanelSimple * p, int x, int y, char* str, int id);
static void         CreateHelp();
static void         CreateStatus();
static void         CreateStdOut();
static void         CreateAllWindows();

static void         usage(FILE *fp, int argc, char **argv);

void                displayGL(void);


void                log( char *);
void                logf( char *, ...);

void                parse_option( int argc, char**argv );
void                parse_option_size( int argc, char**argv );
int                 main(int argc, char **argv);

#endif
