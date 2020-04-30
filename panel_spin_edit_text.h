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
            
            vec2        vCentre;
       PanelSimple*     pCadran;
            
public:
                        PanelSpinEditText();
                        
inline      void        set_val(float f)            { val = f; }                        
inline      void        set_min(float f)            { min = f; }                        
inline      void        set_max(float f)            { max = f; }                        
inline      void        set_step(float f)           { step = f; }                        
inline      void        set_nb(float f)             { nb = f; }                        
inline      void        set(float m, float M, float s, float n)
                        { min = m; max = M; step = s; nb = n; }                        
                        
	virtual void		clickLeft( int, int );
	virtual void		motionLeft( int, int );
	virtual void		releaseLeft( int, int );

    virtual void		updatePos();

};

#endif
