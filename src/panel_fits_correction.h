#ifndef PANEL_FITS_CORRECTION_H
#define PANEL_FITS_CORRECTION_H  1

#include <dirent.h>
#include <algorithm>

#include "main.h"
#include "var_mgr.h"
#include "panel_spin_edit_text.h"
#include <WindowsManager.h>

using namespace std;

class PanelCorrectionFits : public PanelWindow
{

protected:
		double						dCDELT1c;
		double						dCDELT2c;
        PanelSpinEditText*			pCDELT1plus;
        PanelSpinEditText*			pCDELT2plus;
    
public:
        ~PanelCorrectionFits();
        PanelCorrectionFits();

	    void						setVisible(bool);
	    void						supCallBacks();
	    bool						keyboard(char key, int x, int y);
	    void						saveValues();
    
virtual void                        displayGL();

inline	PanelSpinEditText*			getCDELT1()						{ return pCDELT1plus;; }
inline	PanelSpinEditText*			getCDELT2()						{ return pCDELT2plus;; }
		
		void						setParent();
		void						onTop();
//virtual	void					clickLeft( int, int );
};


#endif
