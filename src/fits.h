#ifndef FITS_H
#define FITS_H  1

#include "main.h"
#include <dirent.h>

#include <string>
#include <algorithm>

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#define LENGTH_HDU      2880



//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
class Fits
{
protected:
        struct readBackground       readBgr;
        bool                        bValid;
        string                      _filename;

        typedef 	struct
	    {
	        string      key;
	        string      value; 
	    }row;
	    
	    uint32_t					size_fit;
	    uint32_t					size_gl;
        vector<row>                 datas;
                
        int                         nNAXIS;
        int                         nNAXIS1;
        int                         nNAXIS2;
        int                         nNAXIS3;
        int                         nBITPIX;
        int                         nDATE;

        string                      sCTYPE1;
        string                      sCTYPE2;
        
        double                      dCRVAL1;
        double                      dCRVAL2;

        double                      dCRPIX1;
        double                      dCRPIX2;
        
        double                      dCD1_1;
        double                      dCD1_2;
        double                      dCD2_1;
        double                      dCD2_2;
        
        double						dBZERO;
        double						dBSCALE;
        
        int16_t						iOFFSET;
public :
        Fits(string);
        ~Fits();
        
        void                        chargeHDU(int);
        void						read_RGB_8(  uint16_t&, uint8_t* );
        void						read_RGB_16( uint16_t&, uint16_t* );
        void                        chargeTexture(int);
        
        int                         getInt( string );
        double                      getDouble( string );
        string                      getString( string );
        
        void                        readBITPIX( string );
        void                        readNAXIS( string, string );
        void                        readCR( string, string );
        void                        readCD( string, string );
        void                        readCTYPE( string, string );

        void                        afficheDic();
		bool						haveKey( string );
        void                        afficheDatas();

inline  GLubyte*                    getPTR()                { return (bValid ?  readBgr.ptr.load() : NULL); };
inline  int                         getW()                  { return (bValid ?  readBgr.w.load() : 0); };
inline  int                         getH()                  { return (bValid ?  readBgr.h.load() : 0); };
inline  int                         getD()                  { return (bValid ?  readBgr.d.load() : 0); };

        void                        getRB( struct readBackground* );

};
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#endif


