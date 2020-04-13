//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#ifndef PANELCOURBE_H
#define PANELCOURBE_H  1
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#include <GL/glew.h>
#include <GL/glut.h>

#include <stdio.h>
#include <complex>
#include <valarray>
 
#include <WindowsManager.h>
#include "main.h"
#include "Mathlib.h"
#include "console.h"
#include "var_mgr.h"

using namespace std;

typedef complex<float> CComplex;
typedef valarray<CComplex> CArray;
//const double PI = 3.141592653589793238460;
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
class PanelCourbe : public PanelWindow
{
private:
    float               offset_x;
    float               offset_y;
    float               delta_courbe1 = 1.0;
    float               delta_courbe2 = 1.0;
    float               courbe1 = 1.0;
    float               courbe2 = 1.0;
    float               taille_mini = 20.0; //(pixel)
    int                 decal_resultat = 0;
    
    float               alpha[256];
    float               inverseX[256];
    float               inverseY[256];

    CArray              signalX;
    CArray              signalY;
    CArray              signal_inverseX;// = signalX;
    CArray              signal_inverseY;// = signalY;


    float               delta_courbe1_svg = 1.0;
    float               delta_courbe2_svg = 1.0;
    float               courbe1_svg = 1.0;
    float               courbe2_svg = 1.0;
    int                 decal_svg = 0.0;
    


    int                 xCam;
    int                 yCam;
    int                 dxCam;
    int                 dyCam;
    
    int                 xm_svg;
    int                 ym_svg;
    
    int                 xStartAxe = 70.0;
    float               err = 2.0;

    PanelText*          pXMax;
    PanelText*          pXMin;
    PanelText*          pYMax;
    PanelText*          pYMin;


    vector<PanelText*>  unites;

    bool                bPanelCourbe;

    vector<vec2>        t_vResultat;
    vector<vec2>        t_vSauve;
    vector<vec2>        t_vCourbe;
    //vector<vec2>        courbeY;
    
    vec3                vOrigine;
    float*              pIn;
    float*              pOut;
    vector<float>       t_fOut;
    int                 nb;

public :
    PanelCourbe();
    ~PanelCourbe();
    
    void                init_var();
    void                sauve_guidage_1_1();
    void                sauve_guidage_1_0();
    void                sauve_guidage();
    
    void                charge_guidage_1_1(ifstream&);
    void                charge_guidage_1_0(ifstream&, string);
    void                charge_guidage_version(ifstream&);
    void                charge_guidage(string);

    void                idle_guidage(vec2);
    void                reset_guidage();
    void                ajoute(vec2 v);

    void                update_err();
    void                glEchelleAxe( int, int, float, float, PanelText*, PanelText* );
    void                glEchelle();
    void                glCourbeCube( float*, int, int, int, int, int, float, float, float );
    void                glCourbeCube( float*, int, int, int, int, int, float );
    void                glCourbe( float*, int, int, int, int, int, float, float, float );
    void                glCourbe( float*, int, int, int, int, int, float );
    void                glCourbes();
    void                glFft();

    void                build_unites_text(void);
    int                 sc2winX(int);
    int                 sc2winY(int);
    
virtual void            displayGL( void );

virtual void            clickMiddle( int, int);
virtual void            motionMiddle( int, int);
virtual void            releaseMiddle( int, int);
virtual void            clickLeft( int, int);
virtual void            motionLeft( int, int);
virtual void            releaseLeft( int, int);
    
    void                fft3( CArray& );
    void                ifft3( CArray& );
    void                build_fft3();
    void                sinusoide_fft3(CArray&, float*);

    
inline     void         setColor(long l)            { PanelWindow::setColor(l); }
inline     float        get_offset_x()              { return offset_x; }   
inline     float        get_offset_y()              { return offset_y; }   
inline     float        get_courbe1()               { return courbe1; }   
inline     float        get_courbe2()               { return courbe2; }   
inline     int          get_decal_resultat()        { return decal_resultat; }   
inline     float        get_delta_courbe1()         { return delta_courbe1; }   
inline     float        get_delta_courbe2()         { return delta_courbe2; }   
inline     int          get_xCam()                  { return xCam; }   
inline     int          get_yCam()                  { return yCam; }   
inline     int          get_dxCam()                 { return dxCam; }   
inline     int          get_dyCam()                 { return dyCam; }   
inline     float        get_err()                   { return err; }   
inline     PanelText*   get_pXMax()                 { return pXMax; }
inline     PanelText*   get_pXMin()                 { return pXMin; }
inline     PanelText*   get_pYMax()                 { return pYMax; }
inline     PanelText*   get_pYMin()                 { return pYMin; }
inline     vec3&        get_vOrigine()              { return vOrigine; }

inline     void         set_offset_x(float f)           { offset_x = f; }   
inline     void         set_offset_y(float f)           { offset_y = f; }   
inline     void         set_courbe1(float f)            { courbe1 = f; }   
inline     void         set_courbe2(float f)            { courbe2 = f; }   
inline     void         set_decal_resultat(int n)       { decal_resultat = n; }   
inline     void         set_delta_courbe1(float f)      { delta_courbe1 = f; }   
inline     void         set_delta_courbe2(float f)      { delta_courbe2 = f; }   
inline     void         set_xCam(int n)                 { xCam = n; }   
inline     void         set_yCam(int n)                 { yCam = n; }   
inline     void         set_dxCam(int n)                { dxCam = n; }   
inline     void         set_dyCam(int n)                { dyCam = n; }   
inline     void         set_err(float f)                { err = f; }   

};
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#endif
