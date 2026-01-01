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
#include "button_callback.h"

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
class PanelDir;
class PanelFile;
class ButtonOK;
class ButtonQUIT;
class ButtonCallBack;
//SINGLETON_BEGIN(  FileBrowser )



//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
class FileBrowser
{
protected:
    PanelWindow*                pW;
    PanelDir*                   panelDir;
    PanelFile*                  panelFile;
    PanelSimple*                panelInverseDir;
    PanelSimple*                panelInverseFile;
    
    ButtonOK*                   panelOK;
    ButtonQUIT*                 panelQuit;

    PanelEditText*              panelFilename;
    PanelEditText*              panelDirName;
    PanelText*                  panelDbug;
    
    vector<PanelText*>          lDirs;
    vector<PanelText*>          lFile;
    
    vector<string>              tDirNames;
    vector<string>              tFileNames;
    
    string                      workingDir;
    string                      currentDir;
    string                      filtre;
 
    int                         x, y, dx, dy;

    int                         dirScroll;
    int                         fileScroll;
    
    bool                        bNewline;
    unsigned long               color;
    
    PanelText*					pOldDir;
    PanelText*					pOldFile;
    

private :
	static FileBrowser* s_pInstance;	// Instance de la classe

	FileBrowser(FileBrowser&);
	void operator =(FileBrowser&);
    
    
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

	bool						existe_remonte_dir();

    void                        explore_dir();
    void                        change_dir( int );
    void                        change_dir( string );
    
    bool                        isInsideDir(int, int);
    bool                        isInsideFile(int, int);

    vector<string>&             getDirNames()                       { return tDirNames; }
    vector<string>&             getFilesNames()                     { return tFileNames; }
    
    
    void                        addImage( string, PanelSimple*, int, int);
    
    void                        affiche();
    void                        cache();
    void                        supCallBacks();
    bool                        keyboard(char key, int x, int y);
    
    void                        scrollDir( int );
    void                        scrollFile( int );

	void						mouseOverDir(int, int);
	void						mouseOverFile(int, int);

    void                        setCallBackOK(ButtonCallBack* p);
    void                        setCallBackQUIT(ButtonCallBack* p);
    void                        setCallBack(ButtonCallBack* p);
    void                        setExtra( int );
    void                        setColor( unsigned long );
    void                        setFocus();
    void                        setFilename( string );
    
inline bool                     getVisible()                        { return pW->getVisible(); }

inline string                   getFilename()                       { return panelFilename->getText(); }
inline string                   getCurrentDir()                     { return currentDir; }
inline string                   getWorkingDir()                     { return workingDir; }
inline void                     setCurrentDir(string s)             { currentDir = string(s); }
inline void                     setFiltre(string s)                 { filtre = string(s); }

inline void                     setNewline(bool b)                  { bNewline = b; }


//SINGLETON_END()
};
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
class ButtonOK : public PanelButton
{
protected:
    FileBrowser*                    pFB;
    ButtonCallBack*                 pCallBack;
    
    
public:
    ButtonOK(FileBrowser*);
    void                    setCallBack( ButtonCallBack*p )         { pCallBack = p; }            
	virtual void		    releaseLeft( int, int);
    inline  void            setExtra( int ii )                      { pCallBack->setExtra(ii); }
    inline  int             getExtra()                              { return pCallBack->getExtra(); }

    inline ButtonCallBack*  getCallback()                           { return pCallBack; }
};
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
class ButtonQUIT : public PanelButton
{
protected:
    FileBrowser*                    pFB;
    ButtonCallBack*                 pCallBack;
    
public:
    ButtonQUIT(FileBrowser*);

    void                    setCallBack( ButtonCallBack*p )         { pCallBack = p; }            
	virtual void		    releaseLeft( int, int);
    inline  void            setExtra( int ii )                      { pCallBack->setExtra(ii); }
    inline  int             getExtra()                              { return pCallBack->getExtra(); }
    inline ButtonCallBack*  getCallback()                           { return pCallBack; }
};
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#endif


