#ifndef FILE_BROWSER_H
#define FILE_BROWSER_H  1

#include "main.h"
#include <dirent.h>

#include <string>
#include <algorithm>
#include <WindowsManager.h>
#include "Singleton.h"


SINGLETON_BEGIN(  FileBrowser )

protected:
    PanelWindow*                pW;
    PanelText*                  panelDirName;
    PanelSimple*                panelDir;
    PanelSimple*                panelFile;
    
    PanelButton*                panelOK;
    PanelButton*                panelQuit;

    vector<PanelText*>          lDirs;
    vector<PanelText*>          lFile;
    
    vector<string>              tDirNames;
    vector<string>              tFileNames;
    
    string                      currentDir;
 
    int                         x, y, dx, dy;
   
    
    
public :
    FileBrowser();

    void                        explore_dir( string );
    void                        change_dir( int );
    
    bool                        isInsideDir(int, int);
    bool                        isInsideFile(int, int);

    vector<string>&             getDirNames()                       { return tDirNames; }
    vector<string>&             getFilesNames()                     { return tFileNames; }
    
    inline string               getCurrentDir()                     { return currentDir; }
    
    void                        addImage( string, PanelSimple*, int, int);
    inline void                 affiche()                           { pW->setVisible(true); }
    inline void                 cache()                             { pW->setVisible(false); }
    
    //void                        quit();

SINGLETON_END()


#endif


