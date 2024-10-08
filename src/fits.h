#ifndef FITS_H
#define FITS_H  1

#include "main.h"
#include "panel_fits.h"
#include "panel_capture.h"
#include "panel_fits_correction.h"
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
        
        PanelFits*					pPanelFits;
        PanelCapture*				pPanelCapture;
        PanelCorrectionFits*		pPanelCorrectionFits;
        
        bool						bFlip;   // (0,0) Bottom, Left
        bool						bEOF;
                
        int                         nBITPIX;
        int                         nNAXIS;
        vector<int>					nNAXISn;
        int                         nDATE;

        string                      sCTYPE1;
        string                      sCTYPE2;
        
        double						dMin;
        double						dMax;
        
volatile double                     dCRVAL1;
volatile double                     dCRVAL2;

volatile double                     dCRPIX1;
volatile double                     dCRPIX2;
        
        mat2						mAstroEchl;
        mat2						mAstroTrns;
        mat2						mMat;

        double                      dCD1_1;
        double                      dCD1_2;
        double                      dCD2_1;
        double                      dCD2_2;
        
volatile double                     dCDELT1;
volatile double                     dCDELT2;

        double                      dPC1_1;
        double                      dPC1_2;
        double                      dPC2_1;
        double                      dPC2_2;
        
        double						dBZERO;
        double						dBSCALE;
        
        int16_t						iOFFSET;
public :
        Fits(string, PanelCapture*);
        ~Fits();
        
        void                        chargeFits();
        bool						intersection( vec2&, vec2&, vec2&, vec2&, vec2& );
        void						intersectionH( vec2&, vec2&, vec2& );
        void						intersectionB( vec2&, vec2&, vec2& );
        void						intersectionG( vec2&, vec2&, vec2& );
        void						intersectionD( vec2&, vec2&, vec2& );
        double                      computeEchelle(vec2);
        void                        sauveMatrice();
        void                        chargeHDU(int);
        void						read_RGB_8(  float&, uint8_t* );
        void						read_RGB_16( float&, int16_t* );
        void                        chargeTexture(int);
        
        int                         getInt( string );
        double                      getDouble( string );
        string                      getString( string );
        
        void                        readBITPIX( string );
        void                        readNAXIS( string, string );
        void                        readCR( string, string );
        void                        readCDELT( string, string );
        void                        readCD( string, string );
        void                        readPC( string, string );
        void                        readCTYPE( string, string );

        void                        afficheDic();
		bool						haveKey( string );
        void                        afficheDatas();
		void						afficheInfoFits();
		void						afficheInfoFits(bool);
		void						iconize();
		void						restaure(bool); 	//bAffInfoFits,
		
inline  GLubyte*                    getPTR()                { return (bValid ?  readBgr.ptr.load() : NULL); };
inline  int                         getW()                  { return (bValid ?  readBgr.w.load() : 0); };
inline  int                         getH()                  { return (bValid ?  readBgr.h.load() : 0); };
inline  int                         getD()                  { return (bValid ?  readBgr.d.load() : 0); };
inline  PanelFits*                  getPanelFits()          { return pPanelFits; };
inline  mat2&						getMatrix() 			{ return mMat; };
inline  void 						setPanelCapture(PanelCapture* p) 			{ pPanelCapture = p; };
inline  PanelCorrectionFits*		getPanelCorrectionFits(){ return pPanelCorrectionFits; }

        void                        getRB( struct readBackground* );
        void						tex_2_J2000( vec2, vec2& );
        void						J2000_2_tex( vec2, vec2& );

inline	double                      getNAXIS1()				{ return nNAXISn[0]; }
inline	double                      getNAXIS2()				{ return nNAXISn[1]; }
inline	double                      getCRVAL1()				{ return dCRVAL1; }
inline	double                      getCRVAL2()				{ return dCRVAL2; }
inline	double                      getCRPIX1()				{ return dCRPIX1; }
inline	double                      getCRPIX2()				{ return dCRPIX2; }
inline	double                      getCDELT1()				{ return dCDELT1; }
inline	double                      getCDELT2()				{ return dCDELT2; }

};
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#endif


