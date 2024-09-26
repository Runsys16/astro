#ifndef PANEL_FITS_H
#define PANEL_FITS_H  1

#include <dirent.h>
#include <algorithm>

#include "main.h"
#include "var_mgr.h"
#include <WindowsManager.h>


using namespace std;

class PanelFits : public PanelWindow
{

protected:
        PanelScrollY*				pScroll;
        Panel*          			pParentView;
        vector<PanelText*>			fits_keys;
        vector<PanelText*>			fits_values;
        
    
public:
        ~PanelFits();
        PanelFits();

        void            add_key_value(string, string);
        
        
virtual void            update();
virtual void	        displayGL();
virtual void	        updatePos();

virtual void            clickRight( int, int);
virtual void            motionRight( int, int);
virtual void            releaseRight( int, int);

virtual void			wheelUp( int, int);
virtual void			wheelDown( int, int);



        void            setParent(Panel*);

};


#endif
