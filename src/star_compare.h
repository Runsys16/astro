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
		double				magMax;
vector<Star*>				tStar;
vector<StarCatalog*>		tVizi;
		
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
		
	
inline	void				setStars( Stars* p )				{ pStars = p; }
inline	void				setVizier( Catalog* p )				{ pVizier = p; }
inline	double				getMagMax()							{ return magMax; }
inline	void				setMagMax(double d)					{ magMax = d; }

};

#endif
