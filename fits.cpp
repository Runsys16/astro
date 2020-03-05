#include "fits.h"

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Fits::Fits(string filename)
{
    readBgr.ptr = NULL;
    _filename = filename;
    bValid = false;

    charge(filename);
    /*
    logf( (char*)" nBITPIX %d", nBITPIX );    

    logf( (char*)" nNAXIS  %d", nNAXIS );    
    logf( (char*)" nNAXIS1 %d", nNAXIS1 );    
    logf( (char*)" nNAXIS2 %d", nNAXIS2 );    
    logf( (char*)" nNAXIS3 %d", nNAXIS3 );    

    logf( (char*)" dCRPIX1 %f", dCRPIX1 );    
    logf( (char*)" dCRVAL1 %f", dCRVAL1 );    

    logf( (char*)" dCRPIX2 %f", dCRPIX2 );    
    logf( (char*)" dCRVAL2 %f", dCRVAL2 );    

    logf( (char*)" dCD1_1 %f", dCD1_1 );    
    logf( (char*)" dCD1_2 %f", dCD1_2 );    

    logf( (char*)" dCD2_1 %f", dCD2_1 );    
    logf( (char*)" dCD2_2 %f", dCD2_2 );
    */
    
    afficheDatas();
    unsigned long l = (long)nNAXIS * (long)nNAXIS1 * (long)nNAXIS2;
    
    logf( (char*)"Longueur du buffer : %ld", l ); 

    if ( l>0 )      
    {
        bValid = true;
        readBgr.w = nNAXIS1;
        readBgr.h = nNAXIS2;
        readBgr.d = nNAXIS;
    }

    GLubyte* pBuffer;
    pBuffer = (GLubyte*)malloc( l );

    std::ifstream fichier;
    fichier.open(filename, std::ios_base::app);
    if ( !fichier ) 
    {
        logf( (char*)"[ERROR]impossble d'ouvrir : '%s'", (char*)filename.c_str() );
        return;
    }


    fichier.seekg( LENGTH_HDU, fichier.beg );

    fichier.read(pBuffer, l );
    fichier.close();    


    GLubyte* pBuffer0;
    pBuffer0 = (GLubyte*)malloc( l );
    
    unsigned long s = (long)nNAXIS1 * (long)nNAXIS2;

    for( unsigned long ll=0; ll<s; ll++ )
    {
    
        GLubyte R, G, B;
        R = pBuffer[ll+0];
        G = pBuffer[ll+s];
        B = pBuffer[ll+2*s];
        
        pBuffer0[ll*3+0] = R;
        pBuffer0[ll*3+1] = G;
        pBuffer0[ll*3+2] = B;
        
    }

    free( pBuffer );
    readBgr.ptr = pBuffer0;


}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Fits::charge(string filename)
{
    _filename = filename;

    logf( (char*)"Chargement des valeurs dans '%s'", (char*)filename.c_str() );

    std::ifstream fichier;
    fichier.open(filename, std::ios_base::app);

    if ( !fichier ) 
    {
        logf( (char*)"[ERROR]impossble d'ouvrir : '%s'", (char*)filename.c_str() );
        return;
    }

    fichier.seekg( 0, fichier.end );
    int lenght = fichier.tellg();
    fichier.seekg( 0, fichier.beg );

    char buffer[LENGTH_HDU];
    fichier.read(buffer, LENGTH_HDU);
    
    for( int i=0; i<(LENGTH_HDU/80); i++ )
    {
        string k = "";
        string v = "";
        for( int j=0; j<8; j++ )
        {
            k = k + buffer[i*80+j];
        }
        
        if ( i==0 )
        {
            if ( k.find("SIMPLE") == std::string::npos )
            {
                logf( (char*)"[ERREUR] Ce n'est pas unfichier FITS standard 'SIMPLE' non trouve" );
                return;
            }
        }
        

        for( int j=10; j<80; j++ )
        {
            v = v + buffer[i*80+j];
        }
        //logf( (char*)"%s : %s", (char*)k.c_str(), (char*)v.c_str() );

             if ( k.find("BITPIX") != std::string::npos )           readBITPIX(v);
        else if ( k.find("NAXIS")  != std::string::npos )           readNAXIS( k, v );
        else if ( k.find("CR")     == 0 )                           readCR( k, v );
        else if ( k.find("CD")     == 0 )                           readCD( k, v );
        
        row r;
        r.key = k;
        r.value = v;
        datas.push_back( r );
        
        if ( k.find("END") == 0 )       break;
    }


    fichier.close();
    

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int Fits::getInt(string value )
{
    return stoi( value );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
double Fits::getDouble(string value )
{
    std::string::size_type sz;
    return stod( value, NULL );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Fits::readBITPIX( string value )
{
    nBITPIX = getInt(value);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Fits::readNAXIS( string key, string value )
{
            if ( key.find("NAXIS ") == 0 )          nNAXIS  = getInt( value );   
    else    if ( key.find("NAXIS1") == 0 )          nNAXIS1 = getInt( value );   
    else    if ( key.find("NAXIS2") == 0 )          nNAXIS2 = getInt( value );   
    else    if ( key.find("NAXIS3") == 0 )          nNAXIS3 = getInt( value );   
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Fits::readCR( string key, string value )
{
            if ( key.find("CRVAL1") == 0 )          dCRVAL1  = getDouble( value );   
    else    if ( key.find("CRVAL2") == 0 )          dCRVAL2  = getDouble( value );
    else    if ( key.find("CRPIX1") == 0 )          dCRPIX1  = getDouble( value );
    else    if ( key.find("CRPIX2") == 0 )          dCRPIX2  = getDouble( value );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Fits::readCD( string key, string value )
{
            if ( key.find("CD1_1") == 0 )           dCD1_1  = getDouble( value );   
    else    if ( key.find("CD1_2") == 0 )           dCD1_2  = getDouble( value );
    else    if ( key.find("CD2_1") == 0 )           dCD2_1  = getDouble( value );
    else    if ( key.find("CD2_2") == 0 )           dCD2_2  = getDouble( value );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Fits::getRB( struct readBackground* p )
{
    p->ptr = readBgr.ptr;
    p->w   = readBgr.w;
    p->h   = readBgr.h;
    p->d   = readBgr.d;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Fits::afficheDatas()
{
    int nb = datas.size();
    for( int i=0; i<nb; i++ )
    {
        logf( (char*)"%s : %s", (char*)datas[i].key.c_str(), (char*)datas[i].value.c_str() );
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

