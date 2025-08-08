#ifndef STARS_COMPARE_H
#define STARS_COMPARE_H  1

#include "main.h"
#include "MathlibD.h"
#include "stars.h"
#include "catalog.h"

#include <WindowsManager.h>


using namespace std;

class StarCompare
{

private:
		Stars*				pStars;
		Catalog*			pVizier;
	
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
		
	
inline	void				setStars( Stars* p )				{ pStars = p; }
inline	void				setVizier( Catalog* p )				{ pVizier = p; }
};

#endif
