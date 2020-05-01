#ifndef PANEL_SPIN_EDIT_TEXT_H
#define PANEL_SPIN_EDIT_TEXT_H


#include "main.h"



class PanelSpinEditText : public PanelEditText
{
protected:
            float       min;
            float       max;
            float       step;
            float       nb;
            float       val;
            float       angle;
            
            int         delta_x;
            int         delta_y;
            
            vec2        vCentre;
       PanelSimple*     pCadran;
       PanelSimple*     pBoule;
            
public:
                        PanelSpinEditText();
                        
inline      void        set_delta(int x, int y)     { delta_x = x; delta_y = y; }                        
inline      void        set_val(float f)            { val = f; }                        
inline      void        set_min(float f)            { min = f; }                        
inline      void        set_max(float f)            { max = f; }                        
inline      void        set_step(float f)           { step = f; }                        
inline      void        set_nb(float f)             { nb = f; }                        
inline      void        set(float m, float M, float s, float n)
                        { min = m; max = M+s; step = s; nb = n; }     
                        
            void        compute_pos(int, int);                   
                        
	virtual void		clickLeft( int, int );
	virtual void		motionLeft( int, int );
	virtual void		releaseLeft( int, int );

    virtual void		updatePos();

};

#endif
