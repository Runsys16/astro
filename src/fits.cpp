#include "fits.h"
//--------------------------------------------------------------------------------------------------------------------
//
// Lecture des fichiers FITS (Flexible Image Transport System)
//		https://www.aanda.org/articles/aa/full_html/2010/16/aa15362-10/aa15362-10.html
//		https://fits.gsfc.nasa.gov/standard30/fits_standard30.pdf
//
//--------------------------------------------------------------------------------------------------------------------
Fits::Fits(string filename)
{
    logf((char*)"Constructeur Fits::Fits() -------------" );
    log_tab(true);
    readBgr.ptr = NULL;
    _filename = filename;
    bValid = false;
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
    dBZERO  = -1.0;
    dBSCALE = -1.0;
    iOFFSET = -1;

	int n = 0;
    chargeHDU(n);
    if ( haveKey( "EXTEND" )	)		chargeHDU(++n);
    
    chargeTexture(++n);
    
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
void Fits::chargeHDU(int n)
{
    //_filename = filename;

    logf( (char*)"Fits::chargeHDU(%d)", n );

    std::ifstream fichier;
    fichier.open(_filename, std::ios_base::app);

    if ( !fichier ) 
    {
        logf( (char*)"[ERROR]impossble d'ouvrir : '%s'", (char*)_filename.c_str() );
        return;
    }

    fichier.seekg( 0, fichier.end );
    int lenght = fichier.tellg();
    fichier.seekg( (long)n*LENGTH_HDU, fichier.beg );

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
        
      	/*
        if ( i==0 )
        {
            if ( k.find("SIMPLE") == std::string::npos )
            {
                logf( (char*)"[ERREUR] Ce n'est pas un fichier FITS standard 'SIMPLE' non trouve" );
                return;
            }
        }
        */
        
        for( int j=10; j<80; j++ )
        {
            v = v + buffer[i*80+j];
        }

        logf( (char*)"Lecture de  : '%s' : '%s'", (char*)k.c_str() ,(char*)v.c_str() );
        
             if ( k.find("BITPIX") == 0 )               readBITPIX(v);
        else if ( k.find("NAXIS")  == 0 )               readNAXIS( k, v );
        else if ( k.find("CR")     == 0 )               readCR( k, v );
        else if ( k.find("CD")     == 0 )               readCD( k, v );
        else if ( k.find("BZERO")  == 0 )               dBZERO = getDouble( v );
        else if ( k.find("BSCALE") == 0 )               dBSCALE = getDouble( v );
        else if ( k.find("OFFSET") == 0 )               iOFFSET = getInt( v );
        
        row r;
        r.key = k;
        r.value = v;
        datas.push_back( r );
        
        if ( k.find("END") == 0 )       break;
    }


    fichier.close();
    
	afficheDic();
}
//--------------------------------------------------------------------------------------------------------------------
// Pour une image en 8eb par couleur
// Lit une couleur du fichier fit, et applique les corrections si nessecaire
//--------------------------------------------------------------------------------------------------------------------
void Fits::read_RGB_8( uint16_t &C, uint8_t* pBuffer )
{
	C = *pBuffer;

	if ( dBZERO!=-1 && dBSCALE!=-1 )	C = ((uint16_t)(( (double)(C) ) * dBSCALE + dBZERO) & 0xFF00) >> 8;
	if ( iOFFSET != -1 )				C -= iOFFSET;
}
//--------------------------------------------------------------------------------------------------------------------
// Pour une image en 16eb par couleur
// Lit une couleur du fichier fit, et applique les corrections si nessecaire
//--------------------------------------------------------------------------------------------------------------------
void Fits::read_RGB_16( uint16_t &C, uint16_t* pBuffer )
{
	C = *pBuffer;

	if ( dBZERO!=-1 && dBSCALE!=-1 )	C = ((uint16_t)(( (double)(C) ) * dBSCALE + dBZERO) & 0xFF00) >> 8;
	if ( iOFFSET != -1 )				C -= iOFFSET;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Fits::chargeTexture(int nHDU)
{
	//unsigned int  nbOctet = ( nBITPIX == 16 ? 2 : 1);
    //unsigned long l_fit = (long)nNAXIS * (long)nNAXIS1 * (long)nNAXIS2 * (long)nbOctet;
    size_fit   = (long)nNAXIS1 * (long)nNAXIS2;
    size_gl    = (long)nNAXIS1 * (long)nNAXIS2;
    unsigned long l_fit = (long)nNAXIS * size_fit;
    unsigned long l_gl  = (long)nNAXIS * size_fit;
    
    logf( (char*)"Fits::chargeTexture()" ); 
    logf( (char*)"Longueur des buffer : fits %ld, opengl  %ld", (long)l_fit, (long)l_gl ); 
	//---------------------------------------------------
	// Mise en place la structure readBackground
    if ( l_fit>0 && l_gl>0 )      
    {
        bValid = true;
        readBgr.w = nNAXIS1;
        readBgr.h = nNAXIS2;
        readBgr.d = nNAXIS;
    }
	//---------------------------------------------------
	// Allocation des buffer
    GLubyte* 		pBuffer_gl = (GLubyte*)malloc( l_gl );
    uint8_t* 		pBuffer_8  = NULL;
    uint16_t* 		pBuffer_16 = NULL;

	if 		( nBITPIX == 16 )		pBuffer_16  = (uint16_t*)malloc( l_fit * sizeof(uint16_t) );
	else if	( nBITPIX == 8  )		pBuffer_8   = (uint8_t*) malloc( l_fit * sizeof(uint8_t)  );
    else {
        logf( (char*)"[ERROR] Nompbre de bits par pixel non pris en charge ..." );
        return;
    }
	//---------------------------------------------------
    if ( pBuffer_gl == NULL ) 		{
        logf( (char*)"[ERROR] Impossible d\'alloue la memoire ..." );
        return;
    }
    if (  pBuffer_8 == NULL && pBuffer_16 == NULL ) 		{
        logf( (char*)"[ERROR] Impossible d\'alloue la memoire ..." );
        return;
    }
	//---------------------------------------------------
	// Ouvrir le fichier
    std::ifstream fichier;
    fichier.open(_filename, std::ios_base::app);
	//---------------------------------------------------
    if ( !fichier ) 		{
        logf( (char*)"[ERROR]impossble d'ouvrir : '%s'", (char*)_filename.c_str() );
        return;
    }
	//---------------------------------------------------
    // se placer apres l'entete HDU
    fichier.seekg( LENGTH_HDU*nHDU, fichier.beg );
	//---------------------------------------------------
    // lecture des plans memoires
	if 		( nBITPIX == 16 )		fichier.read( (char*)pBuffer_16, l_fit * sizeof(uint16_t) );
	else if	( nBITPIX == 8  )		fichier.read( (char*)pBuffer_8,  l_fit * sizeof(uint8_t)  );
    fichier.close();    
	//---------------------------------------------------
    // Buffer sous la forme  NAXIS = 3   (3 plans)
    //   NAXIS0     NAXIS1     NAXIS2        Buffer GL
    // RRRRRRRRRR GGGGGGGGGG BBBBBBBBBB => RGBRGBRGB

	//---------------------------------------------------
	// Pour tout les plans
	//    Pout tout le contenu d'un plan
	//
	// remplissage du pointeur pBuffer_gl alias le pointeur readBackground
	unsigned long	p=0;
    uint16_t 		R, G, B;
	long			i_fit, i_gl;
	
    //if ( nBITPIX == 16 )	i_fit = 0;
    //else		        	i_fit = 0;
    
    i_fit = 0;
    i_gl = 0;
	
    for( long i_gl=0; i_gl<size_gl; i_gl++ )    {

        if ( nBITPIX == 16 )			
        {
            uint16_t RR, GG, BB;
            
            read_RGB_16( R, pBuffer_16 + 0*size_fit + i_fit );
            read_RGB_16( G, pBuffer_16 + 1*size_fit + i_fit );
            read_RGB_16( B, pBuffer_16 + 2*size_fit + i_fit );
		}
		else  if ( nBITPIX == 8  )	
		{
            read_RGB_8( R, pBuffer_8 + 0*size_fit + i_fit );
            read_RGB_8( G, pBuffer_8 + 0*size_fit + i_fit );
            read_RGB_8( B, pBuffer_8 + 0*size_fit + i_fit );
        }
    
		i_fit++;
        
        pBuffer_gl[i_gl*3 + 0] = (GLubyte)R;
        pBuffer_gl[i_gl*3 + 1] = (GLubyte)G;
        pBuffer_gl[i_gl*3 + 2] = (GLubyte)B;
    }
    //------------------------------------------------
	// Liberation de la memoire
	if 		( nBITPIX == 16 )		free( pBuffer_16 );
	else if	( nBITPIX == 8  )		free( pBuffer_8  );
	// Affectation dans la memoire gl de l'image affiché
    readBgr.ptr = pBuffer_gl;
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
    logf( (char*)"Affichage des donnees FITS" );

    int nb = datas.size();
    for( int i=0; i<nb; i++ )
    {
        logf( (char*)" - %s \t: %s", (char*)datas[i].key.c_str(), (char*)datas[i].value.c_str() );
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool Fits::haveKey( string k )
{
    logf( (char*)"Affichage des donnees FITS" );

    int nb = datas.size();
    for( int i=0; i<nb; i++ )
    {
        if ( datas[i].key.find(k) == 0)		return true;
    }
    return false;
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
    logf( (char*)" - dCD2_2 %f", dCD2_2 );

    logf( (char*)" - dBZERO %f", dBZERO );
    logf( (char*)" - dBSCALE %f", dBSCALE );

    logf( (char*)" - iOFFSET %d", iOFFSET );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

