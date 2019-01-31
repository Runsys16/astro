#include "pleiade.h"


//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Pleiade::Pleiade()
{
    bChargingPleiades = true;
    Camera();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Pleiade::threadExtractImgPleiade()
{
    logf((char*)"Camera::threadExtractImg() start %s", (char*)getName());
    readB.ptr = WindowsManager::OpenImage( (const std::string)sPleiade, readB.w, readB.h, readB.d );

    bChargingPleiades = true;
    bFreePtr = true;
    //logf((char*)"Camera::threadExtractImg()  stop %s", (char*)getName());
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
std::thread Pleiade::memberThreadPleiade()
{
    //logf((char*)"Camera::memberThread() %s", (char*) getName() );
    return std::thread(&Pleiade::threadExtractImgPleiade, this); 
}  //--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Pleiade::change_background_camera(void)
{
    static int plus = 1;
    static int count_png = 0;

   //if ( pthread_chargement )   delete pthread_chargement;

    if ( bChargingPleiades ) 
    {
        char num[] = "000";
        
        
        sprintf( num, "%03d", count_png );
        sPleiade[56] = num[0];
        sPleiade[57] = num[1];
        sPleiade[58] = num[2];

        //panelPreView->deleteBackground();
        //pCamFilename->changeText( (char*) num );


        unsigned int w, h, d;
        
        if (bFreePtr)   free(ptr);
        bFreePtr = false;

        ptr = readB.ptr;
        w   = readB.w;
        h   = readB.h;
        d   = readB.d;
        
        panelPreview->setBackground( ptr, w, h, d);
        //panelPreView->setBackground( (char*)sPleiade);
        //unsigned int w,h,d;
        //ptr = _ImageTools::OpenImage( (const std::string)sPleiade, w, h, d );
        
        //printf( "%s\n", &sPleiade[56] );
        if ((count_png+=plus)>=119)    plus = -1;
        if ((count_png+=plus)<=30)      plus = 1;



        thread_chargement_pleiade = memberThreadPleiade();
        thread_chargement_pleiade.detach();

        bChargingPleiades = false;

        log((char*)"***************************change_background_pleiade()");
    }
    //log((char*)"change_background_pleiade()");

}


