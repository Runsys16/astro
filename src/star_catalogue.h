#ifndef STAR_CATALOG_H
#define STAR_CATALOG_H  1

#include "main.h"
#include "MathlibD.h"

#include <WindowsManager.h>
#include <thread>

//#define MOUV_PROPRE
using namespace std;

class StarCatalog 
{
public :
		//-----------------------
		double          fXScreen;
		double          fYScreen;
		double          fXTex;
		double          fYTex;
		//-----------------------
		double          fRA;
		double          fDE;
		double          fMag;
		
#ifdef MOUV_PROPRE
		double			fpmRA;
		double			feRA;
		double			fpmDE;
		double			feDE;
#endif
 
		int				idx;
		string          name;
		PanelText *     pInfo;
		//char            p_sInfo[225];

public :
        //StarCatalog( double, double, double, string );
        
        StarCatalog( double, double, double, string, int );
#ifdef MOUV_PROPRE
        StarCatalog( double, double, double, double, double, double, double, string, int );
#endif
        ~StarCatalog();
        
        void			affiche_position();
        void			affiche_magnitude();
        
inline  void            setXScreen(double d)    { fXScreen = d;}
inline  void            setYScreen(double d)    { fYScreen = d;}
inline  void			getScreen(vec2& v)		{ v.x = fXScreen; v.y = fYScreen; }
inline  void			setScreen(vec2& v)		{ fXScreen = v.x; fYScreen = v.y; }

inline  void            setTex(vec2& v)    		{ fXTex = v.x; fYTex = v.y; }
inline  void			getTex(vec2& v)			{ v.x = fXTex; v.y = fYTex; }

inline  double          getXScreen()            {return fXScreen;}
inline  double          getYScreen()            {return fYScreen;}
inline  double          getRA()                 {return fRA;}
inline  double          getDE()                 {return fDE;}
inline  double          getMag()                {return fMag;}
inline  string&         getName()               {return name;}
inline  PanelText*      getInfo()               {return pInfo;}
};

#endif
