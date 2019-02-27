#include "pleiade.h"


//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Pleiade::Pleiade()
{
    logf((char*)"----------- Constructeur Pleiade() -----------" );
    
    bChargingPleiades = true;
    char  sTmp[] = "/home/rene/Documents/astronomie/tmp/test/suivi-20190103-000.png";
    
    sprintf( sPleiade, "%s", sTmp );
//                      0123456789012345678901234567890123456789012345678901234567890123456789
//                      00        10        20        30        40        50        60        -

    ptr = NULL;

    //Camera();
    setName( (char*) "Pleiades" );
    setDevName( (char*) "Pleiades" );
    vCameraSize.x = 1920;
    vCameraSize.y = 1080;
    bFreePtr = false;
    bFirst = true;
    bChargingPleiades=false;
    count_png = 0;
    plus = 1;
    bNewBackground = false; 

    thread_chargement_pleiade = std::thread(&Pleiade::threadExtractImgPleiade, this);
    //thread_chargement_pleiade = startThread();
    thread_chargement_pleiade.detach();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Pleiade::threadExtractImgPleiade()
{
    //logf((char*)"Pleiade::threadExtractImgPleiade() start %s", (char*)getName());
    readB.ptr = WindowsManager::OpenImage( (const std::string)sPleiade, readB.w, readB.h, readB.d );

    bChargingPleiades = true;
    bFreePtr = true;
    //logf((char*)"Camera::threadExtractImg()  stop %ld", readB.ptr);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
std::thread Pleiade::startThread()
{
    //logf((char*)"Pleiade::startThread() %s", (char*) getName() );
    return std::thread(&Pleiade::threadExtractImgPleiade, this); 
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Pleiade::change_background_camera(void)
{
    //log((char*)"***************************change_background_pleiade()");
    
   //if ( pthread_chargement )   delete pthread_chargement;

    if ( bChargingPleiades && !bFirst ) 
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

        char num[] = "000";
        
        sprintf( num, "%03d", count_png );
        pCamFilename->changeText( num );
        //logf((char*)" num =  %s", (char*)num );
        //logf((char*)" namefile =  %s", (char*)sPleiade );
        sPleiade[56] = num[0];
        sPleiade[57] = num[1];
        sPleiade[58] = num[2];

        unsigned int w, h, d;


        ptr = readB.ptr;
        w   = readB.w;
        h   = readB.h;
        d   = readB.d;
        
        panelPreview->setBackground( ptr, w, h, d);
        bNewBackground = true;

        //panelPreView->setBackground( (char*)sPleiade);
        //unsigned int w,h,d;
        //ptr = _ImageTools::OpenImage( (const std::string)sPleiade, w, h, d );
        
        //printf( "%s\n", &sPleiade[56] );
        count_png += plus;
        if ( count_png>=119 )           plus = -1;
        if ( count_png<=30 )            plus = 1;



        //thread_chargement_pleiade = startThread();
        thread_chargement_pleiade = std::thread(&Pleiade::threadExtractImgPleiade, this);
        thread_chargement_pleiade.detach();

        bChargingPleiades = false;

        //log((char*)"***************************change_background_pleiade()");
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




