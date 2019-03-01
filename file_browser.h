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
    
    string                      workingDir;
    string                      currentDir;
 
    int                         x, y, dx, dy;
   
    
    
public :
    FileBrowser();

    void                        explore_dir();
    void                        change_dir( int );
    
    bool                        isInsideDir(int, int);
    bool                        isInsideFile(int, int);

    vector<string>&             getDirNames()                       { return tDirNames; }
    vector<string>&             getFilesNames()                     { return tFileNames; }
    
    inline string               getCurrentDir()                     { return currentDir; }
    inline string               getWorkingDir()                     { return workingDir; }
    inline string               setCurrentDir(string s)             { currentDir = string(s); }
    
    void                        addImage( string, PanelSimple*, int, int);
    
    void                        affiche();
    inline void                 cache()                             { pW->setVisible(false); }
    inline bool                 getVisible()                        { return pW->getVisible(); }
    
    //void                        quit();

SINGLETON_END()


#endif


