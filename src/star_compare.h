#ifndef STARS_COMPARE_H
#define STARS_COMPARE_H  1

#include "main.h"
#include "MathlibD.h"
#include "stars.h"
#include "star.h"
#include "star_catalogue.h"
#include "catalog.h"

#include <WindowsManager.h>


using namespace std;

class StarCompare
{

private:
		Stars*				pStars;
		Catalog*			pVizier;

vector<Star*>				tStar;
vector<StarCatalog*>		tVizi;
vector<ivec2>				cmpViziStar;
		
		double				magMax;
		double				magMin;
		double				lumMax;
		double				lumMin;

public:
		StarCompare();
		StarCompare(Stars*, Catalog*);
		~StarCompare();
		void				init(Stars*, Catalog*);

		int					findVizier(vec2& vStar);
		double				getDst(vec2&, int);
		void				replace(string & str);
		void				compareStar(bool);
		void				compareStar();
		void				saveCompareStar();
		double				computeDelta();
		void				resetDelta();
		
		int					getTwinStar(int);
		int					getTwinVizier(int);
		
inline	vector<Star*>&			getStar()						{ return tStar; }
inline	vector<StarCatalog*>&	getVizi()						{ return tVizi; }
inline	vector<ivec2>&			getCmpViziStar()				{ return cmpViziStar; }

inline	double				getMagMax()							{ return magMax; }
inline	double				getMagMin()							{ return magMin; }
inline	double				getLumMax()							{ return lumMax; }
inline	double				getLumMin()							{ return lumMin; }
	
inline	void				setStars( Stars* p )				{ pStars = p; }
inline	void				setVizier( Catalog* p )				{ pVizier = p; }
inline	void				setMagMax(double d)					{ magMax = d; }

};

#endif
