#include "pleiade.h"


//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Pleiade::Pleiade()
{
    logf((char*)"----------- Constructeur Pleiade() -----------" );
    
    
    //char  sTmp[] = "/home/rene/Documents/astronomie/tmp/test/suivi-20190103-000.png";
//                      0123456789012345678901234567890123456789012345678901234567890123456789
//                      00        10        20        30        40        50        60        -
    //sprintf( sPleiade, "%s", sTmp );

    sPleiades = "/home/rene/Documents/astronomie/tmp/test/suivi-20190103-";
    sPleiade  = "/home/rene/Documents/astronomie/tmp/test/suivi-20190103-000.png";

    ptr = NULL;

    //Camera();
    setName( (char*) "Pleiades" );
    setDevName( (char*) "Pleiades" );
    vCameraSize.x   = 1920;
    vCameraSize.y   = 1080;
    bFreePtr        = false;
    bFirst          = true;
    //bCharged        =false;
    bCharging       = false;
    count_png       = 0;
    plus            = 1;
    bNewBackground  = false; 

    setDevName( (char*)"/dev/pleiades" );
    //charge_background();
    //change_background_camera();
    
    if( bPause )                bOneFrame = true;

    //bFreePtr = true;
    start_thread();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Pleiade::~Pleiade()
{
    logf( (char*)"----------- Destructeur ~Pleiade() -----------" );
    //if ( thread_chargement )  
    //while( bCharging );
         
    //thread_chargement.join();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Pleiade::start_thread()
{
    if ( !bStartThread )
    {
        thread_chargement = thread(&Pleiade::charge_background, this);
        thread_chargement.detach();

        bStartThread = true;
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Pleiade::charge_background()
{
    //while( bPause && bOneFrame);
    
    while( true )
    {
        if ( ( !bPause || bOneFrame ) && bCharging == false ) 
        {    
            //logf((char*)"[thread Pleiade::charge_background()] lit une frame");
            unsigned int w, h, d;
            readBgr.ptr = WindowsManager::OpenImage( (const std::string)sPleiade, w, h, d );
            readBgr.w = w;
            readBgr.h = h,
            readBgr.d = d;
            
            bCharging = true;
            bFreePtr = true;
            bOneFrame = false;
            //logf((char*)"[thread Pleiade::charge_background()] ptr = %lX", readBgr.ptr);
        }
        
    }

    logf( (char*)"[WARNING] [thread Pleiade::charge_background()] Fin du thread Pleiade::charge_background" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Pleiade::change_background(void)
{
    //logf((char*)"Pleiade::change_background()");
    
    if ( bCharging ) 
    {
        logf((char*)"Pleiade::change_background()");
        //logf((char*)"|  lecture d'une frame" );
        
        panelPreview->deleteBackground();
        
        /*
        if ( bFirst )       logf( (char*)"bFirst=true" );
        else                logf( (char*)"bFirst=false" );
        */

        if ( bFirst && bFreePtr )
        {
            bFirst = false;
            bFreePtr = false;
        }

        if (bFreePtr && ptr!=NULL)   free(ptr);
        //bFreePtr = false;

        unsigned int w, h, d;


        ptr = readBgr.ptr.load();
        w   = readBgr.w.load();
        h   = readBgr.h.load();
        d   = readBgr.d.load();
        
        //logf((char*)"|  change le background de panelPreview" );
        panelPreview->setBackground( ptr, w, h, d);
        panelPreview->setRB( &readBgr);

        bNewBackground = true;

        char num[55];
        
        sprintf( num, "%03d", count_png );
        string titre = "suivi-20190103-" + string(num) + ".png";
        panelPreview->setExtraString( string(titre) );
        
        pCamFilename->changeText( (char*)titre.c_str() );

        sPleiade = sPleiades + string(num) + ".png";

        count_png += plus;
        if ( count_png>=119 )           plus = -1;
        if ( count_png<= 30 )            plus = 1;

        logf((char*)"|  (%d, %d) Nom du fichier  %s", pCamFilename->getPosX(), pCamFilename->getPosY(), (char*)titre.c_str() );
        logf((char*)"|  (%d, %d)", pCamFilename->getX(), pCamFilename->getY() );

        //-------------------------------------------------------
        // Calcul du temps pour l'affiche de la frequence d'image
        //-------------------------------------------------------
        float t = Timer::getInstance().getCurrentTime();
        if ( previousTime != -1 )
        {
            if ( nb_images++ >= 10 )
            {
                hz = 10.0 / (t - previousTime);
                previousTime = t;
                nb_images = 0;
            }
            //logf((char*)"Camera::change_background_camera()   Hz=%.0f %s", hz, getName() );
        }
        else
            previousTime = t;
        //-------------------------------------------------------
        

        bCharging = false;
        //logf((char*)"|  bCharging %s", BOOL2STR(bCharging) );

    }
    //log((char*)"change_background_pleiade()");
    if ( bFirst )       bFirst = false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
GLubyte* Pleiade::getPtr()
{
    return (GLubyte*)ptr;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool  Pleiade::haveNewFrame()
{
    return bNewBackground;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Pleiade::haveUseFrame(bool b)
{
    bNewBackground = false;
}




