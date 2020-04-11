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

#include <WindowsManager.h>
#include "main.h"
#include "Mathlib.h"
#include "console.h"
#include "var_mgr.h"

using namespace std;

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


    float               delta_courbe1_svg = 1.0;
    float               delta_courbe2_svg = 1.0;
    float               courbe1_svg = 1.0;
    float               courbe2_svg = 1.0;
    


    int                 xCam;
    int                 yCam;
    int                 dxCam;
    int                 dyCam;
    
    int                 xm_svg;
    int                 ym_svg;
    
    float               xStartAxe = 70.0;
    float               err = 2.0;

    PanelText*          pXMax;
    PanelText*          pXMin;
    PanelText*          pYMax;
    PanelText*          pYMin;


    vector<PanelText*>  unites;

    bool                bPanelCourbe;
    
    vec3                vOrigine;


public :
    PanelCourbe();
    ~PanelCourbe();
    
    void                init_var();
    //void                screen2tex( int& x, int& y );
    //void                tex2screen( int& x, int& y );
    void                setColor(long l)            { PanelWindow::setColor(l); }
    void                update_err();
    void                glEchelleAxe( int, int, float, float, PanelText*, PanelText* );
    void                glEchelle();
    void                glCourbe( float*, int, int, int, int, int, float );
    void                glCourbes();

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
    
    void                fft(double*,  unsigned long);
    
    
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
