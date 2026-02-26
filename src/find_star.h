#ifndef FIND_STAR_H
#define FIND_STAR_H  1
//----------------------------------------------------------------------
#include "main.h"
#include <WindowsManager.h>
#include "convert.h"
#include "panel_graph.h"
#include "panel_debug.h"
#include <mutex>
#include <thread>
//----------------------------------------------------------------------
using namespace std;
//----------------------------------------------------------------------
struct etoile_line
{
	vec2	centre;
	vec2	deb;
	vec2	fin;
	double	lum;
	int		li;
	int		rayon;
};
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
class FindStar
{
//----------------------------------------------------------------------
private:
//---------------------------------------
    GLubyte*			ptr;
    rb_t*				pRB;
    Convert*			pConvert;
    PanelSimple*		pView;
    PanelDebug*			pInfo;
//---------------------------------------
	PanelGraph*			pGraphDistri;
	PanelGraph*			pGraphLum;
	string				name;
//---------------------------------------
	double				dOffsetLow;
//---------------------------------------
	double				local_deb;
	double				local_fin;
	double				local_centre;
	double				local_min;
	double				local_max;
	double				local_dif_p;
	double				local_dif_m;
	double				local_dif;
	int					local_nb;
	double				local_lum;
	double				local_l;
//---------------------------------------
	double				max_lum;
//---------------------------------------
vector<etoile_line>		tStar;
mutex					muStar;
thread					thFindStar;
//---------------------------------------
public:
						~FindStar();
						FindStar();
//---------------------------------------
	void				create_graph_distri();
	void				create_graph_lum();
//---------------------------------------
	double				get_li_low_lvl(int);
	double				getLum(int );
	double				getLum(vec2);
	void				get_li_distribution(int);
	void				update_graph_segment(); 
	void				recentre_etoile(vec2&, int&);
//---------------------------------------
	void				add_star(vec2, double);
	void				get_cercle_n(vec2, double);
	void				get_centre_n(vec2, double);
	void				get_max(vec2, vec2&, double&, double);
	double				get_lum_cercle(vec2, double);
	int					exist(vec2, double);
	void				find_star_2(vec2);
	void				find_stars_2(vec2);
//---------------------------------------
	void				reset_local();
	void				print_local(int);
	void				find_line(int, bool);
	void				click_find_star(vec2);
	void				click_graph_distri(vec2);
	void				click_graph_lum(vec2);
	void				click_all();
//---------------------------------------
	void				glCercle(vec2, double);
	void				glCroix(int,  int,  int,  int);
	void				glCroix(int,  int,  int );
	void				display_segment(struct etoile_line&);
	void				displayGL();
//---------------------------------------
inline int 				getOffset( int X, int Y )			{ return pRB->d*(X) + pRB->d*(Y)*pRB->w; }
inline void				setRB(rb_t* p)						{ pRB = p; ptr = pRB->ptr; }
inline void				setView(PanelSimple* p	)			{ pView = p; }
inline void				setConvert(Convert* p)				{ pConvert = p; }
inline void				setName(string s)					{ name = s; }
inline PanelGraph*		getPanelGraphDistri()						{ return pGraphDistri; }
//---------------------------------------

};
//----------------------------------------------------------------------
#endif
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

