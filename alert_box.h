#ifndef ALERT_BOX_H
#define ALERT_BOX_H  1

#include "main.h"

#include <string>
#include <WindowsManager.h>


class AlertBox
{
protected:
    PanelWindow*                pW;
    PanelText*                  pT;
    PanelText*                  pRetour;

    string                      sMessage;

public :
    AlertBox(string);

    void                        quit();

};


#endif
