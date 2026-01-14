#ifndef FIND_STAR_H
#define FIND_STAR_H  1

#include "main.h"
#include "convert.h"

using namespace std;

class FindStar
{
private:
    GLubyte*			ptr;
    rb_t*				pRB;
    Convert*			pConvert;
    PanelSimple*        pView;

public:
						~FindStar();
						FindStar();

	int					getOffset( int, int );
	float				getLum(int );
	float				getLum( vec2 );
	void				find_line( vec2 );

inline void				setRB(rb_t* p)						{ pRB = p; ptr = pRB->ptr; }
inline void				setView(PanelSimple* p	)			{ pView = p; }
inline void				setConvert(Convert* p)				{ pConvert = p; }

};


#endif
