#ifndef PANEL_DEBUG_H
#define PANEL_DEBUG_H
//----------------------------------------------------------------------------------------------------------
#include <iostream>
#include <GL/glew.h>
#include <string.h>
#include <vector>

#include "MathlibD.h"
#include "main.h"

#include <WindowsManager.h>

//----------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------
class PanelDebug : public PanelWindow
{
protected:
	vector<PanelText*>				pText;
	unsigned						idx;
	unsigned						tab_size;
	
	uint32_t						uColor;
public:
									PanelDebug();
									~PanelDebug();
	
		void						add_text( char* );
		void						add_textf( char*, ... );
		void						reset_list();
	 	void						setTabSize(unsigned);
	 	void						setColor( uint32_t );
	
inline	void						setDebug(bool b )			{ setVisible(b); }
};
//----------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------
#endif

