#ifndef FILE_BROWSER_H
#define FILE_BROWSER_H  1

#include "main.h"
#include <dirent.h>

#include <string>
#include <algorithm>
#include <WindowsManager.h>
#include "Singleton.h"
#include "var_mgr.h"
#include "panel_dir.h"
#include "panel_file.h"

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
class PanelDir;
class PanelFile;
class ButtonOK;
class ButtonQUIT;
//SINGLETON_BEGIN(  FileBrowser )



//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
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
    
    
    void                        addImage( string, PanelSimple*, int, int);
    
    void                        affiche();
    void                        cache();
    bool                        keyboard(char key, int x, int y);
    
    void                        scrollDir( int );
    void                        scrollFile( int );

inline bool                     getVisible()                        { return pW->getVisible(); }

inline string                   getCurrentDir()                     { return currentDir; }
inline string                   getWorkingDir()                     { return workingDir; }
inline string                   setCurrentDir(string s)             { currentDir = string(s); }

protected:
    PanelWindow*                pW;
    PanelText*                  panelDirName;
    PanelDir*                   panelDir;
    PanelFile*                  panelFile;
    
    ButtonOK*                   panelOK;
    ButtonQUIT*                 panelQuit;

    PanelEditText*              panelFilename;
    
    vector<PanelText*>          lDirs;
    vector<PanelText*>          lFile;
    
    vector<string>              tDirNames;
    vector<string>              tFileNames;
    
    string                      workingDir;
    string                      currentDir;
 
    int                         x, y, dx, dy;

    int                         dirScroll;
    int                         fileScroll;
   

private :
	static FileBrowser* s_pInstance;	// Instance de la classe

	FileBrowser(FileBrowser&);
	void operator =(FileBrowser&);

//SINGLETON_END()
};
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
class ButtonOK : public PanelButton
{
protected:
    FileBrowser*                    pFB;
    
public:
    ButtonOK(FileBrowser*);

	virtual void		releaseLeft( int, int);
};
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
class ButtonQUIT : public PanelButton
{
protected:
    FileBrowser*                    pFB;
    
public:
    ButtonQUIT(FileBrowser*);

	virtual void		releaseLeft( int, int);
};
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#endif


