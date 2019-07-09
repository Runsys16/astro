#include "pleiade.h"


//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Pleiade::Pleiade()
{
    logf((char*)"----------- Constructeur Pleiade() -----------" );
    
    bCharged = true;
    
    //char  sTmp[] = "/home/rene/Documents/astronomie/tmp/test/suivi-20190103-000.png";
//                      0123456789012345678901234567890123456789012345678901234567890123456789
//                      00        10        20        30        40        50        60        -
    //sprintf( sPleiade, "%s", sTmp );

    sPleiades = "/home/rene/Documents/astronomie/tmp/test/suivi-20190103-";
    sPleiade = "/home/rene/Documents/astronomie/tmp/test/suivi-20190103-000.png";

    ptr = NULL;

    //Camera();
    setName( (char*) "Pleiades" );
    setDevName( (char*) "Pleiades" );
    vCameraSize.x = 1920;
    vCameraSize.y = 1080;
    bFreePtr = false;
    bFirst = true;
    bCharged=false;
    bCharging = false;
    count_png = 0;
    plus = 1;
    bNewBackground = false; 

    charge_background();
    change_background_camera();
    //bFreePtr = true;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Pleiade::~Pleiade()
{
    logf( (char*)"----------- Destructeur ~Pleiade() -----------" );
    //if ( thread_chargement_pleiade )  
    //while( bCharging );
         
    //thread_chargement_pleiade.join();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Pleiade::charge_background()
{
    bCharging = true;

    readBgr.ptr = WindowsManager::OpenImage( (const std::string)sPleiade, readBgr.w, readBgr.h, readBgr.d );
    //logf((char*)"change_background_pleiade() w=%d", readBgr.w);
    //logf((char*)"change_background_pleiade() h=%d", readBgr.h);

    bCharging = false;
    bCharged = true;
    bFreePtr = true;
    //logf((char*)"Camera::threadExtractImg()  stop %ld", readBgr.ptr);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Pleiade::update(void)
{
//    logf((char*)"Camera::update() -------------");
    panelPreview->setRB( &readBgr );
    panelPreview->update();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Pleiade::change_background_camera(void)
{
    //log((char*)"***************************change_background_pleiade()");
    
   //if ( pthread_chargement )   delete pthread_chargement;

    if ( bCharged && !bFirst ) 
    {
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


        ptr = readBgr.ptr;
        w   = readBgr.w;
        h   = readBgr.h;
        d   = readBgr.d;
        
        panelPreview->setBackground( ptr, w, h, d);
        //panelPreview->setRB( &readBgr );
        //logf((char*)"change_background_pleiade() w=%d", readBgr.w);
        bNewBackground = true;

        //panelPreView->setBackground( (char*)sPleiade);
        //unsigned int w,h,d;
        //ptr = _ImageTools::OpenImage( (const std::string)sPleiade, w, h, d );
        
        //printf( "%s\n", &sPleiade[56] );
        count_png += plus;
        if ( count_png>=119 )           plus = -1;
        if ( count_png<=30 )            plus = 1;



        //thread_chargement_pleiade = startThread();

        char num[] = "000";
        
        sprintf( num, "%03d", count_png );
        pCamFilename->changeText( num );
        //logf((char*)" num =  %s", (char*)num );
        //logf((char*)" namefile =  %s", (char*)sPleiade );
        //sPleiade[56] = num[0];
        //sPleiade[57] = num[1];
        //sPleiade[58] = num[2];
        sPleiade = sPleiades + string(num) + ".png";
        

        thread_chargement_pleiade = std::thread(&Pleiade::charge_background, this);
        thread_chargement_pleiade.detach();

        bCharged = false;

        //log((char*)"***************************change_background_pleiade()");

        //-------------------------------------------------------
        // Calcul du temps
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




