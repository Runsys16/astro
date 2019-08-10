#ifndef PANEL_FILE_H
#define PANEL_FILE_H

#include "camera.h"
#include <dirent.h>
#include <algorithm>

#include <WindowsManager.h>
#include "main.h"
#include "var_mgr.h"
#include "file_browser.h"

using namespace std;

class FileBrowser;


class PanelFile : public PanelSimple
{
protected:
    FileBrowser*                    pFB;
    
public:
    PanelFile(FileBrowser*);

	virtual void		wheelUp( int, int);
	virtual void		wheelDown( int, int);
};


#endif
