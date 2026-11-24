#ifndef CTOOLS_H
#define CTOOLS_H

#include <sys/time.h>
#include <time.h>
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//
// Force le lancement de la commande 'g' sur arduino 
// voir idleGL()
//
//--------------------------------------------------------------------------------------------------------------------
void		get_info_arduino();
double		hms2rad(struct hms& HMS);
double		dms2rad(struct dms& DMS);
void		rad2hms(double r, struct hms& HMS);
void		rad2dms(double v, struct dms& DMS);
char*		deg2str_hms(double r, char* str, int len);
char*		deg2str_dms(double d, char* str, int len);
void		deg2dms(double v, struct dms& DMS);
void		deg2hms(double v, struct hms& HMS);
void		exit_handler();

#ifdef TARGETARM
void		idle();
#endif




//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#endif
