//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#ifndef PANELGRAPH_H
#define PANELGRAPH_H  1
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#include <GL/glew.h>
#include <GL/glut.h>

#include <stdio.h>
#include <complex>
 
#include "MathlibD.h"
#include <WindowsManager.h>
#include "main.h"
#include "var_mgr.h"



using namespace std;
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
class PanelGraph : public PanelWindow
{
private:
	string				name;
	PanelText*			pTitre;
	
	bool				bLogX;
	bool				bLogY;

	double				dXmin;
	double				dXmax;
	double				dYmin;
	double				dYmax;
	
	double				_pasX;
	double				_debX;
	double				_finX;
	
	double				_pasY;
	double				_debY;
	double				_finY;
	
	int					iMouse;
	Panel*				pPanelCallback;

vector<vec2>			cStar;
vector<vec2>			cVizi;
vector<int>				cSort;

vector<PanelText*>		tAbsPanel;
vector<vec2>			tAbsVec2;
vector<PanelText*>		tOrdPanelG;
vector<vec2>			tOrdVec2G;
vector<PanelText*>		tOrdPanelD;
vector<vec2>			tOrdVec2D;

public :
    ~PanelGraph();
    PanelGraph();

    void                init_var();
    
    void				panel2graphX( double& );
    void				panel2graphY( double& );
    void				panel2graph( vec2& );

    void				graph2panelX( double& );
    void				graph2panelY( double& );
    void				graph2panel( vec2& );

    void                glCroix( int, int, int, int );
    void                glCroix( vec2, vec2 );
    void                glLine( int, int, int, int );
    void                glLine( vec2, vec2 );
    void                glCarre( int, int, int, int );
    void                glEchelleAxe( int, int, float, float, PanelText*, PanelText* );
    void                glEchelle();

    void                build_unites_text(void);
    int                 sc2winX(int);
    int                 sc2winY(int);
    
    void				displayCourbeStar();
    void				displayCourbeVizi();
    void				ajoute_ordG( double, vec2 );
    void				ajoute_ordD( double, vec2 );
    void				ajoute_abs( double, vec2 );
    void				update_ord();
    void				update_abs();
    void				displayAxeVlin();
    void				displayAxeVlog();
    void				displayAxeV();
    void				displayAxeHlin();
    void				displayAxeHlog();
    void				displayAxeH();
    void				displayAxes();
    
    void				resetCourbeStar();
    void				resetCourbeVizi();
    void				resetCourbes();
    bool				doublons_star(vec2);
    bool				doublons_vizi(vec2);
    void				addViziStar(vec2, vec2);
    void				addStar(vec2);
    void				addVizi(vec2);
    //bool				cmp(vec2, vec2);
    void				sort_all();

virtual void            displayGL( void );
virtual void            updatePos( void );
virtual void			passiveMotionFunc( int, int);
/*
virtual void            clickLeft( int, int);
virtual void            motionLeft( int, int);
virtual void            releaseLeft( int, int);

virtual void            clickMiddle( int, int);
virtual void            motionMiddle( int, int);
virtual void            releaseMiddle( int, int);
*/    
inline void				setName( string s )				{ name = s; pTitre->changeText(name); }

inline void				setXmin(double d)				{ dXmin = d; }
inline void				setXmax(double d)				{ dXmax = d; }
inline void				setYmin(double d)				{ dYmin = d; }
inline void				setYmax(double d)				{ dYmax = d; }

inline void				setPanelCallback(Panel* p)		{ pPanelCallback = p; }
inline int				getIndexMouse()					{ return iMouse; }

inline void				debug_log()						{ bLogX = !bLogX; resetCourbes(); }
};
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#endif
