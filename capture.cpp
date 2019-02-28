#include "capture.h"



static int                      num;
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Capture::Capture()
{
    logf((char*)"----------- Constructeur Capture() -----------" );


    pooling();

    if ( filenames.size() != 0 )
    {
        num = ++num % filenames.size();
        filename = string( filenames[num] );
    }
    else
    {
        filename = "";
        num = -1;
    }

    create_preview();

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Capture::~Capture()
{
    logf((char*)"----------- Destructeur Capture() -----------" );

	WindowsManager::getInstance().sup( panelPreview );
	delete pTitre;
	delete panelPreview;
	filenames.clear();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::pooling()
{
    struct dirent *lecture;
    DIR *rep;
    string dirname = getCurrentDirectory();
    rep = opendir( dirname.c_str() );
    filenames.clear();
    
    
    while ((lecture = readdir(rep))) {

        string s;
        int found;
        s = lecture->d_name;
        
        if ( s.find(".png") != string::npos || s.find(".jpg") != string::npos)
        {
            string  f =  dirname + s;
            //logf( (char*)"%s", (char*)f.c_str() );
            filenames.push_back( f );
        }
        
    }

    closedir(rep);    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::create_preview()	{
    logf((char*)"----------- CreatePreview ------------- %s", (char*)filename.c_str() );

	WindowsManager& wm = WindowsManager::getInstance();

	//panelPreview = new PanelSimple();
	panelPreview = new PanelWindow();

    int x, y, dx, dy;
    dx = getWidth() - 20;
    dy = getHeight()- 20;

    if ( filename.length() !=  0 )
    {
        readBgr.ptr = WindowsManager::OpenImage( (const std::string)filename, readBgr.w, readBgr.h, readBgr.d );
        panelPreview->setBackground( readBgr.ptr, readBgr.w, readBgr.h, readBgr.d);
    
	
        float ratioX = (float)dx / (float)readBgr.w;
        float ratioY = (float)dy / (float)readBgr.h;
        if  ( ratioX < ratioY ) 
        {
            dx = readBgr.w * ratioX;
            dy = readBgr.h * ratioX;
        }
        else
        {
            dx = readBgr.w * ratioY;
            dy = readBgr.h * ratioY;
        }
    }
    else
    {
        dx = 800;
        dy = 30;
        filename = "pas de fichier dans " + getCurrentDirectory();
    }
    x = (getWidth()-dx)/2;
    y = (getHeight()-dy)/2;
    
    panelPreview->setPosAndSize( x, y, dx, dy );


	pTitre = new PanelText( (char*)filename.c_str(),		PanelText::LARGE_FONT, 20, 10 );
	panelPreview->add( pTitre );
	
	
 	wm.add( panelPreview );

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------




