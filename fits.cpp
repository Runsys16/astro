#include "fits.h"

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Fits::Fits(string filename)
{
    logf((char*)"Constructeur Fits::Fits() -------------" );
    log_tab(true);
    readBgr.ptr = NULL;
    _filename = filename;
    bValid = false;

    charge(filename);
    chargeTexture();
    
    //afficheDic();
    //afficheDatas();
    dCRPIX1 = -1.0;
    dCRVAL1 = -1.0;
    dCRPIX2 = -1.0;
    dCRVAL2 = -1.0;
    dCD1_1  = -1.0;
    dCD1_2  = -1.0;
    dCD2_1  = -1.0;
    dCD2_2  = -1.0;
    log_tab(false);
    logf((char*)"Constructeur Fits::Fits() -------------" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Fits::~Fits()
{
    if ( readBgr.ptr != NULL )          free( readBgr.ptr );
    readBgr.ptr = NULL;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Fits::charge(string filename)
{
    _filename = filename;

    logf( (char*)"Chargement des valeurs dans '%s'", (char*)filename.c_str() );

    std::ifstream fichier;
    fichier.open(_filename, std::ios_base::app);

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

             if ( k.find("BITPIX") == 0 )               readBITPIX(v);
        else if ( k.find("NAXIS")  == 0 )               readNAXIS( k, v );
        else if ( k.find("CR")     == 0 )               readCR( k, v );
        else if ( k.find("CD")     == 0 )               readCD( k, v );
        
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
void Fits::chargeTexture()
{
    unsigned long l = (long)nNAXIS * (long)nNAXIS1 * (long)nNAXIS2;
    
    logf( (char*)"Longueur du buffer : %ld = %ld x %ld x %ld", l, (long)nNAXIS, (long)nNAXIS1, (long)nNAXIS2 ); 

    if ( l>0 )      
    {
        bValid = true;
        readBgr.w = nNAXIS1;
        readBgr.h = nNAXIS2;
        readBgr.d = nNAXIS;
    }

    GLubyte* pBuffer;
    pBuffer = (GLubyte*)malloc( l );
    GLubyte* pBuffer0;
    pBuffer0 = (GLubyte*)malloc( l );

    if (  pBuffer == NULL || pBuffer0 == NULL ) 
    {
        logf( (char*)"[ERROR] Impossible d\'alloue la memoire ..." );
        return;
    }

    std::ifstream fichier;
    fichier.open(_filename, std::ios_base::app);
    if ( !fichier ) 
    {
        logf( (char*)"[ERROR]impossble d'ouvrir : '%s'", (char*)_filename.c_str() );
        return;
    }


    fichier.seekg( LENGTH_HDU, fichier.beg );

    fichier.read((char*)pBuffer, l );
    fichier.close();    

    // Buffer sous la forme
    // RRRGGGBBB => RGBRGBRGB

    
    unsigned long s = (long)nNAXIS1 * (long)nNAXIS2;


    for (unsigned long p=0; p<nNAXIS; p++ )
    {
        for( unsigned long ll=0; ll<s; ll++ )
        {
            GLubyte R, G, B;
            R = pBuffer[ll+ p*nNAXIS];
            //G = pBuffer[ll+1*s];
            //B = pBuffer[ll+2*s];
            
            pBuffer0[ll* (nNAXIS) + p] = R;
            //pBuffer0[ll*3+1] = G;
            //pBuffer0[ll*3+2] = B;   
        }
    }

    free( pBuffer );
    readBgr.ptr = pBuffer0;
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
    p->ptr = readBgr.ptr.load();
    p->w   = readBgr.w.load();
    p->h   = readBgr.h.load();
    p->d   = readBgr.d.load();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Fits::afficheDic()
{
    logf( (char*)"Affichage des donnes FITS" );

    int nb = datas.size();
    for( int i=0; i<nb; i++ )
    {
        logf( (char*)" - %s : %s", (char*)datas[i].key.c_str(), (char*)datas[i].value.c_str() );
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Fits::afficheDatas()
{
    logf( (char*)"Resultat parser" );
    logf( (char*)" - nBITPIX %d", nBITPIX );    
    logf( (char*)" - nNAXIS  %d", nNAXIS );    
    logf( (char*)" - nNAXIS1 %d", nNAXIS1 );    
    logf( (char*)" - nNAXIS2 %d", nNAXIS2 );    
    logf( (char*)" - nNAXIS3 %d", nNAXIS3 );    

    logf( (char*)" - dCRPIX1 %f", dCRPIX1 );    
    logf( (char*)" - dCRVAL1 %f", dCRVAL1 );    

    logf( (char*)" - dCRPIX2 %f", dCRPIX2 );    
    logf( (char*)" - dCRVAL2 %f", dCRVAL2 );    

    logf( (char*)" - dCD1_1 %f", dCD1_1 );    
    logf( (char*)" - dCD1_2 %f", dCD1_2 );    

    logf( (char*)" - dCD2_1 %f", dCD2_1 );    
    logf( (char*)" - dCD2_2 %f", dCD2_2 );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

