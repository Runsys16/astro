#ifndef PANEL_DIR_H
#define PANEL_DIR_H

#include "camera.h"
#include <dirent.h>
#include <algorithm>

#include <WindowsManager.h>
#include "main.h"
#include "var_mgr.h"
#include "file_browser.h"

using namespace std;

class FileBrowser;

class PanelDir : public PanelSimple
{
protected:
    FileBrowser*                    pFB;
    
public:
    PanelDir(FileBrowser*);

	virtual void		wheelUp( int, int);
	virtual void		wheelDown( int, int);

	virtual void		releaseLeft( int, int);
	virtual Panel*		isMouseOver( int, int);
};


#endif
