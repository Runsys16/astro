#ifndef CONVERT_H
#define CONVERT_H

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
class Convert
{
public:
						Convert()							{};
						~Convert()							{};    
    
virtual void			screen_2_tex( vec2& )				{};
virtual void			screen_2_panel( vec2& )				{};
virtual void			tex_2_screen( vec2& )				{};
virtual void			tex_2_panel( vec2& )				{};
virtual void			panel_2_tex( vec2& )				{};
virtual void			panel_2_screen( vec2& )				{};

};


#endif
