#ifndef FILE_BROWSER_H
#define FILE_BROWSER_H  1

#include "main.h"
#include <dirent.h>

#include <string>
#include <algorithm>
#include <WindowsManager.h>
#include "Singleton.h"


//SINGLETON_BEGIN(  FileBrowser )
class FileBrowser
{
    
    
public :
	inline static FileBrowser& getInstance() {
		if (!FileBrowser::s_pInstance)
			s_pInstance = new FileBrowser;

		return *s_pInstance;
	}

	inline static void DestroyInstance() {
		if(s_pInstance) delete s_pInstance;
		s_pInstance = 0;
	}

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
    void                        cache();
    bool                        keyboard(char key, int x, int y);

    inline bool                 getVisible()                        { return pW->getVisible(); }
    
    //void                        quit();


protected:
    PanelWindow*                pW;
    PanelText*                  panelDirName;
    PanelSimple*                panelDir;
    PanelSimple*                panelFile;
    
    PanelButton*                panelOK;
    PanelButton*                panelQuit;

    PanelEditText*              panelFilename;
    
    vector<PanelText*>          lDirs;
    vector<PanelText*>          lFile;
    
    vector<string>              tDirNames;
    vector<string>              tFileNames;
    
    string                      workingDir;
    string                      currentDir;
 
    int                         x, y, dx, dy;
   

private :
	static FileBrowser* s_pInstance;	// Instance de la classe

	FileBrowser(FileBrowser&);
	void operator =(FileBrowser&);

//SINGLETON_END()
};


#endif


