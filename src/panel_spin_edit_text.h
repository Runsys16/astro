#ifndef PANEL_SPIN_EDIT_TEXT_H
#define PANEL_SPIN_EDIT_TEXT_H


#include "main.h"


typedef void (*motion_cb_t)(int,int);

class PanelSpinEditText : public PanelEditText
{
protected:
            float       min;
            float       max;
            float       step;
            float       nb;
            float       val_angle;
            float       val;
            float       angle;
            
            float*      pVal;
            
            int         delta_x;
            int         delta_y;
            int         nDecimal;
            
            vec2        vCentre;
            vec3        vRef;
       PanelEditText* 	pEditCopy;
       PanelSimple*     pCadran;
       PanelSimple*     pBoule;
       Panel*			pPrevParent;
       
            vector<float>   t_val;
            int         x_click;
            int         y_click;
            motion_cb_t cb_motion;
            
public:
                        PanelSpinEditText();
                        
inline      void        set_pVal(float* v)          { pVal = v; }                        
inline      void        set_delta(int x, int y)     { delta_x = 0; delta_y = 0; }                        
inline      void        set_val(float f)            { val_angle = val = f; }                        
inline      void        set_min(float f)            { min = f; }                        
inline      void        set_max(float f)            { max = f; }                        
inline      void        set_step(float f)           { step = f; }                        
inline      void        set_nb(float f)             { nb = f; }                        
inline      void        set_ndecimal(int n)         { nDecimal = n; }                        
inline      void        set(float m, float M, float s, float n)
							                        { min = m; max = M; step = s; nb = n; }     

inline 		void		setMotion( motion_cb_t cb)               { cb_motion = cb; }
inline      float*      get_pVal()          		{ return pVal; }                        


            void        set_enum(vector<float>);
                        
            void        boule_pos(int, int);                   
            void        compute_pos(int, int);                   
            void        compute_pos_relatif(int, int);                   
                        
			void		ajusteDelta( int, int );
            void        clampVal();
            
	virtual void		clickLeft( int, int );
	virtual void		motionLeft( int, int );
	virtual void		releaseLeft( int, int );

	virtual void		clickRight( int, int );
	virtual void		releaseRight( int, int );

    virtual void		updatePos();
    virtual void		displayGL();

};

#endif
