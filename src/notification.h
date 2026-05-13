#ifndef NOTIFICATION_H
#define NOTIFICATION_H  1


//----------------------------------------------------------------------------------------------
class Notification
{
public:
	virtual void	notifie( unsigned, void*  )		{}
	virtual int		getInfo()						{ return -1; }
};
//----------------------------------------------------------------------------------------------


#endif
